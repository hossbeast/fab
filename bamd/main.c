/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "bacon/load.h"
#include "bam/load.h"
#include "bam/lockfile.h"
#include "bam/sigutil.h"
#include "lorien/load.h"
#include "moria/load.h"
#include "narrator/load.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xpthread.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"

#include "args.h"
#include "bpe.h"
#include "build.h"
#include "buildplan.h"
#include "channel.h"
#include "channel_server_thread.h"
#include "client.h"
#include "config.h"
#include "config_system.h"
#include "configset.h"
#include "dir_fsname_edge.h"
#include "events/events.h"
#include "exec_value.h"
#include "formula.h"
#include "filesystem.h"
#include "formula_record.h"
#include "fsent.h"
#include "fsname.h"
#include "fstree.h"
#include "goals.h"
#include "monitor_thread.h"
#include "namespace.h"
#include "notify.h"
#include "notify_thread.h"
#include "ns_edge.h"
#include "params.h"
#include "plugin.h"
#include "plugin_system.h"
#include "plugin_thread.h"
#include "request_handler.h"
#include "rule_system.h"
#include "ruleset.h"
#include "shadow.h"
#include "sweeper_thread.h"
#include "system.h"
#include "sysvar.h"
#include "timeout_thread.h"
#include "timers.h"
#include "varset.h"
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

  /* signal handling */
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
  char buf[PATH_MAX];

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
  snprintf(buf, sizeof(buf), "%.*s/basedir", (int)g_params.ipcdir_len, g_params.ipcdir);
  xreadlinks(buf, g_basedir, sizeof(g_basedir));
  g_basedir_len = strlen(g_basedir);
  g_basedirfd = xopens(O_PATH | O_DIRECTORY, g_basedir);

  dprintf(1, "bamd_id : %.*s\n", g_args.bamd_id_len, g_args.bamd_id);
  dprintf(1, "pid : %d\n", g_params.pid);
  dprintf(1, "homedir : %.*s\n", (int)g_params.homedir_len, g_params.homedir);
  dprintf(1, "ipcdir : %.*s\n", (int)g_params.ipcdir_len, g_params.ipcdir);
  dprintf(1, "basedir : %.*s\n", (int)g_basedir_len, g_basedir);

  /* for convenience, display basedir in argv */
  if(g_argv_basedir_space) {
    memset(g_argv_basedir_space, 0, 100);
    zcatf(g_argv_basedir_space, 100, "%.*s", g_basedir_len, g_basedir);
  }

  getlock();

  /* this normally happens in libbam/client right after fork */
  xchdirf("%s/bamd", g_params.ipcdir);
  uxunlinks("exit");
  uxunlinks("core");

  // modules
  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  rcu_setup();
  build_setup();
  formula_setup();
  buildplan_setup();
  graph_setup();
  rule_system_setup();
  fsent_base_setup();
  namespace_setup();
  notify_setup();
  filesystem_setup();
  system_setup();
  fsname_setup();
  config_setup();
  goals_setup();
  module_setup();
  configset_setup();
  notify_thread_setup();
  varset_setup();
  request_handler_setup();
  ruleset_setup();
  sysvar_setup();
  buildplan_entity_setup();
  formula_record_setup();
  vertices_setup();
  config_system_setup();
  events_setup();
  exec_value_setup();
  plugin_system_setup();
  plugin_setup();
  channel_setup();
  client_setup();
  ns_edge_setup();
  dir_fsname_edge_setup();

  xmain();

  smp_mb();

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
