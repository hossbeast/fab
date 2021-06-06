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

#include "fab/sigutil.h"
#include "logger/config.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xpthread.h"

#include "reconcile_thread.h"
#include "channel.h"
#include "config.h"
#include "extern.h"
#include "formula.h"
#include "fsent.h"
#include "handler.h"
#include "logging.h"
#include "module.h"
#include "params.h"
#include "rule_system.h"
#include "stats.h"
#include "var.h"
#include "walker.h"

#include "atomics.h"

//struct trylock reconcile_lock;
bool system_error;
uint16_t reconciliation_id;

xapi reconcile(channel * restrict chan)
{
  enter;

  graph_invalidation_context invalidation = { };
  rule_run_context rule_ctx;
  uint16_t walk_id;
  bool work;
  int x;

  fatal(rule_run_context_xinit, &rule_ctx);

  reconciliation_id++;
  walk_id = walker_begin();
  fatal(graph_invalidation_begin, &invalidation);

  work = true;
  for(x = 0; work; x++)
  {
    if((x % 2) == 0)
    {
      /* reload all config files (this can change filesystem invalidation settings)
       * it can also change walker include/exclude settings */
      system_state_change(BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE);
      fatal(config_system_reconcile, &work, &invalidation);
tracef("x %2d config-system work %d", x, work);
    }
    else
    {
      /* walk filesystem trees (this can find config files in the ancestry of the project root)
       * it can also invalidate config files */
      system_state_change(BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE);
      fatal(walker_system_reconcile, walk_id, &work, &invalidation);
tracef("x %2d walker-system work %d", x, work);
    }
    if(system_error) {
      goto XAPI_FINALLY;
    }
  }

  /* reload modules */
  system_state_change(BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE);
  fatal(module_system_reconcile);
  if(system_error) {
    goto XAPI_FINALLY;
  }

  /* cleanup var files not reloaded by any module */
  system_state_change(BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE);
  fatal(var_system_reconcile);
  if(system_error) {
    goto XAPI_FINALLY;
  }

  /* run rules to quiescence */
  system_state_change(BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE);
  fatal(rule_system_reconcile, &rule_ctx);
  if(system_error) {
    goto XAPI_FINALLY;
  }

  /* cleanup formulas which are no longer referenced */
  system_state_change(BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE);
  fatal(formula_system_reconcile);
  if(system_error) {
    goto XAPI_FINALLY;
  }

  system_state_change(BAM_SYSTEM_STATE_OK);
  STATS_INC(g_stats.system_state_ok);

finally:
  graph_invalidation_end(&invalidation);
  fatal(rule_run_context_xdestroy, &rule_ctx);
coda;
}

#if 0
static xapi reconcile_thread(handler_context * restrict handler)
{
  enter;

  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  rcu_register(&rcu_self);
  fatal(reconcile, handler->chan);

  /* unblock owner */
  handler->state = HANDLER_RECONCILE_DONE;
  fatal(sigutil_tgkill, g_params.pid, handler->tid, SIGUSR1);

finally:

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
  rcu_unregister(&rcu_self);
coda;
}

static void * reconcile_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *handler;

  handler = arg;
  tid = g_params.thread_reconcile = gettid();
  logger_set_thread_name("reconcile");
  logger_set_thread_categories(L_LOADER);

  fatal(reconcile_thread, handler);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("thread", "reconcile");
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

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi reconcile_thread_launch(handler_context * restrict ctx)
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

tracef();

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, reconcile_thread_jump, ctx)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
#endif
