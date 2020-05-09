/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "xapi.h"
#include "types.h"

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/config.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"
#include "moria/edge.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xepoll.h"
#include "xlinux/xwait.h"

#include "build_thread.internal.h"
#include "CONFIG.errtab.h"
#include "buildplan.h"
#include "config.internal.h"
#include "build_slot.h"
#include "epoll_attrs.h"
#include "logging.h"
#include "node.h"
#include "params.h"
#include "path.h"
#include "formula.h"
#include "config.internal.h"
#include "exec_builder.h"

#include "atomic.h"
#include "macros.h"
#include "spinlock.h"
#include "hash.h"
#include "attrs.h"
#include "box.h"

// configuration
static build_thread_config staging_cfg;
build_thread_config build_thread_cfg;

// internal
int build_devnull_fd = -1;
build_slot * build_slots;
hashtable * build_slots_bypid;
int build_slots_bypid_lock;
bool build_stage_failure;

// local
static int epfd = -1;
static bool build_thread_relaunching;
static int build_thread_lock;
static uint64_t stage_index_sum;
static int64_t stage_index_sum_target;
static bool notify_completion;
static selected_node *sn_first;
static uint16_t build_stage;
static uint16_t build_numranks;

attrs32 * stream_part_attrs = (attrs32[]) {{
#define STREAM_PART_DEF(x, n, r, y) + 1
    num : 0 STREAM_PART_TABLE
  , members : (member32[]) {
#undef STREAM_PART_DEF
#define STREAM_PART_DEF(x, n, r, y) { name : #n, value : UINT32_C(y), range : UINT32_C(r) },
STREAM_PART_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(stream_part_attrs);
}

/*
 * closed form calcuation of the sum of integers [1 .. n]
 */
static uint64_t sum_first(uint64_t n)
{
    return (n * (n + 1)) >> 1;
}

static uint32_t build_slot_hash(uint32_t h, const void * e, size_t el)
{
  const build_slot * const * bsp = e;
  return (*bsp)->pid;
}

static int build_slot_cmp(const void * A, size_t Al, const void * B, size_t Bl)
{
  const build_slot * const * Absp = A;
  const build_slot * const * Bbsp = B;

  return INTCMP((*Absp)->pid, (*Bbsp)->pid);
}

static void build_show_settings(const struct config_formula_show_settings * restrict settings, struct show_settings * restrict config)
{
  box_bool_setif(settings->show_path, &config->show_path);
  box_bool_setif(settings->show_command, &config->show_command);
  box_bool_setif(settings->show_cwd, &config->show_cwd);
  box_bool_setif(settings->show_arguments, &config->show_arguments);
  box_bool_setif(settings->show_sources, &config->show_sources);
  box_bool_setif(settings->show_targets, &config->show_targets);
  box_bool_setif(settings->show_environment, &config->show_environment);
  box_bool_setif(settings->show_status, &config->show_status);
  box_bool_setif(settings->show_stdout, &config->show_stdout);
  box_int16_setif(settings->show_stdout_limit_bytes, &config->show_stdout_limit_bytes);
  box_int16_setif(settings->show_stdout_limit_lines, &config->show_stdout_limit_lines);
  box_bool_setif(settings->show_stderr, &config->show_stderr);
  box_int16_setif(settings->show_stderr_limit_bytes, &config->show_stderr_limit_bytes);
  box_int16_setif(settings->show_stderr_limit_lines, &config->show_stderr_limit_lines);
  box_bool_setif(settings->show_auxout, &config->show_auxout);
  box_int16_setif(settings->show_auxout_limit_bytes, &config->show_auxout_limit_bytes);
  box_int16_setif(settings->show_auxout_limit_lines, &config->show_auxout_limit_lines);
}

static void build_config(const config * restrict cfg, build_thread_config * restrict config)
{
  box_int16_setif(cfg->build.concurrency, &config->concurrency);
  box_int_setif(cfg->formula.capture_stdout, (int*)&config->capture_stdout);
  box_uint16_setif(cfg->formula.stdout_buffer_size, &config->stdout_buffer_size);
  box_int_setif(cfg->formula.capture_stderr, (int*)&config->capture_stderr);
  box_uint16_setif(cfg->formula.stderr_buffer_size, &config->stderr_buffer_size);
  box_int_setif(cfg->formula.capture_auxout, (int*)&config->capture_auxout);
  box_uint16_setif(cfg->formula.auxout_buffer_size, &config->auxout_buffer_size);

  build_show_settings(&cfg->formula.error, &config->error);
  build_show_settings(&cfg->formula.success, &config->success);

  if(config->concurrency == 0)
  {
    config->concurrency = g_params.procs;
  }
}

static xapi build_thread()
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;
  int rv;
  uint32_t stage_index;
  build_slot * bs;
  buildplan_entity * bpe;
  ptrdiff_t slot_index;
  uint32_t slot;
  uint32_t stream;
  uint16_t numranks = 0;
  struct epoll_event events[64];
  int x;
  selected_node *sn;

  g_params.thread_build = gettid();

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, FABIPC_SIGSCH);
  sigdelset(&sigs, SIGCHLD);

  stage_index = 0;
  slot = 0;

  sn = 0;
  while(!g_params.shutdown && !build_thread_relaunching)
  {
    // launch up to max concurrency
    while(build_slots_bypid->size < build_thread_cfg.concurrency)
    {
      // stage has failed
      if(build_stage_failure) {
        break;
      }

      // pick up the first stage
      if(sn == 0 && sn_first) {
        sn = sn_first;
        sn_first = 0;
        numranks = build_numranks;
        logf(L_BUILDER, "stage begin %3d", 0);
      }

      if(!sn) { // no work
        break;
      }

      if(sn->rank > build_stage) {
        break;
      }

      // locate an open slot
      while(build_slots[slot].pid) {
        slot = (slot + 1) % build_thread_cfg.concurrency;
      }

      bs = &build_slots[slot];
      bpe = sn->bpe;

      if(bpe->fml)
      {
        /* configure the build slot */
        fatal(build_slot_prep, bs, bpe, stage_index);

        /* launch the build slot */
        fatal(build_slot_fork_and_exec, bs);

        fatal(hashtable_put, build_slots_bypid, &bs);

        slot_index = bs - build_slots;
        logf(L_BUILDER, "%6s slot %3"PRIu32" stage %3d index %3d", "launch", (uint32_t)slot_index, build_stage, bs->stage_index);
      }

      // cycle to the next slot
      stage_index++;
      sn = llist_next(&bp_selection.list, sn, lln);
      if(!sn || sn->rank != build_stage)
      {
        stage_index_sum_target = sum_first(stage_index);
      }
    }

    // receive epoll events, or a signal
    fatal(uxepoll_pwait, &rv, epfd, events, sizeof(events) / sizeof(events[0]), -1, &sigs);

    /* read slots */
    for(x = 0; x < rv; x++)
    {
      slot = events[x].data.u32 >> 16;
      stream = events[x].data.u32 & 0xFFFF;

      bs = &build_slots[slot];
      fatal(build_slot_read, bs, stream);
    }

    /* reap children */
    while(true)
    {
      fatal(uxwaitid, P_ALL, 0, &siginfo, WEXITED | WNOHANG);
      if(siginfo.si_pid == 0)
        break;

      build_slot * key = (build_slot[]) {{ pid : siginfo.si_pid }};
      build_slot ** bsp = hashtable_get(build_slots_bypid, &key);
      RUNTIME_ASSERT(bsp);

      bs = *bsp;
      slot_index = bs - build_slots;

      logf(L_BUILDER, "%6s slot %3"PRIu32" stage %3d index %3d status %3d", "reap", (uint32_t)slot_index, build_stage, bs->stage_index, bs->status);
      fatal(build_slot_reap, bs, slot_index, &siginfo);

      if(!bs->success) {
        build_stage_failure = true;
      }

      // free up the slot
      fatal(hashtable_delete, build_slots_bypid, &key);

      bs->pid = 0;
      bs->stdout_buf_len = 0;
      bs->stderr_buf_len = 0;
      bs->auxout_buf_len = 0;

      // check for end-of-stage
      stage_index_sum += (bs->stage_index + 1);
      if(stage_index_sum == stage_index_sum_target)
      {
        logf(L_BUILDER, "stage advance %3d -> %3d", build_stage, build_stage + 1);
        build_stage++;
        stage_index = 0;
        stage_index_sum = 0;
        stage_index_sum_target = -1;
      }

      // check for end-of-build
      if(build_stage == numranks || (build_slots_bypid->size == 0 && build_stage_failure))
      {
        build_stage = numranks + 1;
        sn = 0;
        if(notify_completion)
        {
          fatal(sigutil_tgkill, g_params.pid, g_params.thread_server, FABIPC_SIGSCH);
        }
      }
    }
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * build_thread_main(void * arg)
{
  enter;

  xapi R;
  logger_set_thread_name("build");
  logger_set_thread_categories(L_BUILDER);

  spinlock_engage(&build_thread_lock);
  fatal(build_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd/build");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  spinlock_release(&build_thread_lock);

  if(!build_thread_relaunching)
    syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, FABIPC_SIGSCH);

  return 0;
}

//
// public
//

xapi build_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, build_thread_main, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

xapi build_thread_setup()
{
  enter;

  int x;
  union epoll_data data;
  struct epoll_event ev;

  epfd = -1;

  fatal(xopens, &build_devnull_fd, O_RDWR, "/dev/null");

  // setup epoll
  fatal(xepoll_create, &epfd);

  // setup pipes
  fatal(xmalloc, &build_slots, sizeof(*build_slots) * build_thread_cfg.concurrency);
  for(x = 0; x < build_thread_cfg.concurrency; x++)
  {
    fatal(xpipe, build_slots[x].stdout_pipe);
    fatal(xmalloc, &build_slots[x].stdout_buf, build_thread_cfg.stdout_buffer_size);

    fatal(xpipe, build_slots[x].stderr_pipe);
    fatal(xmalloc, &build_slots[x].stderr_buf, build_thread_cfg.stderr_buffer_size);

    fatal(xpipe, build_slots[x].auxout_pipe);
    fatal(xmalloc, &build_slots[x].auxout_buf, build_thread_cfg.auxout_buffer_size);

    fatal(exec_builder_xinit, &build_slots[x].exec_builder);
    fatal(exec_render_context_xinit, &build_slots[x].exec_builder_context);

    data = (typeof(data)){ 0 };
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR;

    data.u32 = (x << 16) | 1;
    ev.data = data;
    fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, build_slots[x].stdout_pipe[0], &ev);

    data.u32 = (x << 16) | 2;
    ev.data = data;
    fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, build_slots[x].stderr_pipe[0], &ev);

    data.u32 = (x << 16) | 1001;
    ev.data = data;
    fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, build_slots[x].auxout_pipe[0], &ev);
  }

  fatal(hashtable_createx
    , &build_slots_bypid
    , sizeof(build_slot*)
    , build_thread_cfg.concurrency
    , build_slot_hash
    , build_slot_cmp
    , 0
    , 0
  );

  build_stage = UINT16_MAX;

  finally : coda;
}

