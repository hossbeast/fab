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

#include "xapi.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "fab/sigutil.h"
#include "logger/config.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xpthread.h"
#include "atomics.h"

#include "run_thread.h"
#include "handler_thread.h"
#include "channel.h"
#include "reconcile_thread.h"
#include "goals.h"
#include "handler.h"
#include "logging.h"
#include "params.h"

static handler_thread * owner;

static xapi run_thread(handler_context * restrict ctx)
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;
  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  rcu_register(&rcu_self);

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  if(!trylock_acquire(&reconcile_lock)) {
    fprintf(stderr, "reconcile already in progress %d\n", reconcile_lock.i32);
    goto XAPI_FINALIZE;
  }

  if(!trylock_acquire(&handler_build_lock)) {
    fprintf(stderr, "build already in progress by %d\n", handler_build_lock.i32);
    goto XAPI_FINALIZE;
  }

  /* global reconciliation */
  ctx->state = HANDLER_RECONCILE_IN_PROGRESS;
  fatal(reconcile_thread_launch, ctx);
  while(!g_params.shutdown && ctx->state == HANDLER_RECONCILE_IN_PROGRESS) {
    rcu_quiesce(&rcu_self);
    fatal(sigutil_wait, &sigs, &siginfo);
  }
  if(ctx->chan->error) {
    goto XAPI_FINALLY;
  }

  /* prosecute the same goals */
  ctx->state = HANDLER_BUILD_IN_PROGRESS;
  fatal(goals_kickoff, ctx);

  /* wait for the build to complete */
  while(!g_params.shutdown && ctx->state == HANDLER_BUILD_IN_PROGRESS) {
    rcu_quiesce(&rcu_self);
    fatal(sigutil_wait, &sigs, &siginfo);
  }

finally:
  trylock_release(&handler_build_lock);
  trylock_release(&reconcile_lock);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  rcu_unregister(&rcu_self);
coda;
}

static void * run_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx = 0;
  channel *chan = 0;

  ctx = arg;
  g_params.thread_run = ctx->tid = tid = gettid();
  logger_set_thread_name("run");
  logger_set_thread_categories(L_HANDLER);
  tracef("run thread start");

  fatal(channel_create, &chan, tid);
  ctx->chan = chan;

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &ctx->stk);
  futexlock_release(&handlers_lock);

  fatal(run_thread, ctx);

finally:
  tracef("run thread done");

  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "run");
    xapi_infof("pid", "%ld", (long)g_params.pid);
    xapi_infof("tid", "%"PRId32, tid);
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  futexlock_acquire(&handlers_lock);
  rcu_list_delete(&ctx->stk);
  futexlock_release(&handlers_lock);

  if(R) {
    g_params.shutdown = true;
    g_params.handler_error = true;
  }

  rcu_synchronize();
  channel_release(chan);
  handler_release(ctx);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi run_thread_launch(handler_context * restrict handler)
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;
  handler_context *ctx = 0;

  owner = handler;

  fatal(handler_alloc, &ctx);
  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, run_thread_jump, ctx)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
