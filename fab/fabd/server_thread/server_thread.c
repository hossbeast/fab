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

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"
#include "valyria/set.h"

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/stream.h"
#include "logger/config.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "moria/graph.h"

#include "logger/arguments.h"

#include "server_thread.internal.h"
#include "handler_thread.h"
#include "config.h"
#include "errtab/FABD.errtab.h"
#include "logging.h"
#include "module.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "config.internal.h"
#include "usage.h"
#include "stats.h"
#include "walker.internal.h"
#include "buildplan.h"
#include "channel.h"
#include "goals.h"
#include "build_thread.internal.h"
#include "rule.h"
#include "match.h"
#include "path.h"
#include "pattern.h"
#include "node_operations.h"
#include "formula.h"

#include "common/attrs.h"
#include "macros.h"
#include "atomics.h"

bool g_server_autorun;

#if DEVEL
bool g_server_no_initial_client;      // no client right at startup
fabipc_channel *g_server_initial_channel;
#endif

/**
 * run invalidated rules to quiesence
 */
static xapi rules_full_refresh(rule_run_context * restrict rule_ctx)
{
  enter;

  rule_module_association *rma;
  narrator * N;

  /* re-run invalidated rules */
  fatal(rule_run_context_begin, rule_ctx);
  fatal(graph_full_refresh, rule_ctx);

  /* warn about rules that have no effect */
  if(log_would(L_WARN))
  {
    rbtree_foreach(&rule_ctx->nohits, rma, nohits_rbn) {
      fatal(log_start, L_WARN, &N);
      xsays("0 matches : ");
      fatal(rule_say, rma->rule, N);
      xsays(" @ module ");
      fatal(node_absolute_path_say, rma->mod->dir_node, N);
      fatal(log_finish);
    }
  }

finally:
  rule_run_context_end(rule_ctx);
coda;
}

static xapi server_thread()
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;
  int iteration;
  rule_run_context rule_ctx = { 0 };
  int walk_id;
  graph_invalidation_context invalidation = { 0 };

  g_params.thread_server = gettid();

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGRTMIN);

  fatal(rule_run_context_xinit, &rule_ctx);
  rule_ctx.modules = &g_modules;

  // extern_reconfigure loads areas of the filesystem referenced via extern
  fatal(config_begin_staging);
  fatal(config_reconfigure);
  if(!config_reconfigure_result)
  {
    fprintf(stderr, "config was not applied\n");
    exit(1);
  }

  // load the filesystem rooted at the project dir
  walk_id = walker_descend_begin();
  fatal(graph_invalidation_begin, &invalidation);
  fatal(walker_descend, 0, g_project_root, 0, g_params.proj_dir, walk_id, &invalidation);
  fatal(walker_ascend, g_project_root, walk_id, &invalidation);
  graph_invalidation_end(&invalidation);

  // load the module in this directory (and nested modules, recursively)
  fatal(graph_invalidation_begin, &invalidation);
  fatal(module_load_project, g_project_root, g_params.proj_dir, &invalidation);
  graph_invalidation_end(&invalidation);

  if(g_project_root->mod == 0) {
    fprintf(stderr, "module.bam not found\n");
    exit(1);
  }

  if(g_project_root->mod->self_node->not_parsed) {
    fprintf(stderr, "module.bam was not parsed\n");
    exit(1);
  }

  fatal(rules_full_refresh, &rule_ctx);
  fatal(formula_full_refresh);

  iteration = 0;
#if DEVEL
  if(g_server_no_initial_client)
  {
    fatal(handler_thread_launch, 0, 0, false);
    iteration = 1;
  }
#endif

  for(; !g_params.shutdown; iteration++)
  {
    fatal(sigutil_wait, &sigs, &siginfo);
    if(g_params.shutdown) {
      break;
    }

    /* signal from monitor thread */
    if(siginfo.si_signo == SIGUSR1)
    {
      continue;
    }

#if DEVEL
    g_logging_skip_reconfigure = false;
#endif

    /* assumes the very first request always comes immediately after starting up */
    if(iteration)
    {
      fatal(node_full_refresh);
      fatal(graph_invalidation_begin, &invalidation);
      fatal(module_full_refresh, &invalidation);
      graph_invalidation_end(&invalidation);
      fatal(rules_full_refresh, &rule_ctx);
      fatal(formula_full_refresh);
    }

#if 0
    /* signal from sweeper thread */
    if(siginfo.si_pid == g_params.pid)
    {
      RUNTIME_ASSERT(g_server_autorun);
      fatal(handler_thread_launch, 0, 0, true);
    }
    else
    {
    }
#endif
    fatal(handler_thread_launch, siginfo.si_pid, (intptr_t)siginfo.si_value.sival_ptr, false);

    fatal(usage_report);
    fatal(stats_report);
  }

finally:
  // locals
  graph_invalidation_end(&invalidation);
  fatal(rule_run_context_xdestroy, &rule_ctx);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * server_thread_main(void * arg)
{
  enter;

  xapi R;

  logger_set_thread_name("server");
  logger_set_thread_categories(L_SERVER);
  fatal(server_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/server");
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
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi server_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, server_thread_main, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
