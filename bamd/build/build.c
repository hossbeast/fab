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
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xepoll.h"
#include "xlinux/xepoll.h"
#include "xlinux/xwait.h"
#include "xlinux/xunistd.h"

#include "build.h"
#include "buildplan.h"
#include "config.h"
#include "build_slot.h"
#include "fsent.h"
#include "params.h"
#include "handler.h"
#include "exec_builder.h"
#include "buildplan_entity.h"
#include "dependency.h"
#include "system.h"
#include "probes.h"
#include "goals.h"

#include "atomics.h"
#include "macros.h"
#include "locks.h"
#include "common/hash.h"
#include "common/attrs.h"
#include "events.h"
#include "zcat.h"
#include "marshal.h"
#include "rcu.h"
#include "times.h"
#include "stats.h"
#include "vertex.h"

union stats_record_build build_stats;
moria_vertex *g_build = (moria_vertex[]){{ }};
char g_build_id[40];
uint16_t g_build_id_len;

// internal
int build_devnull_fd = -1;
build_slot *build_slots;
hashtable build_slots_bypid;
int build_slots_bypid_lock;

// local
static int16_t concurrency;
static int epfd = -1;
static uint64_t stage_index_sum;
static int64_t stage_index_sum_target;
static selected *sn_first;
static uint16_t build_stage;
static uint16_t build_numranks;
static bool build_autorun;
static uint32_t build_id;
static pid_t build_pid;
static uint64_t build_token;
static char build_label[128];
static uint16_t build_label_len;

/*
 * closed form calcuation of the sum of integers [1 .. n]
 */
static uint64_t sum_first(uint64_t n)
{
    return (n * (n + 1)) >> 1;
}

static uint32_t build_slot_hash(uint32_t h, const void * e)
{
  const build_slot *bs = e;
  return bs->pid;
}

static int build_slot_cmp(const void * A, const void * B)
{
  const build_slot * Abs = A;
  const build_slot * Bbs = B;

  return INTCMP(Abs->pid, Bbs->pid);
}

static void build_event_start(void)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  event_bacon *msg;
  events_context ectx;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_BUILD_START))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("build-start")
    { SET
      { MAPPING STRING("id") UINT(build_id) }
      { MAPPING STRING("token") HEX64(build_token) }
      { MAPPING STRING("label") BYTES(build_label, build_label_len) }
      { MAPPING STRING("pid") UINT(build_pid) }

      { MAPPING STRING("autorun") BOOL(build_autorun) }
      { MAPPING STRING("slots") UINT(buildplan_selection.selected_entities.size) }
      { MAPPING STRING("stages") UINT(buildplan_selection.numranks) }
    }
  }

  msg->id = 0;
  msg->size = fixed.l;
  events_publish(&ectx);
}

static void build_event_end(uint32_t slot_pass, uint32_t slot_fail, uint32_t slot_prep_fail)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  event_bacon *msg;
  events_context ectx;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_BUILD_END))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("build-end")
    { SET
      { MAPPING STRING("id") UINT(build_id) }
      { MAPPING STRING("token") HEX64(build_token) }
      { MAPPING STRING("label") BYTES(build_label, build_label_len) }
      { MAPPING STRING("pid") UINT(build_pid) }
      { MAPPING STRING("autorun") BOOL(build_autorun) }
      { MAPPING STRING("success") BOOL(slot_fail == 0 && slot_prep_fail == 0) }
      { MAPPING STRING("slot-pass") UINT(slot_pass) }
      { MAPPING STRING("slot-fail") UINT(slot_fail) }
      { MAPPING STRING("slot-prep-fail") UINT(slot_prep_fail) }
    }
  }

  msg->id = 0;
  msg->size = fixed.l;
  events_publish(&ectx);
}

