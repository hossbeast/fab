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

#include <unistd.h>
#include <stdio.h>

#include "fab/sigutil.h"
#include "xlinux/xpthread.h"
#include "atomics.h"

#include "run_thread.h"
#include "handler_thread.h"
#include "channel.h"
#include "reconcile.h"
#include "goals.h"
#include "handler.h"
#include "params.h"
#include "system_state.h"
#include "buildplan.h"
#include "build.h"
#include "request.internal.h"

bool run_thread_autorun;
static handler_context * owner;
static struct trylock run_lock;
static command_type run_command;

static void run_thread(handler_context * restrict ctx)
{
  sigset_t sigs;
  rcu_thread rcu_self = { };

  rcu_register(&rcu_self);
  run_lock.i32 = tid;

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  if(goals_reconcile || run_command == COMMAND_RECONCILE)
  {
    reconcile();
    if(system_error) {
      goto end;
    }
  }

  if(goals_build && run_command != COMMAND_RECONCILE)
  {
    // potentially re-create the build plan
    goals_create_buildplan();

    if(buildplan_state == READY)
    {
      build(&rcu_self);
    }
  }

  trylock_release(&run_lock);

  if(owner) {
    if(system_error) {
      owner->chan->error = true;
    }

    owner->running = false;
    sigutil_tgkill(g_params.pid, owner->tid, SIGUSR1);
  }

end:
  rcu_unregister(&rcu_self);
}

static void * run_thread_jump(void * arg)
{
  handler_context *ctx = 0;

  ctx = arg;
  g_params.thread_run = ctx->tid = tid = gettid();

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &ctx->stk);
  futexlock_release(&handlers_lock);

  run_thread(ctx);

  futexlock_acquire(&handlers_lock);
  rcu_list_delete(&ctx->stk);
  futexlock_release(&handlers_lock);

  rcu_synchronize();
  handler_release(ctx);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

void run_thread_launch(handler_context * restrict handler, command_type cmd)
{
  pthread_t pthread_id;
  pthread_attr_t attr = {};
  handler_context *ctx = 0;

  if(!trylock_acquire(&run_lock)) {
    fprintf(stderr, "run already in progress %d\n", run_lock.i32);
    return;
  }

  owner = handler;
  run_command = cmd;

  handler_alloc(&ctx);
  if((owner = handler))
  {
    ctx->chan = owner->chan;
  }

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, run_thread_jump, ctx);

  pthread_attr_destroy(&attr);
}
