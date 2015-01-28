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
#include "say.h"

#define DEBUG_IPC 0

// signal handling
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
#if DEBUG_IPC
	char space[2048];
	char * dst = space;
	size_t sz = sizeof(space);
	size_t z = 0;

	z += znprintf(dst + z, sz - z, "fabw[%u] received %d { from pid : %ld", getpid(), signum, (long)info->si_pid);

	if(signum == SIGCHLD)
		z += znprintf(dst + z, sz - z, ", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);

	z += znprintf(dst + z, sz - z, " }\n");
	int __attribute__((unused)) r = write(1, space, z);
#endif
}

int main(int argc, char** argv)
{
	char stem[2048];
	char space[2048];

	int fd = -1;
	pid_t fab_pid = -1;
	pid_t fabd_pid = -1;
	size_t z;

	pstring * ptmp = 0;

#if DEBUG_IPC
	SAYF("fabw[%lu] started\n", (long)getpid());
#endif

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// ignore most signals, handle a few
	struct sigaction action = {
		  .sa_sigaction = signal_handler
		, .sa_flags = SA_SIGINFO
	};

	int x;
	for(x = 1; x < SIGUNUSED; x++)
	{
		if(x == SIGKILL || x == SIGSTOP || x == SIGSEGV) { }
		else if(x == SIGINT || x == SIGQUIT || x == SIGTERM || x == SIGCHLD)
			fatal(xsigaction, x, &action, 0);
		else
			fatal(xsignal, x, SIG_IGN);
	}
	for(x = SIGRTMIN; x <= SIGRTMAX; x++)
  {
#if DEBUG || DEVEL
    if((x - SIGRTMIN) == 30)
    {
      // internal valgrind signal
      continue;
    }
#endif

		// all of the fab ipc signals are in this range ; they are silently ignored by fabw
		fatal(xsignal, x, SIG_IGN);
	}

	// std file descriptors
#if DEBUG_IPC
	for(x = 3; x < 64; x++)
#else
	for(x = 0; x < 64; x++)
#endif
	{
		close(x);
	}
	fatal(xopen, "/dev/null", O_RDONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);

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

	// wait for fabd to die
	int status;
	fatal(xwaitpid, fabd_pid, &status, 0);

	// fabd-exit file
	snprintf(stem + z, sizeof(stem) - z, "/fabd/exit");
	fatal(xopen_mode, stem, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
	fatal(axwrite, fd, &status, sizeof(status));
	fatal(ixclose, &fd);

	// read fab/pid
	snprintf(stem + z, sizeof(stem) - z, "/fab/pid");
	fatal(xopen, stem, O_RDONLY, &fd);
	fatal(axread, fd, &fab_pid, sizeof(fab_pid));
	
finally:
	fatal(ixclose, &fd);
	psfree(ptmp);

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