static void build_init()
{
  int x;
  union epoll_data data;
  struct epoll_event ev;

  // setup epoll
  epfd = xepoll_create();

  // setup pipes
  xmalloc(&build_slots, sizeof(*build_slots) * concurrency);
  for(x = 0; x < concurrency; x++)
  {
    xpipe(build_slots[x].stdout_pipe);
    xpipe(build_slots[x].stderr_pipe);
    xpipe(build_slots[x].auxout_pipe);

    exec_builder_init(&build_slots[x].exec_builder);
    exec_render_context_init(&build_slots[x].exec_builder_context);

    data = (typeof(data)){ 0 };
    ev.events = EPOLLIN;

    data.u32 = (x << 16) | 1;
    ev.data = data;
    xepoll_ctl(epfd, EPOLL_CTL_ADD, build_slots[x].stdout_pipe[0], &ev);

    data.u32 = (x << 16) | 2;
    ev.data = data;
    xepoll_ctl(epfd, EPOLL_CTL_ADD, build_slots[x].stderr_pipe[0], &ev);

    data.u32 = (x << 16) | 1001;
    ev.data = data;
    xepoll_ctl(epfd, EPOLL_CTL_ADD, build_slots[x].auxout_pipe[0], &ev);

    bacon_parser_init(&build_slots[x].sysvar_parser);
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

    free(build_slots[x].argv_stor);
    free(build_slots[x].env_stor);
    exec_builder_destroy(&build_slots[x].exec_builder);
    exec_render_context_destroy(&build_slots[x].exec_builder_context);

    bacon_value_free(&build_slots[x].sysvar_parser, build_slots[x].sysvars);
    build_slots[x].sysvars = 0;
    bacon_parser_destroy(&build_slots[x].sysvar_parser);
  }
  ifree(&build_slots);

  hashtable_destroy(&build_slots_bypid);

  ixclose(&epfd);
}

