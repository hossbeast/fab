/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fab/load.h"
#include "lorien/load.h"
#include "moria/load.h"
#include "narrator/load.h"
#include "bacon/load.h"

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
#include "backtracef.h"

#include "common/parseint.h"
#include "common/hash.h"

#include "locks.h"

__thread int32_t tid;

static void xmain()
{
  sigset_t sigs;

  // allow creation of world+rw files
  umask(0);
  sigutil_install_handlers();

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGTERM);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGCHLD);
  sigaddset(&sigs, SIGRTMIN);

  xpthread_sigmask(SIG_SETMASK, &sigs, 0);

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  notify_thread_launch();
  server_thread_launch();
  sweeper_thread_launch();

  // become the monitor thread
  monitor_thread();
}

int main(int argc, char ** argv)
{
  int r;

  r = 0;
  tid = g_params.thread_monitor = gettid();

#ifdef CONFIG_DEVEL
  backtracef_setup();
#endif

  // libraries
  fab_load();
  lorien_load();
  moria_load();
  narrator_load();
  bacon_load();

  args_setup(argc, argv);
  params_setup(g_args.hash);

#if CONFIG_DEVEL
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

    uxkill(pid, SIGTERM);
    sleep(1);
  }
  if(pid && pid != getpid()) {
    fprintf(stderr, "daemon still running pid %ld", (long)pid);
    RTA;
  }

  /* this normally happens in libfab/client right after fork */
  xchdirf("%s/fabd", g_params.ipcdir);
  uxunlinks("exit");
  uxunlinks("core");
#endif

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

  xmain();

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
  lorien_unload();
  moria_unload();
  narrator_unload();
  bacon_unload();

  r |= g_params.handler_error;  // error on a handler thread

  return r;
}
