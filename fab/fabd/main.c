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

#include <string.h>
#include <stdlib.h>

#include "xapi.h"
#include "fab/load.h"
#include "logger/load.h"
#include "lorien/load.h"
#include "moria/load.h"
#include "narrator/load.h"
#include "value/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "yyutil/load.h"

#include "xapi/SYS.errtab.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xapi/errtab.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger.h"
#include "logger/arguments.h"

#include "FABD.errtab.h"
#include "build_thread.h"
#include "buildplan.h"
#include "config.h"
#include "config.internal.h"
#include "extern.h"
#include "filesystem.h"
#include "formula.h"
#include "path_cache.h"
#include "logging.h"
#include "module.h"
#include "monitor_thread.h"
#include "node.h"
#include "shadow.h"
#include "notify_thread.h"
#include "params.h"
#include "request.internal.h"
#include "server_thread.h"
#include "sweeper_thread.h"
#include "var.h"
#include "variant.h"
#include "goals.h"
#include "stats.h"
#include "selection.h"
#include "common/parseint.h"

static xapi xmain_exit;
static xapi xmain()
{
  enter;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  fatal(params_report);

  // allow creation of world+rw files
  umask(0);

  // close standard file descriptors
  fatal(xclose, 0);
  fatal(xopens, 0, O_RDONLY, "/dev/null");

#if DEVEL
  if(g_argc > 1 && strcmp(g_argv[1], "--request") == 0)
  {
    g_logging_skip_reconfigure = true;
    g_server_no_initial_client = true;
    RUNTIME_ASSERT(g_argc > 2);
    g_server_initial_request = g_argv[2];

    pid_t pgid;
    fatal(ipc_lock_obtain, &pgid, 0, "%s/fabd/lock", g_params.ipcdir);

    if(pgid)
      failf(FABD_DAEMONEXCL, "pgid", "%ld", (long)pgid);
  }
#endif

  g_params.thread_monitor = gettid();

  // launch other threads
  fatal(sigutil_defaults);
  fatal(notify_thread_launch);
  fatal(server_thread_launch);
  fatal(sweeper_thread_launch);

  /* the build thread is launched after the initial reconfiguration */

  // become the monitor thread
  fatal(monitor_thread);

  finally : coda;
}

static xapi xmain_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_trace_pithy, L_ERROR, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif

    xmain_exit = XAPI_ERRVAL;
    xapi_calltree_unwind();
  }
coda;
}

static xapi xmain_load(char ** envp)
{
  enter;

  uint64_t hash = 0;

  // libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(lorien_load);
  fatal(moria_load);
  fatal(narrator_load);
  fatal(value_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(yyutil_load);

  // initialize logger, main program
  fatal(logger_arguments_setup, envp);

#if !DEVEL
  if(g_argc < 2 || parseuint(g_argv[1], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &hash, 0) != 0)
    fail(SYS_BADARGS);
#endif

  // core file goes in cwd
  fatal(params_setup, &hash);
  fatal(xchdirf, "%s/fabd", g_params.ipcdir);

  // logging with per-ipc logfiles
  fatal(logging_setup, hash);

  // modules
  fatal(build_thread_setup);
  fatal(path_cache_setup);
  fatal(formula_setup);
  fatal(buildplan_setup);
  fatal(config_setup);
  fatal(extern_setup);
  fatal(filesystem_setup);
  fatal(graph_setup);
  fatal(module_setup);
  fatal(node_setup);
  fatal(shadow_setup);
  fatal(notify_thread_setup);
  fatal(request_setup);
  fatal(sweeper_thread_setup);
  fatal(var_setup);
  fatal(variant_setup);
  fatal(goals_setup);
  fatal(stats_setup);
  fatal(selection_setup);

  fatal(xmain_jump);

finally:
  // modules
  fatal(buildplan_cleanup);
  fatal(config_cleanup);
  fatal(extern_cleanup);
  fatal(filesystem_cleanup);
  fatal(module_cleanup);
  fatal(node_cleanup);
  fatal(shadow_cleanup);
  fatal(graph_cleanup);
  fatal(notify_thread_cleanup);
  fatal(params_cleanup);
  fatal(sweeper_thread_cleanup);
  fatal(build_thread_cleanup);
  fatal(path_cache_cleanup);
  fatal(formula_cleanup);
  fatal(request_cleanup);
  fatal(var_cleanup);
  fatal(variant_cleanup);
  fatal(goals_cleanup);
  fatal(stats_cleanup);
  fatal(selection_cleanup);

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(lorien_unload);
  fatal(moria_unload);
  fatal(narrator_unload);
  fatal(value_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
  fatal(yyutil_unload);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  xapi R = 0;
  fatal(xmain_load, envp);

finally:
  if(XAPI_UNWINDING)
  {
    // failures which cannot be logged with liblogger to stderr
    xapi_backtrace();
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;

  return !!R;
}

