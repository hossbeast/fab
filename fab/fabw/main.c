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

#include <sys/types.h>
#include <sys/wait.h>

#include "xapi.h"
#include "types.h"

#include "fab/load.h"
#include "xlinux/load.h"
#include "logger/load.h"
#include "narrator/load.h"

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger.h"
#include "logger/arguments.h"
#include "logger/config.h"
#include "narrator.h"
#include "xapi/SYS.errtab.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdio.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xwait.h"

#include "logging.h"
#include "params.h"

#include "parseint.h"
#include "macros.h"

static xapi xmain_exit;
static xapi xmain()
{
  enter;

#if DEVEL
  char space[4096];
#endif
  char * fabd_path = 0;
  int fd = -1;
  pid_t child_pid = -1;
  pid_t client_pid = -1;
  char hash[16+1] = { 0 };
  char ** argv = 0;
  int i = 0;
  int x;
  uint64_t u64;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  // std file descriptors
  fatal(xclose, 0);
  fatal(xopens, 0, O_RDONLY, "/dev/null");

  // required second argument : ipc hash
  if(g_argc < 2 || parseuint(g_argv[1], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &u64, 0) != 0)
    fail(SYS_BADARGS);
  snprintf(hash, sizeof(hash), "%016"PRIx64, u64);

  // fork child
  fatal(xfork, &child_pid);
  if(child_pid == 0)
  {
#if DEVEL
    snprintf(space, sizeof(space), "%s/../fabd/fabd.devel", g_params.exedir);
    fabd_path = space;
#endif

    fatal(xmalloc, &argv, sizeof(*argv) * (g_argc + g_logc + g_ulogc + 3));
    argv[i++] = "fabd";

    for(x = 1; x < g_argc; x++)
      argv[i++] = g_argv[x];

    for(x = 0; x < g_logc; x++)
      argv[i++] = g_logv[x];

    for(x = 0; x < g_ulogc; x++)
      argv[i++] = g_ulogv[x];

#if DEBUG || DEVEL
    narrator * N;
    argv[0] = "fabd.devel";
    fatal(log_start, L_IPC, &N);
    xsays("execv(");
    xsays(fabd_path ?: "fabd");
    for(x = 0; x < i; x++)
    {
      xsays(",");
      xsays(argv[x]);
    }
    xsays(")");
    fatal(log_finish);
#endif

    if(fabd_path)
      fatal(xexecv, fabd_path, argv);
    else
      fatal(xexecvp, "fabd", argv);
  }

  // suspend and wait for the child to exit
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGTERM);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGCHLD);

  siginfo_t info;
  fatal(sigutil_wait, &sigs, &info);
  fatal(sigutil_assert, SIGCHLD, child_pid, &info);

  // record the exit status
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/%s/fabd/exit", XQUOTE(FABIPCDIR), hash);
  fatal(axwrite, fd, &info.si_status, sizeof(info.si_status));
  fatal(ixclose, &fd);

#if DEBUG || DEVEL
  // preserve a coredump, if any
  if(WIFSIGNALED(info.si_status) && WCOREDUMP(info.si_status))
  {
    fatal(uxrenamef, "%s/%s/fabd/core.%d", "%s/%s/fabd/core", XQUOTE(FABIPCDIR), hash, child_pid, XQUOTE(FABIPCDIR), hash);
  }
#endif

  // signal a client, if any
  fatal(xopenf, &fd, O_RDONLY, "%s/%s/client/pid", XQUOTE(FABIPCDIR), hash);
  fatal(axread, fd, &client_pid, sizeof(client_pid));
  fatal(uxkill, client_pid, FABIPC_SIGEND, 0);

finally:
  // locals
  fatal(ixclose, &fd);
  wfree(argv);
coda;
}

static xapi xmain_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabw");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());

    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE);
#else
    fatal(logger_trace_pithy, L_ERROR, XAPI_TRACE_COLORIZE);
#endif

    xmain_exit = XAPI_ERRVAL;
    xapi_calltree_unwind();
  }
coda;
}

static xapi xmain_load(char ** envp)
{
  enter;

  // load libraries
  fatal(logger_load);
  fatal(narrator_load);
  fatal(xlinux_load);
  fatal(fab_load);

  // modules
  fatal(logging_setup, envp);
  fatal(params_setup);

  fatal(xmain_jump);

finally:
  // modules
  params_teardown();

  // libraries
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(xlinux_unload);
  fatal(fab_unload);
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
    // write failures before liblogger to stderr
    xapi_backtrace();
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;

  return !!R;
}
