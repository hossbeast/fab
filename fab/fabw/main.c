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

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "xapi.h"
#include "xlinux/load.h"
#include "logger/load.h"
#include "narrator/load.h"

#include "xapi/trace.h"
#include "xapi/SYS.errtab.h"
#include "xlinux/xsignal.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xwait.h"
#include "xlinux/xfcntl.h"
#include "narrator.h"
#include "logger.h"
#include "logger/arguments.h"
#include "logger/config.h"
#include "fab/ipc.h"

#include "logging.h"
#include "params.h"

#include "parseint.h"
#include "macros.h"

static xapi begin(int argc, char ** argv, char ** envp)
{
  enter;

#if DEVEL
  char space[4096];
#endif
  char * fabd_path = 0;

  int fd = -1;
  pid_t child_pid = -1;
  pid_t client_pid = -1;
  char hash[9] = { 0 };

  // std file descriptors
  fatal(xclose, 0);
  fatal(xopens, 0, O_RDONLY, "/dev/null");

  // required second argument : ipc hash
  uint32_t u32;
  if(argc < 2 || parseuint(g_argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &u32, 0) != 0)
    fail(SYS_BADARGS);
  snprintf(hash, sizeof(hash), "%x", u32);

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  // fork child
  fatal(xfork, &child_pid);
  if(child_pid == 0)
  {
#if DEVEL
    snprintf(space, sizeof(space), "%s/../fabd/fabd.devel", g_params.exedir);
    fabd_path = space;
#endif

    argv[0] = "fabd";

#if DEBUG || DEVEL
    narrator * N;
    argv[0] = "fabd.devel";
    fatal(log_start, L_IPC, &N);
    xsays("execv(");
    xsays(fabd_path ?: "fabd");
    int x;
    for(x = 0; x < sentinel(argv); x++)
    {
      if(*argv[x])
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

  // suspend execution pending child status change
  int status;
  fatal(xwaitpid, child_pid, &status, 0);

#if DEBUG || DEVEL
#if 0
  // if fabd terminated abnormally
  if(WIFEXITED(status)) { }
  else if(WEXITSTATUS(status) || WIFSIGNALED(status))
  {
    if(WEXITSTATUS(status))
      xlogf(L_ERROR, L_RED, "fabd : exited status %d", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
      xlogf(L_ERROR, L_RED, "fabd : term signal %d %s", WTERMSIG(status), strsignal(WTERMSIG(status)));
  }
#endif
#endif

  // record the exit status
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/%s/fabd/exit", XQUOTE(FABIPCDIR), hash);
  fatal(axwrite, fd, &status, sizeof(status));
  fatal(ixclose, &fd);

  // signal a client, if any
  fatal(xopenf, &fd, O_RDONLY, "%s/%s/client/pid", XQUOTE(FABIPCDIR), hash);
  fatal(axread, fd, &client_pid, sizeof(client_pid));
  fatal(uxkill, client_pid, FABIPC_SIGEND, 0);

finally:
  // locals
  fatal(ixclose, &fd);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  xapi R;

  // load libraries
  fatal(logger_load);
  fatal(narrator_load);
  fatal(xlinux_load);

  // modules
  fatal(logging_setup, envp);
  fatal(params_setup);

  fatal(begin, argc, argv, envp);
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
  }

  // modules
  params_teardown();

  // libraries
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(xlinux_unload);
conclude(&R);
  xapi_teardown();

  return !!R;
}
