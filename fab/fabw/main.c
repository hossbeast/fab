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

#include "parseint.h"
#include "macros.h"
#include "sigbank.h"

#ifndef DEBUG_IPC
# define DEBUG_IPC 0
#endif

int main(int argc, char** argv)
{
	char stem[2048];
	char space[2048];

	int fd = -1;
	pid_t fab_pid = -1;
	pid_t fabd_pid = -1;
	size_t z;

  int x;

  // initialize signal handling
  fatal(sigbank_init
#if DEBUG_IPC
    , "fabw", getpid()
#endif
  );

	// std file descriptors
#if !DEBUG_IPC
	for(x = 0; x < 3; x++)
		close(x);

	fatal(xopen, "/dev/null", O_RDONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
#endif

	for(x = 3; x < 64; x++)
		close(x);

	// process parameter gathering
	fatal(params_setup);

	// get ipc dir
	uint32_t canhash;
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	z = snprintf(stem, sizeof(stem), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	// fork fabd
	fatal(xfork, &fabd_pid);
	if(fabd_pid == 0)
	{
		// tell fabd its name
		char * w;
		while((w = strstr(argv[0], "fabw")))
			w[3] = 'd';

#if DEVEL
		snprintf(space, sizeof(space), "%s/../fabd/fabd.devel", g_params.exedir);
		execv(space, argv);
#else
		execvp("fabd", argv);
#endif

		tfail(perrtab_SYS, errno);
	}

  // suspend execution pending fabd status change
	int status;
	fatal(xwaitpid, fabd_pid, &status, 0);

	// fabd-exit file
	snprintf(stem + z, sizeof(stem) - z, "/fabd/exit");
	fatal(xopen_mode, stem, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
	fatal(axwrite, fd, &status, sizeof(status));
	fatal(ixclose, &fd);

	// read fab/pid
	snprintf(stem + z, sizeof(stem) - z, "/fab/pid");
	fatal(xopen, stem, O_RDONLY, &fd);
	fatal(axread, fd, &fab_pid, sizeof(fab_pid));
	
finally:
	fatal(ixclose, &fd);

#if DEBUG || DEVEL
  XAPI_INFOF("pid", "%ld", (long)getpid());
#endif

	if(XAPI_UNWINDING)
	{
		size_t tracesz = xapi_trace_full(space, sizeof(space));
		logw(L_RED, space, tracesz);
	}

	// notify fab
	if(fab_pid != -1)
		kill(fab_pid, 15);

	// kill fabd, if any
	if(fabd_pid != -1)
		kill(fabd_pid, 9);
coda;
}
