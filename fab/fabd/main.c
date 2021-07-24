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

#include "fab/load.h"
#include "lorien/load.h"
#include "moria/load.h"
#include "narrator/load.h"
#include "value/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "yyutil/load.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "fab/lockfile.h"

#include "build.h"
#include "buildplan.h"
#include "config.h"
#include "config.internal.h"
#include "filesystem.h"
#include "formula.h"
#include "path_cache.h"
#include "rule_system.h"
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

static void xmain_exit;
static void xmain()
{
//  int fds[2];
  sigset_t sigs;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  // allow creation of world+rw files
  umask(0);
  sigutil_install_handlers();

  sigfillset(&sigs);
  xpthread_sigmask(SIG_SETMASK, &sigs, 0);

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  notify_thread_launch();
  server_thread_launch();
  sweeper_thread_launch();

  // become the monitor thread
  monitor_thread();
}

static void xmain_jump()
{
  xmain();

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    void_infos("thread", "fabd");
    void_infof("pid", "%ld", (long)getpid());
    void_infof("tid", "%ld", (long)gettid());
    logger_trace_full(L_ERROR, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    logger_trace_pithy(L_ERROR, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif

    xmain_exit = XAPI_ERRVAL;
    void_calltree_unwind();
  }
coda;
}

static void xmain_load(char ** envp)
{
  tid = g_params.thread_monitor = gettid();

  // libraries
  fab_load();
  logger_load();
  lorien_load();
  moria_load();
  narrator_load();
  value_load();
  valyria_load();
  xlinux_load();
  yyutil_load();

  // initialize logger, main program
  logger_arguments_setup(envp);
  args_setup();
  params_setup(g_args.hash);

#if DEVEL
  /* this normally happens in libfab/client right before fork */
  pid_t pid;
  int lockfd = -1;
  int x;
  for(x = 0; x < 3; x++)
  {
    fabipc_lockfile_obtain(&pid, &lockfd, "%s/fabd/lock", g_params.ipcdir);
    if(!pid || pid == getpid()) {
      break;
    }

    if(!g_args.kill) {
      break;
    }

    uxkill(0, pid, SIGTERM);
    sleep(1);
  }
  if(pid && pid != getpid()) {
    failf(FABD_DAEMONEXCL, "pid", "%ld", (long)pid);
  }

  /* this normally happens in libfab/client right after fork */
  xchdirf("%s/fabd", g_params.ipcdir);
  uxunlinks("exit");
  uxunlinks("core");
#endif

  logging_setup(g_args.hash);

  // modules
  build_setup();
  path_cache_setup();
  formula_setup();
  buildplan_setup();
  filesystem_setup();
  graph_setup();
  rule_system_setup();
  fsent_setup();
  shadow_setup();
  module_setup();
  config_setup();
  notify_thread_setup();
  var_setup();
  variant_setup();
  goals_setup();
  handler_setup();
  walker_setup();

  /* comment */
  fsent_system_bootstrap();
  module_system_bootstrap();
  config_system_bootstrap();

  xmain_jump();

finally:
  // modules
  buildplan_cleanup();
  config_cleanup();
  filesystem_cleanup();
  module_cleanup();
  rule_module_cleanup();
  fsent_cleanup();
  shadow_cleanup();
  notify_thread_cleanup();
  params_cleanup();
  sweeper_thread_cleanup();
  build_cleanup();
  path_cache_cleanup();
  formula_cleanup();
  var_cleanup();
  variant_cleanup();
  goals_cleanup();
  handler_cleanup();
  selection_cleanup();
  dependency_cleanup();
  walker_cleanup();
  request_cleanup();
  graph_cleanup();

  // libraries
  fab_unload();
  logger_unload();
  lorien_unload();
  moria_unload();
  narrator_unload();
  value_unload();
  valyria_unload();
  xlinux_unload();
  yyutil_unload();
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  void R = 0;
  xmain_load(envp);

finally:
  if(XAPI_UNWINDING)
  {
    // failures which cannot be logged with liblogger to stderr
#if DEBUG || DEVEL
    void_backtrace(2, 0);
#else
    void_pithytrace(2, 0);
#endif
  }

conclude(&R);
  void_teardown();

  R |= xmain_exit;              // error on the main thread
  R |= g_params.handler_error;  // error on a handler thread

  return !!R;
}
