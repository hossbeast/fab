/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "types.h"

#include "bacon/builder.h"
#include "bam/ipc.h"
#include "bam/events.h"
#include "bam/sigutil.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"
#include "moria/edge.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xpthread.h"
#include "xlinux/xtime.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xepoll.h"
#include "xlinux/xwait.h"
#include "xlinux/xunistd.h"

#include "build.h"
#include "build/events.h"
#include "buildplan.h"
#include "config.h"
#include "build_slot.h"
#include "fsent.h"
#include "params.h"
#include "request_handler.h"
#include "exec_builder.h"
#include "bpe.h"
#include "dependency.h"
#include "system.h"
#include "probes.h"
#include "goals.h"
#include "formula_record.h"

#include "atomics.h"
#include "macros.h"
#include "locks.h"
#include "common/hash.h"
#include "common/attrs.h"
#include "events/events.h"
#include "zcat.h"
#include "marshal.h"
#include "rcu.h"
#include "times.h"
#include "stats.h"
#include "vertex.h"
#include "build_record.h"
#include "meta_edge.h"
#include "shadow.h"
#include "subsys.h"

union stats_record_build build_stats;
subsys sys_build;

// internal
int build_devnull_fd = -1;
build_slot *build_slots;
hashtable build_slots_bypid;
int build_slots_bypid_lock;

#define BUILD_BUFFER_SIZE_MIN 4096
#define BUILD_BUFFER_SIZE_MAX UINT16_MAX

uint32_t build_system_stdout_size;
uint32_t build_system_stderr_size;
uint32_t build_system_auxout_size;
uint32_t build_system_retention_max_entries;

// local
static llist slot_freelist;
static llist slot_waitlist;
static int16_t concurrency;
static int epfd = -1;
static uint64_t stage_index_sum;
static int64_t stage_index_sum_target;
static uint16_t build_stage;
static uint16_t build_numranks;

/*
 * closed form calcuation of the sum of integers [1 .. n]
 */
static uint64_t sum_first(uint64_t n)
{
    return (n * (n + 1)) >> 1;
}

static uint32_t build_slot_hash(uint32_t h, const void *e)
{
  const build_slot *bs = e;
  return hash32(0, MM(bs->rec->pid));
}

static int build_slot_cmp(const void *A, const void *B)
{
  const build_slot * Abs = A;
  const build_slot * Bbs = B;

  return INTCMP(Abs->rec->pid, Bbs->rec->pid);
}

struct build_slot_search_context {
  hashtable_search_context htx;
  int64_t pid;
};

static int build_slot_key_cmp(const void *A, const void *K)
{
  const build_slot *Abs = A;
  const struct build_slot_search_context *ctx = K;

  return INTCMP(Abs->rec->pid, ctx->pid);
}

