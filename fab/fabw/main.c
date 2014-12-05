/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include "xlinux.h"

#include "global.h"

#include "parseint.h"
#include "macros.h"

// signal handling
static void signal_handler(int signum)
{
	printf("fabw[%u] rcv %d\n", getpid(), signum);
}

int main(int argc, char** argv)
{
	char stem[2048];
	char space[2048];

	int fd = -1;
	pid_t fab_pid = -1;
	pid_t fabd_pid = -1;

	// process parameter gathering
	fatal(params_setup);

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// ignore most signals
	for(x = 0; x < NSIG; x++)
		signal(x, SIG_DFL);

	// handle these signals by terminating gracefully.
	signal(SIGINT		, signal_handler);	// terminate gracefully
	signal(SIGQUIT	, signal_handler);
	signal(SIGTERM	, signal_handler);
	signal(SIGCHLD	, signal_handler);

	// stdin is not needed
	close(0);
	fatal(xopen, "/dev/null", O_RDONLY, 0);

	// get ipc dir
	uint32_t canhash;
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	size_t z = snprintf(stem, sizeof(stem), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	// fork fabd
	fatal(xfork, &fabd_pid);
	if(fabd_pid == 0)
	{
#if DEVEL
		snprintf(space, sizeof(space), "%s/../fabw/fabw.devel", g_params.exedir);
		execv(space, argv, argc);
#else
		execvp("fabw", argv, argc);
#endif
	}

	// wait for fabd to die
	pause();

	// read fab/pid
	snprintf(stem + z, sizeof(stem) - z, "/fab/pid");
	fatal(gxopen, stem, O_RDONLY, &fd);
	fatal(axread, fd, &fab_pid, sizeof(fab_pid));
	
finally:
	fatal(ixclose, &fd);

	if(XAPI_UNWINDING)
	{
		tracesz = xapi_trace_full(space, sizeof(space));
		logw(L_RED, space, tracesz);
	}

	// notify fab
	if(fab_pid != -1)
		kill(fab_pid, 15);
coda;
}
