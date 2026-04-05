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
#include "block.h"
#include "bpe.h"
#include "build.h"
#include "build_record.h"
#include "buildplan.h"
#include "channel.h"
#include "channel_server_thread.h"
#include "client.h"
#include "command.h"
#include "config.h"
#include "config_system.h"
#include "configset.h"
#include "dependency.h"
#include "detail.h"
#include "dir_fsname_edge.h"
#include "dir_rma_edge.h"
#include "events/events.h"
#include "exec_value.h"
#include "filesystem.h"
#include "formula.h"
#include "formula_record.h"
#include "fsedge.h"
#include "fsent.h"
#include "fsname.h"
#include "fstree.h"
#include "goals.h"
#include "meta_edge.h"
#include "module_edge.h"
#include "monitor_thread.h"
#include "namespace.h"
#include "notify.h"
#include "notify_thread.h"
#include "ns_edge.h"
#include "params.h"
#include "plugin.h"
#include "plugin_process.h"
#include "plugin_system.h"
#include "plugin_thread.h"
#include "rcu.h"
#include "ref_edge.h"
#include "request_handler.h"
#include "rma.h"
#include "rule_system.h"
#include "ruleset.h"
#include "selection.h"
#include "shadow.h"
#include "special_edge.h"
#include "stats.h"
#include "sweeper_thread.h"
#include "system.h"
#include "sysvar.h"
#include "timeout_thread.h"
#include "timers.h"
#include "varset.h"
#include "vertices.h"

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

/* disable rcu checks for cleanup */
#if CONFIG_DEBUG || CONFIG_DEVEL
extern __thread bool rcu_is_reader;
rcu_is_reader = true;
#endif

#if CONFIG_DEVEL

  // modules
  fsent_root_cleanup();
  buildplan_cleanup();
  configset_cleanup();
  config_system_cleanup();
  config_cleanup();
  module_cleanup();
  shadow_cleanup();
  notify_thread_cleanup();
  sweeper_thread_cleanup();
  build_cleanup();
  formula_cleanup();
  varset_cleanup();
  goals_cleanup();
  request_handler_cleanup();
  selection_cleanup();
  dependency_cleanup();
  block_cleanup();
  module_module_cleanup();
  ruleset_cleanup();
  sysvar_cleanup();
  special_edge_cleanup();
  dir_rma_cleanup();
  rule_module_association_cleanup();
  plugin_cleanup();
  plugin_system_cleanup();
  plugin_process_cleanup();
  plugin_thread_cleanup();
  detail_cleanup();
  fsname_cleanup();
  events_cleanup();
  namespace_cleanup();
  fstree_cleanup();
  notify_cleanup();
  vertices_cleanup();
  exec_value_cleanup();
  lookup_cleanup();
  request_cleanup();
  command_cleanup();
  channel_cleanup();
  client_cleanup();
  build_record_cleanup();
  formula_record_cleanup();
  buildplan_entity_cleanup();
  rule_cleanup();
  fsent_cleanup();
  ns_edge_cleanup();
  dir_fsname_edge_cleanup();
  ref_edge_cleanup();
  fsedge_cleanup();
  meta_edge_cleanup();
  graph_cleanup();

  system_stats_show();

  fsents_show();
  fstrees_show();
  formulas_show();
  modules_show();
  rmas_show();
  rules_show();
  bpes_show();
  formula_records_show();
  rulesets_show();
  plugins_show();
  fsnames_show();
  detail_show();
  varsets_show();
  configsets_show();
  ns_edges_show();
  dir_fsname_edges_show();
  ref_edges_show();
  module_edges_show();

  // libraries
  bam_unload();
  lorien_unload();
  moria_unload();
  narrator_unload();
  bacon_unload();

  /* best-effort attempt to save final resource usage stats */
  fd = uxopen_modes(O_CREAT | O_WRONLY, 0666, "objects");
  if(fd != -1) {
    system_stats_show_fd(fd);
    close(fd);
  }
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
