/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "bam/load.h"
#include "lorien/load.h"
#include "moria/load.h"
#include "narrator/load.h"
#include "bacon/load.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "bam/ipc.h"
#include "bam/sigutil.h"
#include "bam/lockfile.h"

#include "build.h"
#include "buildplan.h"
#include "config.h"
#include "configuration.h"
#include "config_system.h"
#include "filesystem.h"
#include "formula.h"
#include "path_cache.h"
#include "rule_system.h"
#include "module.h"
#include "block.h"
#include "rule_module_association.h"
#include "module_edge.h"
#include "monitor_thread.h"
#include "timeout_thread.h"
#include "fsent.h"
#include "shadow.h"
#include "notify_thread.h"
#include "params.h"
#include "args.h"
#include "request.h"
#include "server_thread.h"
#include "dependency.h"
#include "sweeper_thread.h"
#include "handler_thread.h"
#include "reconcile.h"
#include "channel.h"
#include "handler.h"
#include "var.h"
#include "goals.h"
#include "stats.h"
#include "selection.h"
#include "walker.h"
#include "rules.h"
#include "special_edge.h"
#include "index_edge.h"
#include "meta_edge.h"
#include "rule_module_association.h"
#include "buildplan_entity.h"
#include "entlist.h"
#include "fsedge.h"
#include "backtracef.h"
#include "rcu.h"
#include "index_vertex.h"

#include "common/parseint.h"
#include "common/hash.h"

#include "locks.h"
#include "zcat.h"

__thread int32_t tid;
__thread int *errnop;

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

  notify_thread_launch();
  server_thread_launch();
  sweeper_thread_launch();
  timeout_thread_launch();

  // become the monitor thread
  monitor_thread();
}

/* normally, libbam/client obtains this lockfile before fork, and this process
 * has inherited the lock */
static void getlock(void)
{
  pid_t pid;
  int lockfd = -1;
  char path[PATH_MAX];

  snprintf(path, sizeof(path), "%s/bamd/lock", g_params.ipcdir);
  bamipc_lockfile_acquire(&pid, &lockfd, path);
  if(!pid || pid == getpid()) {
    xclose(lockfd);
    /* fastpath lock-held check */
    return;
  }

  bamipc_lockfile_obtain(&pid, &lockfd, "%s", path);
  if(pid && pid != getpid()) {
    dprintf(2, "daemon still running pid %ld\n", (long)pid);
    exit(1);
  }

  /* this normally happens in libbam/client right after fork */
  xchdirf("%s/bamd", g_params.ipcdir);
  uxunlinks("exit");
  uxunlinks("core");
}

int main(int argc, char ** argv)
{
  int r;
  int fd;
  char buf[32];

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  r = 0;
  tid = g_params.thread_monitor = gettid();
  errnop = &errno;

#if CONFIG_DEVEL
  backtracef_setup();
#endif

  // libraries
  bam_load();
  lorien_load();
  moria_load();
  narrator_load();
  bacon_load();

  args_setup(argc, argv);
  params_setup();
  xrealpathf(g_base_path, "%.*s/basedir", (int)g_params.ipcdir_len, g_params.ipcdir);
  g_base_path_len = strlen(g_base_path);

  dprintf(1, "hash : %s\n", g_args.hash_str);
  dprintf(1, "pid : %d\n", g_params.pid);
  dprintf(1, "homedir : %.*s\n", (int)g_params.homedir_len, g_params.homedir);
  dprintf(1, "ipcdir : %.*s\n", (int)g_params.ipcdir_len, g_params.ipcdir);
  dprintf(1, "basedir : %.*s\n", (int)g_base_path_len, g_base_path);

  /* for convenience, display the path in argv */
  if(g_argv_base_dir_space) {
    memset(g_argv_base_dir_space, 0, 100);
    zcatf(g_argv_base_dir_space, 100, "[%.*s]", g_base_path_len, g_base_path);
  }

  getlock();

#if CONFIG_DEBUG || CONFIG_DEVEL
  rcu_is_registered = true;
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
  configuration_setup();
  notify_thread_setup();
  var_setup();
  goals_setup();
  handler_setup();
  walker_setup();
  rules_setup();
  buildplan_entity_setup();
  entlist_setup();
  config_system_setup();
  index_vertex_setup();

  /* comment */
  fsent_system_bootstrap();
  config_system_bootstrap();

  xmain();

  // modules
  buildplan_cleanup();
  config_cleanup();
  config_system_cleanup();
  configuration_cleanup();
  filesystem_cleanup();
  module_cleanup();
  fsent_cleanup();
  shadow_cleanup();
  notify_thread_cleanup();
  sweeper_thread_cleanup();
  build_cleanup();
  path_cache_cleanup();
  formula_cleanup();
  var_cleanup();
  goals_cleanup();
  handler_cleanup();
  selection_cleanup();
  dependency_cleanup();
  walker_cleanup();
  request_cleanup();
  graph_cleanup();
  block_cleanup();
  module_module_cleanup();
  rules_cleanup();
  special_edge_cleanup();
  index_edge_cleanup();
  meta_edge_cleanup();
  rule_module_association_cleanup();
  entlist_cleanup();
  index_vertex_cleanup();

  // libraries
  bam_unload();
  lorien_unload();
  moria_unload();
  narrator_unload();
  bacon_unload();

  r |= g_params.handler_error;  // error on a handler thread

  /* best-effort attempt to save our exit status */
  fd = uxopen_modef(O_CREAT | O_WRONLY, 0666, "%s/bamd/exit", g_params.ipcdir);
  if(fd != -1) {
    snprintf(buf, sizeof(buf), "%d\n", r);
    (void)!write(fd, buf, strlen(buf));
    close(fd);
  }

  return r;
}