xapi build_thread_cleanup()
{
  enter;

  int x;

  fatal(ixclose, &build_devnull_fd);

  for(x = 0; x < build_thread_cfg.concurrency; x++)
  {
    fatal(xclose, build_slots[x].stdout_pipe[0]);
    fatal(xclose, build_slots[x].stdout_pipe[1]);
    fatal(xclose, build_slots[x].stderr_pipe[0]);
    fatal(xclose, build_slots[x].stderr_pipe[1]);
    fatal(xclose, build_slots[x].auxout_pipe[0]);
    fatal(xclose, build_slots[x].auxout_pipe[1]);

    wfree(build_slots[x].argv_stor);
    wfree(build_slots[x].env_stor);
    wfree(build_slots[x].stdout_buf);
    wfree(build_slots[x].stderr_buf);
    wfree(build_slots[x].auxout_buf);
    fatal(exec_builder_xdestroy, &build_slots[x].exec_builder);
    fatal(exec_render_context_xdestroy, &build_slots[x].exec_builder_context);
  }
  iwfree(&build_slots);

  fatal(hashtable_ixfree, &build_slots_bypid);

  fatal(ixclose, &epfd);

  finally : coda;
}

xapi build_thread_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  if(dry)
  {
    build_config(cfg, &staging_cfg);
  }
  else if(cfg->build.changed || cfg->formula.changed)
  {
    if(build_thread_lock)
    {
      build_thread_relaunching = true;
      fatal(sigutil_tgkill, g_params.pid, g_params.thread_build, FABIPC_SIGSCH);

      // wait
      spinlock_engage(&build_thread_lock);
      spinlock_release(&build_thread_lock);

      build_thread_relaunching = false;
    }

    fatal(build_thread_cleanup);

    // apply new config
    memcpy(&build_thread_cfg, &staging_cfg, sizeof(build_thread_cfg));

    // relaunch
    fatal(build_thread_setup);
    fatal(build_thread_launch);
  }

  finally : coda;
}

/* called from server thread */
xapi build_thread_build(bool notify)
{
  enter;

  build_stage = 0;
  build_stage_failure = false;
  stage_index_sum = 0;
  stage_index_sum_target = -1;
  sn_first = llist_first(&bp_selection.list, typeof(*sn_first), lln);
  build_numranks = bp_selection.numranks;

  // go for launch
  notify_completion = notify;
  fatal(sigutil_tgkill, g_params.pid, g_params.thread_build, FABIPC_SIGSCH);

  finally : coda;
}