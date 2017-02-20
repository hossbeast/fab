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
#include "xlinux/xunistd.h"
#include "xlinux/xwait.h"
#include "xlinux/xfcntl.h"
#include "narrator.h"
#include "logger.h"
#include "logger/arguments.h"
#include "fab/ipc.h"

#include "logging.h"
#include "params.h"

#include "parseint.h"
#include "macros.h"

int main(int argc, char ** argv, char ** envp)
{
  enter;

  xapi R = 0;

  char space[512];
  char * fabd_path = 0;

  int token = 0;
  int fd = -1;
  pid_t child_pid = -1;
  pid_t client_pid = -1;
  char hash[9] = { 0 };

  // load libraries
  fatal(logger_load);
  fatal(narrator_load);
  fatal(xlinux_load);

  // logging
  fatal(logging_setup);
  fatal(logger_arguments_setup, envp);
  fatal(logger_finalize);

  // modules
  fatal(params_setup);

  // std file descriptors
  fatal(xclose, 0);
  fatal(xopens, 0, O_RDONLY, "/dev/null");

  // required second argument : ipc hash
  uint32_t u32;
  if(argc < 2 || parseuint(g_argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &u32, 0) != 0)
    tfail(perrtab_SYS, SYS_BADARGS);
  snprintf(hash, sizeof(hash), "%x", u32);

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  // block libfab signals
  sigset_t app_set;
  sigemptyset(&app_set);
  sigaddset(&app_set, FABIPC_SIGSYN);
  sigaddset(&app_set, FABIPC_SIGACK);
  sigaddset(&app_set, FABIPC_SIGEND);

  fatal(xsigprocmask, SIG_SETMASK, &app_set, 0);

  // fork child
  fatal(xfork, &child_pid);
  if(child_pid == 0)
  {
#if DEVEL
    snprintf(space, sizeof(space), "%s/../fabd/fabd.devel", g_params.exedir);
    fabd_path = space;
#endif

    char * argv[] = {
#if DEVEL
        "fabd.devel"
#else
        "fabd"
#endif
      , hash, (void*)0
    };

#if DEBUG || DEVEL
    fatal(log_start, L_IPC, &token);
    logs(0, "execv(");
    logs(0, fabd_path ?: "fabd");
    int x;
    for(x = 0; x < sentinel(argv); x++)
    {
      logs(0, ",");
      logs(0, argv[x]);
    }
    logs(0, ")");
    fatal(log_finish, &token);
#endif

    if(fabd_path)
      fatal(execv, fabd_path, argv);
    else
      fatal(execvp, "fabd", argv);
  }

  // suspend execution pending child status change
  int status;
  fatal(xwaitpid, child_pid, &status, 0);

#if DEBUG || DEVEL
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

  // record the exit status
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/%s/fabd/exit", XQUOTE(FABIPCDIR), hash);
  fatal(axwrite, fd, &status, sizeof(status));
  fatal(ixclose, &fd);

  // signal a client, if any
  fatal(xopenf, &fd, O_RDONLY, "%s/%s/client/pid", XQUOTE(FABIPCDIR), hash);
  fatal(axread, fd, &client_pid, sizeof(client_pid));
  fatal(uxkill, client_pid, FABIPC_SIGEND, 0);

finally:
  fatal(log_finish, &token);

#if DEBUG || DEVEL
  if(log_would(L_IPC))
  {
    xapi_infos("name", "fabw");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infos("hash", hash);
  }
#endif

  if(XAPI_UNWINDING)
  {
    size_t tracesz;
#if DEBUG || DEVEL
    tracesz = xapi_trace_full(space, sizeof(space));
#else
    tracesz = xapi_trace_pithy(space, sizeof(space));
#endif

    xlogw(L_ERROR, L_RED, space, tracesz);
  }

  // locals
  fatal(ixclose, &fd);

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
