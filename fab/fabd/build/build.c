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
#include "fab/events.h"
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
#include "xlinux/xunistd.h"
#include "fab/metadata.h"

#include "build.h"
#include "CONFIG.errtab.h"
#include "buildplan.h"
#include "config.internal.h"
#include "build_slot.h"
#include "logging.h"
#include "fsent.h"
#include "params.h"
#include "handler.h"
#include "exec_builder.h"
#include "dependency.h"
#include "system_state.h"

#include "atomics.h"
#include "macros.h"
#include "locks.h"
#include "common/hash.h"
#include "common/attrs.h"
#include "yyutil/box.h"
#include "events.h"
#include "zbuffer.h"
#include "marshal.h"
#include "rcu.h"
#include "times.h"

typedef struct build_thread_config {
  int16_t concurrency;
} build_thread_config;

// internal
int build_devnull_fd = -1;
build_slot * build_slots;
hashtable * build_slots_bypid;
int build_slots_bypid_lock;
bool build_stage_failure;

// local
static build_thread_config staging_cfg;
static build_thread_config active_cfg;
static int epfd = -1;
static uint64_t stage_index_sum;
static int64_t stage_index_sum_target;
static selected *sn_first;
static uint16_t build_stage;
static uint16_t build_numranks;

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

static void configure(const configblob * restrict cfg, build_thread_config * restrict config)
{
  box_int16_setif(cfg->build.concurrency, &config->concurrency);

  if(config->concurrency == 0)
  {
    config->concurrency = g_params.procs;
  }
}

xapi build(rcu_thread * restrict rcu_self)
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;
  int rv;
  uint32_t stage_index;
  build_slot * bs;
  dependency * bpe;
  ptrdiff_t slot_index;
  uint32_t slot;
  uint32_t stream;
  struct epoll_event events[64];
  int x;
  selected *sn;

  build_stage = 0;
  build_stage_failure = false;
  stage_index_sum = 0;
  stage_index_sum_target = -1;
  sn_first = llist_first(&buildplan_selection.list, typeof(*sn_first), lln);
  build_numranks = buildplan_selection.numranks;

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);
  sigdelset(&sigs, SIGCHLD);

  stage_index = 0;
  slot = 0;

  sn = 0;
  while(!g_params.shutdown)
  {
    rcu_quiesce(rcu_self);

    /* build is complete */
    if(build_stage == build_numranks) {
      break;
    }

    /* build has failed, all slots have exited  */
    if(build_stage_failure && build_slots_bypid->size == 0) {
      break;
    }

    // launch up to max concurrency
    while(!g_params.shutdown && build_slots_bypid->size < active_cfg.concurrency)
    {
      rcu_quiesce(rcu_self);

      // stage has failed
      if(build_stage_failure) {
        break;
      }

      // pick up the first stage
      if(sn == 0 && sn_first) {
        sn = sn_first;
        sn_first = 0;
        logf(L_BUILDER, "stage begin %3d", 0);

        system_state_change(BAM_SYSTEM_STATE_BUILDING);
      }

      if(!sn) { // no work
        break;
      }

      if(sn->rank > build_stage) {
        break;
      }

      // locate an open slot
      while(build_slots[slot].pid) {
        slot = (slot + 1) % active_cfg.concurrency;
      }

      bs = &build_slots[slot];
      bpe = sn->bpe;

      /* configure the build slot, mark targets as suppressing notify events */
      fatal(build_slot_prep, bs, bpe, stage_index);

      /* launch the build slot */
      bs->stage = build_stage;
      fatal(build_slot_fork_and_exec, bs);

      fatal(hashtable_put, build_slots_bypid, &bs);

      slot_index = bs - build_slots;
      logf(L_BUILDER, "%6s slot %3"PRIu32" stage %3d index %3d", "launch", (uint32_t)slot_index, build_stage, bs->stage_index);

      // cycle to the next slot
      stage_index++;
      sn = llist_next(&buildplan_selection.list, sn, lln);
      if(!sn || sn->rank != build_stage)
      {
        stage_index_sum_target = sum_first(stage_index);
      }
    }

    // receive epoll events, or a signal
    fatal(uxepoll_pwait, &rv, epfd, events, sizeof(events) / sizeof(*events), SEC_AS_MSEC(1), &sigs);

    /* read slots */
    for(x = 0; x < rv; x++)
    {
      rcu_quiesce(rcu_self);

      slot = events[x].data.u32 >> 16;
      stream = events[x].data.u32 & 0xFFFF;

      bs = &build_slots[slot];
      fatal(build_slot_read, bs, stream);
    }

    /* reap children */
    while(!g_params.shutdown)
    {
      rcu_quiesce(rcu_self);

      fatal(uxwaitid, P_ALL, 0, &siginfo, WEXITED | WNOHANG);
      if(siginfo.si_pid == 0) {
        break;
      }

      build_slot * key = (build_slot[]) {{ pid : siginfo.si_pid }};
      build_slot ** bsp = hashtable_get(build_slots_bypid, &key);
      RUNTIME_ASSERT(bsp);

      bs = *bsp;
      slot_index = bs - build_slots;

      fatal(build_slot_reap, bs, &siginfo);
      logf(L_BUILDER, "%6s slot %3"PRIu32" stage %3d index %3d status %3d stderr-len %3d", "reap", (uint32_t)slot_index, build_stage, bs->stage_index, bs->status, bs->stderr_total);

      if(bs->status || bs->stderr_total) {
        build_stage_failure = true;
      }

      // free up the slot
      fatal(hashtable_delete, build_slots_bypid, &key);

      bs->pid = 0;
      bs->stdout_total = 0;
      bs->stderr_total = 0;
      bs->auxout_total = 0;

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
    }
  }

  system_state_change(BAM_SYSTEM_STATE_OK);

  finally : coda;
}