/* always/only called on the run thread */
void build(rcu_thread * restrict rcu_self, bool autorun, pid_t pid, uint64_t token, const char * restrict label, uint16_t label_len)
{
  sigset_t sigs;
  siginfo_t siginfo;
  int rv;
  uint32_t stage_index;
  buildplan_entity * bpe;
  ptrdiff_t slot_index;
  uint32_t slot;
  uint32_t stream;
  struct epoll_event events[64];
  int x;
  selected *sn;
  build_slot *bs, key;
  uint32_t slot_prep_fail;
  uint32_t slot_fail;
  uint32_t slot_pass;
  rcu_thread rcu;

  build_label_len = zcatw(build_label, sizeof(build_label), label, label_len);
  build_pid = pid;
  build_token = token;
  build_autorun = autorun;
  build_stage = 0;
  slot_prep_fail = 0;
  slot_fail = 0;
  slot_pass = 0;
  stage_index_sum = 0;
  stage_index_sum_target = -1;
  sn_first = llist_first(&buildplan_selection.list, typeof(*sn_first), lln);
  build_numranks = buildplan_selection.numranks;

  rcu_enter(&rcu);

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);
  sigdelset(&sigs, SIGCHLD);

  build_stats.build++;
  build_id++;
  build_event_start();

  stage_index = 0;
  slot = 0;

  sn = 0;
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu);

    /* build is complete */
    if(build_stage == build_numranks) {
      break;
    }

    /* build has failed, all slots have exited  */
    if((slot_prep_fail || slot_fail) && build_slots_bypid.size == 0) {
      break;
    }

    // launch up to max concurrency
    while(!g_params.shutdown && build_slots_bypid.size < concurrency)
    {
      rcu_quiesce(&rcu);

      // stage has failed
      if(slot_prep_fail || slot_fail) {
        break;
      }

      // pick up the first stage
      if(sn == 0 && sn_first) {
        sn = sn_first;
        sn_first = 0;
        build_stage_probe(0);

        system_state_change(SYSTEM_STATE_BUILDING);
      }

      if(!sn) { // no work
        break;
      }

      if(sn->rank > build_stage) {
        break;
      }

      if(sn->type == SELECTED_VERTEX)
      {
        /* this is a placeholder node without an attached formula ; it becomes fresh once the
         * preceeding stages, which contain its dependencies, have completed */
        vertex_mark_fresh(sn->v);
        goto next;
      }

      // locate an open slot
      while(build_slots[slot].pid) {
        slot = (slot + 1) % concurrency;
      }

      bs = &build_slots[slot];
      bpe = sn->bpe;

      /* launch the build slot */
      bs->stage = build_stage;
      build_slot_fork_and_exec(bs, bpe, stage_index);
      if(system_has_error) {
        slot_prep_fail++;
        break;
      }

      hashtable_put(&build_slots_bypid, bs);

      slot_index = bs - build_slots;
      build_fork_probe(build_stage, slot_index, bs);

      // cycle to the next slot
      stage_index++;

    next:
      sn = llist_next(&buildplan_selection.list, sn, lln);

      /* this becomes true when the current stage has been fully launched */
      if(!sn || sn->rank != build_stage)
      {
        stage_index_sum_target = sum_first(stage_index);
      }
    }

    /* shortcut in case zero slots were launched */
    if(build_slots_bypid.size == 0) {
      break;
    }

    // receive epoll events, or a signal
    rv = uxepoll_pwait(epfd, events, sizeof(events) / sizeof(*events), SEC_AS_MSEC(1), &sigs);

    /* read slots */
    for(x = 0; x < rv; x++)
    {
      rcu_quiesce(&rcu);

      slot = events[x].data.u32 >> 16;
      stream = events[x].data.u32 & 0xFFFF;

      bs = &build_slots[slot];
      build_slot_read(bs, stream);
    }

    /* reap children */
    while(!g_params.shutdown)
    {
      rcu_quiesce(&rcu);

      /* NOTE : this thread is competing with any other threads calling wait */
      uxwaitid(P_ALL, tid, &siginfo, WEXITED | WNOHANG | WNOWAIT);
      if(siginfo.si_pid == 0) {
        break;
      }

      key = (typeof(key)){ pid : siginfo.si_pid };
      bs = hashtable_get(&build_slots_bypid, &key);
      if(bs == 0) {
        /* some other child, e.g. from the plugin thread */
        continue;
      }

      /* reap */
      xwaitid(P_PID, siginfo.si_pid, &siginfo, WEXITED | WNOHANG);

      slot_index = bs - build_slots;
      bs->status = siginfo.si_status;
      build_slot_reap(bs);
      build_wait_probe(build_stage, slot_index, bs);

      if(bs->status || bs->stderr_total) {
        slot_fail++;
      } else {
        slot_pass++;
      }

      // free up the slot
      hashtable_delete(&build_slots_bypid, &key);

      bs->pid = 0;
      bs->stdout_total = 0;
      bs->stderr_total = 0;
      bs->auxout_total = 0;

      // check for end-of-stage
      stage_index_sum += (bs->stage_index + 1);
      if(stage_index_sum == stage_index_sum_target)
      {
        build_stage_probe(build_stage + 1);
        build_stage++;
        stage_index = 0;
        stage_index_sum = 0;
        stage_index_sum_target = -1;
      }
    }
  }

  if(slot_prep_fail || slot_fail) {
    build_stats.build_error++;
  } else {
    build_stats.build_ok++;
  }
  build_event_end(slot_pass, slot_fail, slot_prep_fail);

  system_state_change(SYSTEM_STATE_OK);
  rcu_leave(&rcu);
}

//
// public
//

void build_setup()
{
  build_devnull_fd = xopens(O_RDWR, "/dev/null");
  build_init();

  moria_vertex_initw(g_build, &g_graph, VERTEX_BUILD | VERTEX_OK, MMS("(build)"));
  g_build_id_len = zcatf(g_build_id, sizeof(g_build_id), "%016"PRIx64, hash64(0x4567, MMS("(build)")));
}

void build_cleanup()
{
  ixclose(&build_devnull_fd);

  build_destroy();
}

void build_reconfigure(config * restrict cfg, bool dryrun)
{
  if(dryrun) {
    return;
  }

  build_destroy();

  // apply new config
  concurrency = cfg->build.concurrency.u16;
  if(concurrency == 0) {
    concurrency = g_params.procs;
  }

  // relaunch
  build_init();

  return;
}

void build_stats_build(bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&build_stats.record, &descriptor_stats_record_build, builder_writer);
}

void build_describe(struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(g_build, builder_writer);
    { SET
      vertex_mappings(g_build, builder_writer);
      { MAPPING
        STRING("stats")
        { SET
          build_stats_build(false, builder_writer);
        }
      }
    }
  }
}

size_t build_id_cat(char *dst, size_t sz)
{
  return zcatw(dst, sz, g_build_id, g_build_id_len);
}
