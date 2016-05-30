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
#include "xapi/trace.h"
#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

#include "fabcore.h"
#include "fabcore/params.h"
#include "fabcore/FAB.errtab.h"
#include "fabcore/ipc.h"

#include "global.h"
#include "logging.h"

#include "parseint.h"
#include "macros.h"

int main(int argc, char ** argv, char ** envp)
{
  enter;

  xapi R = 0;
  char ipcdir[512];

  int token = 0;
	int fd = -1;
	pid_t child_pid = -1;
	char hash[9] = { 0 };

  // load libraries
  fatal(xlinux_load);
  fatal(fabcore_load);

#if 0
	// std file descriptors
  int x;
	for(x = 0; x < 3; x++)
		close(x);

	fatal(xopen, "/dev/null", O_RDONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
#endif

  // required second argument : ipc hash
  uint32_t u32;
	if(argc < 2 || parseuint(argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &u32, 0) != 0)
		fail(FAB_BADARGS);
  snprintf(hash, sizeof(hash), "%08x", u32);
  snprintf(ipcdir, sizeof(ipcdir), "%s/%s", XQUOTE(FABIPCDIR), hash);

#if DEBUG || DEVEL
	char space[4096];
  snprintf(space, sizeof(space), "%ld/%ld/%s", (long)getpgid(0), (long)getpid(), "fabw");
  dprintf(1, "%s%*s started\n", space, (int)MAX(0, 20 - strlen(space)), "");
#endif

  // block FABSIG signals
  sigset_t app_set;
  sigemptyset(&app_set);
  sigaddset(&app_set, FABSIG_SYN);
  sigaddset(&app_set, FABSIG_ACK);

  fatal(xsigprocmask, SIG_SETMASK, &app_set, 0);

	// fork child
	fatal(xfork, &child_pid);
	if(child_pid == 0)
	{
#if DEVEL
    snprintf(space, sizeof(space), "%s/../fabd/fabd.devel", g_params.exedir);
    char * argvp[] = { space, hash, "+ALL", (void*)0 };
    execv(space, argvp);
#else
    execlp("fabd", "fabd", hash, (void*)0);
#endif

		tfail(perrtab_SYS, errno);
	}

  // suspend execution pending child status change
	int status;
	fatal(xwaitpid, child_pid, &status, 0);

  // if fabd terminated abnormally
  if(WIFEXITED(status)) { }
  else if(WEXITSTATUS(status))
  {
    printf("fabd : exited status %d\n", WEXITSTATUS(status));
  }
  else if(WIFSIGNALED(status))
  {
    printf("fabd : term signal %d %s\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
  }
	
finally:
  fatal(log_finish, &token);

#if XAPI_STACKTRACE
	if(XAPI_UNWINDING)
	{
    xapi_infos("name", "fabw");
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infos("hash", hash);

    xapi_backtrace_to(1);
	}
#endif

  // locals
	fatal(ixclose, &fd);

  // libraries
  fatal(xlinux_unload);
  fatal(fabcore_unload);

conclude(&R);
  xapi_teardown();

  return !!R;
}
