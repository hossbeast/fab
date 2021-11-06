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
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "fab/lockfile.h"
#include "logger.h"
#include "logger/arguments.h"

#include "FABD.errtab.h"
#include "build.h"
#include "buildplan.h"
#include "config.h"
#include "config.internal.h"
#include "filesystem.h"
#include "formula.h"
#include "path_cache.h"
#include "rule_system.h"
#include "logging.h"
#include "module.h"
#include "rule_module.h"
#include "monitor_thread.h"
#include "fsent.h"
#include "shadow.h"
#include "notify_thread.h"
#include "params.h"
#include "args.h"
#include "request.internal.h"
#include "server_thread.h"
#include "dependency.h"
#include "sweeper_thread.h"
#include "handler_thread.h"
#include "reconcile.h"
#include "channel.h"
#include "handler.h"
#include "var.h"
#include "variant.h"
#include "goals.h"
#include "stats.h"
#include "selection.h"
#include "walker.h"

#include "common/parseint.h"
#include "common/hash.h"

#include "locks.h"

static xapi xmain_exit;
static xapi xmain()
{
  enter;

//  int fds[2];
  sigset_t sigs;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  // allow creation of world+rw files
  umask(0);
  fatal(sigutil_install_handlers);

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGTERM);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGCHLD);
  sigaddset(&sigs, SIGRTMIN);

  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  fatal(notify_thread_launch);
  fatal(server_thread_launch);
  fatal(sweeper_thread_launch);

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
#if DEBUG || DEVEL
    xapi_infos("thread", "fabd");
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

  tid = g_params.thread_monitor = gettid();

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
  fatal(args_setup);
  fatal(params_setup, g_args.hash);

#if DEVEL
  /* this normally happens in libfab/client right before fork */
  pid_t pid;
  int lockfd = -1;
  int x;
  for(x = 0; x < 3; x++)
  {
    fatal(fabipc_lockfile_obtain, &pid, &lockfd, "%s/fabd/lock", g_params.ipcdir);
    if(!pid || pid == getpid()) {
      break;
    }

    if(!g_args.kill) {
      break;
    }

    fatal(uxkill, 0, pid, SIGTERM);
    sleep(1);
  }
  if(pid && pid != getpid()) {
    failf(FABD_DAEMONEXCL, "pid", "%ld", (long)pid);
  }

  /* this normally happens in libfab/client right after fork */
  fatal(xchdirf, "%s/fabd", g_params.ipcdir);
  fatal(uxunlinks, "exit");
  fatal(uxunlinks, "core");
#endif

  fatal(logging_setup, g_args.hash);

  // modules
  fatal(build_setup);
  fatal(path_cache_setup);
  fatal(formula_setup);
  fatal(buildplan_setup);
  fatal(filesystem_setup);
  fatal(graph_setup);
  fatal(rule_system_setup);
  fatal(fsent_setup);
  fatal(shadow_setup);
  fatal(module_setup);
  fatal(config_setup);
  fatal(notify_thread_setup);
  fatal(var_setup);
  fatal(variant_setup);
  fatal(goals_setup);
  fatal(handler_setup);
  fatal(walker_setup);

  /* comment */
  fatal(fsent_system_bootstrap);
  fatal(module_system_bootstrap);
  fatal(config_system_bootstrap);

  fatal(xmain_jump);

finally:
  // modules
  fatal(buildplan_cleanup);
  fatal(config_cleanup);
  fatal(filesystem_cleanup);
  fatal(module_cleanup);
  fatal(rule_module_cleanup);
  fatal(fsent_cleanup);
  fatal(shadow_cleanup);
  fatal(notify_thread_cleanup);
  fatal(params_cleanup);
  fatal(sweeper_thread_cleanup);
  fatal(build_cleanup);
  fatal(path_cache_cleanup);
  fatal(formula_cleanup);
  fatal(var_cleanup);
  fatal(variant_cleanup);
  fatal(goals_cleanup);
  fatal(handler_cleanup);
  fatal(selection_cleanup);
  fatal(dependency_cleanup);
  fatal(walker_cleanup);
  fatal(request_cleanup);
  fatal(graph_cleanup);

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
#if DEBUG || DEVEL
    xapi_backtrace(2, 0);
#else
    xapi_pithytrace(2, 0);
#endif
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;              // error on the main thread
  R |= g_params.handler_error;  // error on a handler thread

  return !!R;
}