static void build_init()
{
  int x;
  union epoll_data data;
  struct epoll_event ev;
  build_slot *slot;
  int flags;

  llist_init(&slot_freelist);
  llist_init(&slot_waitlist);

  // setup epoll
  epfd = xepoll_create();

  // setup pipes
  xmalloc(&build_slots, sizeof(*build_slots) * concurrency);
  for(x = 0; x < concurrency; x++)
  {
    slot = &build_slots[x];
    xpipe(slot->stdout_pipe);
    xpipe(slot->stderr_pipe);
    xpipe(slot->auxout_pipe);

    flags = fcntl(slot->stdout_pipe[0], F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(slot->stdout_pipe[0], F_SETFL, flags);

    flags = fcntl(slot->stderr_pipe[0], F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(slot->stderr_pipe[0], F_SETFL, flags);

    flags = fcntl(slot->auxout_pipe[0], F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(slot->auxout_pipe[0], F_SETFL, flags);

    exec_builder_init(&slot->exec_builder);
    exec_render_context_init(&slot->exec_builder_context);

    data = (typeof(data)){ };
    ev.events = EPOLLIN;

    data.u32 = (x << 16) | 1;
    ev.data = data;
    xepoll_ctl(epfd, EPOLL_CTL_ADD, slot->stdout_pipe[0], &ev);

    data.u32 = (x << 16) | 2;
    ev.data = data;
    xepoll_ctl(epfd, EPOLL_CTL_ADD, slot->stderr_pipe[0], &ev);

    data.u32 = (x << 16) | 1001;
    ev.data = data;
    xepoll_ctl(epfd, EPOLL_CTL_ADD, slot->auxout_pipe[0], &ev);

    bacon_parser_init(&slot->sysvar_parser);
    llist_push(&slot_freelist, &slot->lln);
  }

  hashtable_initx(
      &build_slots_bypid
    , concurrency
    , build_slot_hash
    , build_slot_cmp
    , 0
  );
}

static void build_destroy()
{
  int x;

  for(x = 0; x < concurrency; x++)
  {
    xclose(build_slots[x].stdout_pipe[0]);
    xclose(build_slots[x].stdout_pipe[1]);
    xclose(build_slots[x].stderr_pipe[0]);
    xclose(build_slots[x].stderr_pipe[1]);
    xclose(build_slots[x].auxout_pipe[0]);
    xclose(build_slots[x].auxout_pipe[1]);

    exec_builder_destroy(&build_slots[x].exec_builder);
    exec_render_context_destroy(&build_slots[x].exec_builder_context);

    bacon_parser_destroy(&build_slots[x].sysvar_parser);
  }

  ifree(&build_slots);
  llist_init(&slot_freelist);

  hashtable_destroy(&build_slots_bypid);

  ixclose(&epfd);
}

/* always/only called on the run thread */
void build(bool autorun, pid_t pid, uint64_t token, const char * restrict label, uint16_t label_len)
{
  sigset_t sigs;
  siginfo_t siginfo;
  int rv;
  uint32_t stage_index;
  ptrdiff_t slot_index;
  uint32_t slot;
  uint32_t stream;
  struct epoll_event events[64];
  int x;
  selected *sn;
  build_slot *bs = NULL;
  formula_record *rec;
  uint32_t slot_prep_fail;
  uint32_t slot_fail;
  uint32_t slot_pass;
  rcu_thread rcu;
  struct build_slot_search_context sctx;
  build_record *build_rec;

  build_stage = 0;
  slot_prep_fail = 0;
  slot_fail = 0;
  slot_pass = 0;
  stage_index_sum = 0;
  stage_index_sum_target = -1;
  build_numranks = buildplan_selection.numranks;

  rcu_enter(&rcu);

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);
  sigdelset(&sigs, SIGCHLD);

  /* allocate a new build record */
  build_rec = build_record_alloc();
  build_rec->token = token;
  build_rec->pid = pid;
  build_rec->autorun = autorun;
  build_rec->label_len = zcatw(build_rec->label, sizeof(build_rec->label), label, label_len);

  build_event_start(build_rec);

  stage_index = 0;
  slot = 0;

  /* start off with the first stage */
  sn = llist_first(&buildplan_selection.list, typeof(*sn), lln);
  build_stage_probe(0);
  system_state_change(SYSTEM_STATE_BUILDING);

  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu);

    /* build has completed */
    if(build_stage == build_numranks) {
      break;
    }

    /* build has failed and all slots have exited  */
    if((slot_prep_fail || slot_fail) && build_slots_bypid.size == 0 && llist_empty(&slot_waitlist)) {
      break;
    }

    // launch up to max concurrency
    while(!g_params.shutdown && (build_slots_bypid.size < concurrency) && llist_empty(&slot_waitlist))
    {
      rcu_quiesce(&rcu);

      // stage has failed
      if(slot_prep_fail || slot_fail) {
        break;
      }

      /* current stage is fully launched */
      if(!sn || (sn->rank != build_stage)) {
        break;
      }

      /* this is a placeholder node without an attached formula ; it becomes fresh once the
       * preceeding stages, which contain its dependencies, have completed */
      if(sn->type == SELECTED_VERTEX) {
        build_event_slot_ok(0);
        vertex_mark_fresh(sn->v);
        goto next_slot;
      }

      /* next slot */
      bs = llist_shift(&slot_freelist, typeof(*bs), lln);

      bs->bpe = sn->bpe;
      rec = formula_record_alloc(build_rec, bs->bpe);
      bs->rec = rec;
      adaptor_init(&bs->stdout_adaptor, rec->stdout_buf, rec->stdout_size, &rec->stdout_head, &rec->stdout_tail);
      adaptor_init(&bs->stderr_adaptor, rec->stderr_buf, rec->stderr_size, &rec->stderr_head, &rec->stderr_tail);
      adaptor_init(&bs->auxout_adaptor, rec->auxout_buf, rec->auxout_size, &rec->auxout_head, &rec->auxout_tail);

      /* setup the build record */
      rec->stage = build_stage;
      rec->stage_index = stage_index;

      /* launch the build slot */
      build_slot_fork_and_exec(bs);
      if(system_has_error) {
        /* this can happen when the build slot is executing a formula which has args or
         * envs bound to selectors, and those selectors fail to resolve */
        slot_prep_fail++;
        break;
      }

      hashtable_put(&build_slots_bypid, bs);

      slot_index = bs - build_slots;
      build_fork_probe(build_stage, slot_index, bs);

      // cycle to the next slot
      stage_index++;

    next_slot:
      sn = llist_next(&buildplan_selection.list, sn, lln);

      /* this becomes true when the current stage has been fully launched */
      if(!sn || sn->rank != build_stage) {
        stage_index_sum_target = sum_first(stage_index);
        break;
      }
    }

    // if any children are in flight, block on epoll events, or signal
    rv = 0;
    if(build_slots_bypid.size > 0) {
      rv = uxepoll_pwait(epfd, events, sizeof(events) / sizeof(*events), MSEC_AS_MSEC(125), &sigs);
    }

    /* read slots */
    for(x = 0; x < rv; x++)
    {
      rcu_quiesce(&rcu);

      slot = events[x].data.u32 >> 16;
      stream = events[x].data.u32 & 0xFFFF;

      bs = &build_slots[slot];
      build_slot_read(bs, stream);
    }

    /* consume children */
    while(!g_params.shutdown)
    {
      rcu_quiesce(&rcu);

      /* NOTE : this thread is competing with any other threads calling wait (plugin_thread)
       *  WEXITED - check for exited child processes
       *  WNOHANG - dont block
       *  WNOWAIT - the event is not consumed by this call */
      uxwaitid(P_ALL, tid, &siginfo, WEXITED | WNOHANG | WNOWAIT);
      if(siginfo.si_pid == 0) {
        break;
      }

      /* lookup this pid in the table */
      memset(&sctx, 0, sizeof(sctx));
      sctx.pid = siginfo.si_pid;
      sctx.htx.hash = hash32(0, MM(sctx.pid));

      sctx.htx.cmp_fn = build_slot_key_cmp;
      sctx.htx.entry = &sctx;
      if(hashtable_search(&build_slots_bypid, &sctx.htx)) {
        bs = hashtable_fetch(&build_slots_bypid, &sctx.htx);
      }

      if(bs == 0) {
        /* some other child, e.g. from the plugin thread
         * the plugin thread will consume this event */
        continue;
      }

      /* pid is in the table ; consume the event */
      xwaitid(P_PID, siginfo.si_pid, &siginfo, WEXITED | WNOHANG);

      hashtable_remove(&build_slots_bypid, &sctx.htx);

      slot_index = bs - build_slots;
      bs->rec->status = siginfo.si_status;
      build_wait_probe(build_stage, slot_index, bs);

      /* dont reap until all fds have been read to the end */
      llist_push(&slot_waitlist, &bs->lln);
    }

    do {
      if((bs = llist_shift(&slot_waitlist, typeof(*bs), lln)) == 0) {
        break;
      }

      if(build_slot_read(bs, 1) != 0 ||
         build_slot_read(bs, 2) != 0 ||
         build_slot_read(bs, 1001) != 0) {
        llist_push(&slot_waitlist, &bs->lln);
        break;
      }

      /* ok to reap after all streams have drained */
      slot_index = bs - build_slots;
      build_reap_probe(build_stage, slot_index, bs);
      build_slot_reap(bs);

      if(bs->rec->status || bs->rec->stderr_total) {
        slot_fail++;
      } else {
        slot_pass++;
      }

      /* remove the build record from the slot, append it to the bpe */
      rec = bs->rec;
      bs->rec = 0;
      bpe_link_record(bs->bpe, build_rec, rec);

      // free up the slot
      llist_push(&slot_freelist, &bs->lln);

      stage_index_sum += (rec->stage_index + 1);
    } while(0);

    /* next stage */
    if(stage_index_sum != stage_index_sum_target) {
      continue;
    }

    build_stage_probe(build_stage + 1);
    build_stage++;
    stage_index = 0;
    stage_index_sum = 0;
    stage_index_sum_target = -1;
  }

  build_rec->slot_pass = slot_pass;
  build_rec->slot_fail = slot_fail;
  build_rec->slot_prep_fail = slot_prep_fail;

  build_event_end(build_rec);
  build_record_add(build_rec);

  if(slot_prep_fail || slot_fail) {
    build_event_error();
    system_state_change(SYSTEM_STATE_ERR);
  } else {
    build_event_ok();
    system_state_change(SYSTEM_STATE_OK);
  }

  rcu_leave(&rcu);
}

