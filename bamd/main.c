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
#include "configset.h"
#include "config.h"
#include "config_system.h"
#include "filesystem.h"
#include "formula.h"
#include "rule_system.h"
#include "module.h"
#include "block.h"
#include "rma.h"
#include "module_edge.h"
#include "monitor_thread.h"
#include "timeout_thread.h"
#include "fsent.h"
#include "shadow.h"
#include "notify_thread.h"
#include "params.h"
#include "args.h"
#include "request.h"
#include "channel_server_thread.h"
#include "dependency.h"
#include "sweeper_thread.h"
#include "handler_thread.h"
#include "reconcile.h"
#include "channel.h"
#include "request_handler.h"
#include "varset.h"
#include "goals.h"
#include "stats.h"
#include "selection.h"
#include "ruleset.h"
#include "special_edge.h"
#include "ns_edge.h"
#include "dir_rma_edge.h"
#include "meta_edge.h"
#include "rma.h"
#include "buildplan_entity.h"
#include "fsedge.h"
#include "rcu.h"
#include "fsname.h"
#include "plugin.h"
#include "events/events.h"
#include "fstree.h"
#include "namespace.h"
#include "filesystem.h"
#include "system.h"
#include "notify.h"

#include "common/parseint.h"
#include "common/hash.h"

#include "locks.h"
#include "zcat.h"

#if CONFIG_DEVEL
#include "backtracef.h"
#endif

__thread int32_t tid;
__thread int *errnop;

static void xmain()
{
  sigset_t sigs;

  // allow creation of world+rw files
  umask(0);
  sigutil_install_handlers();
  timers_setup();

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGTERM);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGUSR2);    /* see timers */
  sigaddset(&sigs, SIGCHLD);
  sigaddset(&sigs, SIGRTMIN);

  xpthread_sigmask(SIG_SETMASK, &sigs, 0);

  notify_thread_launch();
  channel_server_thread_launch();
  sweeper_thread_launch();
  timeout_thread_launch();

  // become the monitor thread
  monitor_thread();
}

/* normally, libbam/client obtains this lockfile before fork, and this process
 * has inherited the lock via fd 250 */
static void getlock(void)
{
  int64_t pid;
  int lockfd = -1;
  char path[PATH_MAX];
  int status;

  if((status = fcntl(250, F_GETFL)) == -1) {
    RUNTIME_ASSERT(errno == EBADF);
  } else {
    /* assuming here that the client opened fd 250 correctly */
    return;
  }

  snprintf(path, sizeof(path), "%s/bamd/lock", g_params.ipcdir);
  bamipc_lockfile_acquire(&pid, &lockfd, path);
  bamipc_lockfile_validate(&pid, &lockfd, path);
  if(lockfd == -1) {
    dprintf(2, "unable to obtain lockfile %s, recorded pid %ld\n", path, (long)pid);
    exit(1);
  }

  /* move to known location, write the pid */
  xdup2(lockfd, 250);
  xclose(lockfd);
  bamipc_lockfile_update(250);
}

int main(int argc, char ** argv)
{
  int r;
  int fd;
  char buf[32];

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

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
  xrealpathf(g_base_dir, "%.*s/basedir", (int)g_params.ipcdir_len, g_params.ipcdir);
  g_base_dir_len = strlen(g_base_dir);
  g_base_dirfd = xopens(O_PATH | O_DIRECTORY, g_base_dir);

  dprintf(1, "hash : %s\n", g_args.hash_str);
  dprintf(1, "pid : %d\n", g_params.pid);
  dprintf(1, "homedir : %.*s\n", (int)g_params.homedir_len, g_params.homedir);
  dprintf(1, "ipcdir : %.*s\n", (int)g_params.ipcdir_len, g_params.ipcdir);
  dprintf(1, "basedir : %.*s\n", (int)g_base_dir_len, g_base_dir);

  /* for convenience, display basedir in argv */
  if(g_argv_base_dir_space) {
    memset(g_argv_base_dir_space, 0, 100);
    zcatf(g_argv_base_dir_space, 100, "[%.*s]", g_base_dir_len, g_base_dir);
  }

  getlock();

  /* this normally happens in libbam/client right after fork */
  xchdirf("%s/bamd", g_params.ipcdir);
  uxunlinks("exit");
  uxunlinks("core");

  // modules
  rcu_setup();
  build_setup();
  formula_setup();
  buildplan_setup();
  graph_setup();
  rule_system_setup();
  fsent_root_setup();
  fsent_base_setup();
  fsindex_setup();
  notify_setup();
  filesystem_setup();
  system_setup();
  fsname_setup();
  shadow_setup();
  fstree_setup();
  module_setup();
  configset_setup();
  config_setup();
  notify_thread_setup();
  varset_setup();
  goals_setup();
  handler_setup();
  ruleset_setup();
  buildplan_entity_setup();
  config_system_setup();
  events_setup();
  system_setup();

  xmain();

  // modules
  buildplan_cleanup();
  configset_cleanup();
  config_system_cleanup();
  config_cleanup();
  module_cleanup();
  fsent_cleanup();
  shadow_cleanup();
  notify_thread_cleanup();
  sweeper_thread_cleanup();
  build_cleanup();
  formula_cleanup();
  varset_cleanup();
  fstree_cleanup();
  goals_cleanup();
  handler_cleanup();
  selection_cleanup();
  dependency_cleanup();
  request_cleanup();
  graph_cleanup();
  block_cleanup();
  module_module_cleanup();
  ruleset_cleanup();
  special_edge_cleanup();
  fsindex_edge_cleanup();
  dir_rma_cleanup();
  meta_edge_cleanup();
  rule_module_association_cleanup();
  fsname_cleanup();
  plugin_cleanup();
  events_cleanup();
  fsindex_cleanup();
  notify_cleanup();

  // libraries
  bam_unload();
  lorien_unload();
  moria_unload();
  narrator_unload();
  bacon_unload();

/* disable rcu checks for cleanup */
#if CONFIG_DEBUG || CONFIG_DEVEL
extern __thread bool rcu_is_reader;
rcu_is_reader = true;
#endif

  r = 0;
  r |= g_params.handler_error; /* error on another thread */

  /* best-effort attempt to save our exit status */
  fd = uxopen_modes(O_CREAT | O_WRONLY, 0666, "exit");
  if(fd != -1) {
    snprintf(buf, sizeof(buf), "%d\n", r);
    (void)!write(fd, buf, strlen(buf));
    close(fd);
  }

  /* best-effort attempt to cleanup the lockfile */
  uxunlinks("lock");

  return r;
}
