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
#include "handler_thread.h"
#include "beholder_thread.h"
#include "channel.h"
#include "var.h"
#include "variant.h"
#include "goals.h"
#include "stats.h"
#include "selection.h"
#include "common/parseint.h"
#include "common/hash.h"

#if DEVEL
static xapi setup_initial_channel(const char * restrict request)
{
  enter;

  size_t len;
  fabipc_channel * chan;
  fabipc_page *page;
  fabipc_message *msg;

  g_logging_skip_reconfigure = true;
  g_server_no_initial_client = true;
  RUNTIME_ASSERT(g_argc > 2);

  len = strlen(request);
  fatal(xmalloc, &chan, sizeof(*chan));
  page = &chan->client_ring.pages[0];
  msg = &page->msg;
  chan->client_ring.tail++;

  memcpy(msg->text, request, len);
  msg->text[len + 0] = 0;
  msg->text[len + 1] = 0;
  msg->size = sizeof(*msg) + len + 2;
  g_server_initial_channel = chan;

  finally : coda;
}
#endif

static xapi xmain_exit;
static xapi xmain()
{
  enter;

  int fds[2];
  sigset_t sigs;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  g_params.thread_monitor = gettid();

  // allow creation of world+rw files
  umask(0);
  fatal(params_report);
  fatal(sigutil_install_handlers);

  sigfillset(&sigs);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

#if DEVEL
  if(g_argc > 2 && strcmp(g_argv[1], "--request") == 0)
  {
    fatal(setup_initial_channel, g_argv[2]);
  }
#endif

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  fatal(xdup2, 1, 601);
  fatal(xdup2, 2, 602);

  /* reopen stdout/stderr to the beholder pipes */
  fatal(xpipe, fds);
  beholder_stdout_rd = fds[0];
  fatal(xdup2, fds[1], 1);
  fatal(xclose, fds[1]);

  fatal(xpipe, fds);
  fatal(xdup2, fds[1], 2);
  beholder_stderr_rd = fds[0];
  fatal(xclose, fds[1]);

  // launch other threads
  fatal(notify_thread_launch);
  fatal(server_thread_launch);
  fatal(sweeper_thread_launch);
  fatal(beholder_thread_launch);

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

#if DEVEL
  if(g_argc > 1)
  {
    if(parseuint(g_argv[1], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &hash, 0) != 0)
    {
      fail(SYS_BADARGS);
    }
    fatal(params_setup, hash);
  }
  else
  {
    char *pwd;
    fatal(xrealpaths, &pwd, 0, getenv("PWD"));
    hash = hash64(0, pwd, strlen(pwd));
    free(pwd);

    fatal(params_setup, hash);

    /* this normally happens in libfab/client right after fork */
    fatal(xchdirf, "%s/fabd", g_params.ipcdir);
    fatal(uxunlinks, "exit");
    fatal(uxunlinks, "core");
    fatal(uxunlinks, "stdout");
    fatal(uxunlinks, "stderr");
  }

  pid_t pid;
  int lockfd = -1;
  fatal(fabipc_lockfile_obtain, &pid, &lockfd, "%s/fabd/lock", g_params.ipcdir);
  if(pid && pid != getpid()) {
    failf(FABD_DAEMONEXCL, "pid", "%ld", (long)pid);
  }
#else
  if(g_argc < 2 || parseuint(g_argv[1], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &hash, 0) != 0)
    fail(SYS_BADARGS);

  fatal(params_setup, hash);
#endif

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
  fatal(handler_thread_setup);
  fatal(beholder_thread_setup);

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
  fatal(handler_thread_cleanup);
  fatal(beholder_thread_cleanup);
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
    xapi_backtrace(2, 0);
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;              // error on the main thread
  R |= g_params.handler_error;  // error on a handler thread

extern void rcu_log(void);
rcu_log();

  return !!R;
}