//
// public
//

void build_setup()
{
  build_devnull_fd = xopens(O_RDWR, "/dev/null");

  subsys_init(&sys_build, VERTEX_SYS_BUILD);

  shadow_mkref(g_shadow_bm, MMS("sys-build"), &sys_build.vertex);

  llist_init(&slot_freelist);
  llist_init(&slot_waitlist);
}

void build_cleanup()
{
  ixclose(&build_devnull_fd);

  build_destroy();

  RUNTIME_ASSERT(llist_empty(&slot_freelist));
  RUNTIME_ASSERT(llist_empty(&slot_waitlist));
}

void build_reconfigure(config * restrict cfg, bool dryrun)
{
  config_param param;

  if(dryrun) {
    /* sane buffer sizes */
    param.min = BUILD_BUFFER_SIZE_MIN;
    param.max = BUILD_BUFFER_SIZE_MAX;

    param.config_path = "build.stdout-buffer-size";
    param.val = cfg->build.stdout_buffer_size.v;
    config_validate(&param);

    param.config_path = "build.stderr-buffer-size";
    param.val = cfg->build.stderr_buffer_size.v;
    config_validate(&param);

    param.config_path = "build.auxout-buffer-size";
    param.val = cfg->build.auxout_buffer_size.v;
    config_validate(&param);
    return;
  }

  build_destroy();

  // apply new config
  concurrency = cfg->build.concurrency.u16;
  if(concurrency == 0) {
    concurrency = g_params.procs;
  }

  build_system_stdout_size = cfg->build.stdout_buffer_size.u32;
  build_system_stderr_size = cfg->build.stderr_buffer_size.u32;
  build_system_auxout_size = cfg->build.auxout_buffer_size.u32;
  build_system_retention_max_entries = cfg->build.retention.max_entries.u32;

  /* required parameters */
  RUNTIME_ASSERT(concurrency);
  RUNTIME_ASSERT(build_system_stdout_size);
  RUNTIME_ASSERT(build_system_stderr_size);
  RUNTIME_ASSERT(build_system_auxout_size);

  build_init();
}

void build_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&build_stats.record, &descriptor_stats_record_build, builder_writer);
}

void build_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      vertex_mappings(v, builder_writer);
      { MAPPING STRING("concurrency")
        INT(concurrency)
      }
      { MAPPING
        STRING("stats")
        { SET
          build_sys_stats_build(v, false, builder_writer);
        }
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}