//
// public
//

xapi build_setup()
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
  fatal(xmalloc, &build_slots, sizeof(*build_slots) * active_cfg.concurrency);
  for(x = 0; x < active_cfg.concurrency; x++)
  {
    fatal(xpipe, build_slots[x].stdout_pipe);
    fatal(xpipe, build_slots[x].stderr_pipe);
    fatal(xpipe, build_slots[x].auxout_pipe);

    fatal(exec_builder_xinit, &build_slots[x].exec_builder);
    fatal(exec_render_context_xinit, &build_slots[x].exec_builder_context);

    data = (typeof(data)){ 0 };
    ev.events = EPOLLIN;

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
    , active_cfg.concurrency
    , build_slot_hash
    , build_slot_cmp
    , 0
    , 0
  );

  finally : coda;
}

xapi build_cleanup()
{
  enter;

  int x;

  fatal(ixclose, &build_devnull_fd);

  for(x = 0; x < active_cfg.concurrency; x++)
  {
    fatal(xclose, build_slots[x].stdout_pipe[0]);
    fatal(xclose, build_slots[x].stdout_pipe[1]);
    fatal(xclose, build_slots[x].stderr_pipe[0]);
    fatal(xclose, build_slots[x].stderr_pipe[1]);
    fatal(xclose, build_slots[x].auxout_pipe[0]);
    fatal(xclose, build_slots[x].auxout_pipe[1]);

    wfree(build_slots[x].argv_stor);
    wfree(build_slots[x].env_stor);
    fatal(exec_builder_xdestroy, &build_slots[x].exec_builder);
    fatal(exec_render_context_xdestroy, &build_slots[x].exec_builder_context);
  }
  iwfree(&build_slots);

  fatal(hashtable_ixfree, &build_slots_bypid);

  fatal(ixclose, &epfd);

  finally : coda;
}

xapi build_reconfigure(configblob * restrict cfg, bool dry)
{
  enter;

  if(dry)
  {
    configure(cfg, &staging_cfg);
  }
  else if(cfg->build.changed)
  {
    fatal(build_cleanup);

    // apply new config
    memcpy(&active_cfg, &staging_cfg, sizeof(active_cfg));

    // relaunch
    fatal(build_setup);
  }

  finally : coda;
}
