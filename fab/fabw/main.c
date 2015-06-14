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

#include "xlinux.h"
#include "pstring.h"

#include "global.h"
#include "params.h"

#include "parseint.h"
#include "macros.h"
#include "sigbank.h"

#ifndef DEBUG_IPC
# define DEBUG_IPC 0
#endif

int main(int argc, char** argv)
{
	char space[2048];

	int fd = -1;
	pid_t fab_pid = -1;
	pid_t child_pid = -1;

  // initialize signal handling
  fatal(sigbank_init
#if DEBUG_IPC
    , "fabw", getpid()
#endif
  );

	// std file descriptors
#if !DEBUG_IPC
  int x;
	for(x = 0; x < 3; x++)
		close(x);

	fatal(xopen, "/dev/null", O_RDONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
#endif

#if 0
	for(x = 3; x < 64; x++)
		close(x);
#endif

  // module setup
  fatal(params_setup);

	// required first argument : program name
  if(argc < 2)
    fail(FAB_BADARGS);
  char * child = argv[1];

  // whitelist possible children
  if(strcmp(child, "fabd") && strcmp(child, "faba"))
    fail(FAB_BADARGS);

  // required second argument : ipc hash
	uint32_t canhash;
	if(argc < 3 || parseuint(argv[2], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	snprintf(g_params.ipcdir, sizeof(g_params.ipcdir), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	// fork child
	fatal(xfork, &child_pid);
	if(child_pid == 0)
	{
    // give the child its name
		char * w;
		while((w = strstr(argv[0], "fabw")))
			w[3] = child[3];

    // remove second argument
    argv[1] = argv[2];
    argv[2] = 0;

#if DEVEL
		snprintf(space, sizeof(space), "%s/../%s/%s.devel", g_params.exedir, child, child);
		execv(space, argv);
#else
		execvp(child, argv);
#endif

		tfail(perrtab_SYS, errno);
	}

  // suspend execution pending child status change
	int status;
	fatal(xwaitpid, child_pid, &status, 0);

	// record child exit status
  snprintf(space, sizeof(space), "%s/%s/exit", g_params.ipcdir, child);
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
	fatal(axwrite, fd, &status, sizeof(status));
	fatal(ixclose, &fd);

	// read fab/pid in order to report
	snprintf(space, sizeof(space), "%s/fab/pid", g_params.ipcdir);
	fatal(xopen, space, O_RDONLY, &fd);
	fatal(axread, fd, &fab_pid, sizeof(fab_pid));
	
finally:
  // cleanup
	fatal(ixclose, &fd);

  // module teardown
  params_teardown();

#if DEBUG || DEVEL
  XAPI_INFOF("name", "fabw/%s", child);
  XAPI_INFOF("pid", "%ld", (long)getpid());

	if(XAPI_UNWINDING)
	{
		size_t tracesz = xapi_trace_full(space, sizeof(space));
		logw(L_RED, space, tracesz);
	}
#endif

	// notify fab
  if(fab_pid != -1)
    kill(fab_pid, FABSIG_DONE);

	// kill child, if any
  if(child_pid != -1)
    kill(child_pid, 9);
coda;
}
