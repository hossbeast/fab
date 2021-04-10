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

#include "xapi.h"
#include "logger/config.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xpthread.h"

#include "bootstrap_thread.h"
#include "config.h"
#include "global.h"
#include "handler.h"
#include "logging.h"
#include "params.h"

#include "atomics.h"

static xapi bootstrap_thread(handler_context * restrict ctx)
{
  enter;

  bool reconciled;
  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  rcu_register(&rcu_self);
  fatal(global_system_reconcile, ctx, &reconciled);

  /* the initial reconciliation has to be successful */
  if(!reconciled)
  {
    g_params.shutdown = true;
  }

finally:
  trylock_reset(&global_system_reconcile_lock);
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
  rcu_unregister(&rcu_self);
coda;
}

static void * bootstrap_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx;

  tid = g_params.thread_bootstrap = gettid();

  logger_set_thread_name("bootstrap");
  logger_set_thread_categories(L_LOADER);

  ctx = arg;
  fatal(bootstrap_thread, ctx);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("thread", "bootstrap");
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  if(R) {
    g_params.shutdown = true;
    g_params.handler_error = true;
  }

  handler_release(ctx);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi bootstrap_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;
  handler_context *ctx = 0;

  fatal(handler_alloc, &ctx);

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, bootstrap_thread_jump, ctx)) != 0)
  {
    trylock_reset(&global_system_reconcile_lock);
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }
  ctx = 0;

finally:
  pthread_attr_destroy(&attr);
  handler_release(ctx);
coda;
}
