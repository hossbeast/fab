/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "bam/sigutil.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "atomics.h"

#include "run_thread.h"
#include "channel_thread.h"
#include "channel.h"
#include "reconcile.h"
#include "goals.h"
#include "request_handler.h"
#include "params.h"
#include "system.h"
#include "buildplan.h"
#include "build.h"
#include "request.h"
#include "events/events.h"
#include "timeout_thread.h"

#include "threads.h"
#include "zcat.h"

bool run_thread_autorun;
static struct trylock run_lock;
static command_type run_command;
static pid_t run_pid;
static uint64_t run_token;
static bool run_reconcile;
static bool run_build;
static char run_label[128];
static uint16_t run_label_len;

static void run_thread(channel_thread * restrict owner)
{
  run_lock.i32 = tid;

  if(run_reconcile || run_command == COMMAND_RECONCILE)
  {
    reconcile();
    if(system_has_error) {
      goto end;
    }
  }

  if(run_build && run_command != COMMAND_RECONCILE)
  {
    // potentially re-create the build plan
    goals_create_buildplan();
    if(system_has_error) {
      goto end;
    }

    build(run_command == COMMAND_AUTORUN, run_pid, run_token, run_label, run_label_len);
    if(system_has_error) {
      goto end;
    }
  }

end:
  trylock_release(&run_lock);

  if(owner) {
    /* wake the handler thread which created this run thread */
    owner->run_thread_pending = false;
    sigutil_uxtgkill(g_params.pid, owner->tid, SIGUSR1);
  }
}

static void *run_thread_jump(void *arg)
{
  channel_thread *owner = 0;

  owner = arg;
  g_params.thread_run = tid = gettid();
  errnop = &errno;

  run_thread(owner);

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

bool run_thread_launch(struct request_handler * restrict handler, channel_thread * restrict thread, command_type cmd)
{
  pthread_t pthread_id;
  pthread_attr_t attr = {};

  if(!trylock_acquire(&run_lock)) {
    return false;
  }

  /* reset the timeout thread */
  if(cmd == COMMAND_AUTORUN) {
    timeout_thread_refresh_on_autobuild();
  }

  run_command = cmd;
  run_reconcile = goals_reconcile;
  run_build = goals_build;
  run_pid = goals_pid;
  run_token = goals_token;
  run_label_len = zcatw(run_label, sizeof(run_label), goals_label, goals_label_len);

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, run_thread_jump, thread);

  pthread_attr_destroy(&attr);

  return true;
}

void autorun_set_event(request_handler * restrict handler)
{
  bamipc_message *msg;
  events_context ectx;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_AUTORUN_SET))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("autorun-set")
    { SET
      { MAPPING
        STRING("autorun")
        BOOL(run_thread_autorun)
      }
    }
  }

  msg->len = fixed.l;
  events_publish(&ectx);
}

void run_thread_autorun_set(bool autorun)
{
  if(run_thread_autorun == autorun) {
    return;
  }

  run_thread_autorun = autorun;
  autorun_set_event(0);
}
