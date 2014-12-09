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
#include <sys/wait.h>

#include "xlinux.h"

#include "global.h"

#include "parseint.h"
#include "macros.h"

// signal handling
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
	printf("fabw[%u] %d { pid : %ld", getpid(), signum, (long)info->si_pid);
	if(signum == SIGCHLD)
		printf(", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);
	printf(" }\n");
}

int main(int argc, char** argv)
{
	char stem[2048];
	char space[2048];

	int fd = -1;
	pid_t fab_pid = -1;
	pid_t fabd_pid = -1;

printf("fabw[%lu]\n", (long)getpid());

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
			fatal(xsigaction, SIGINT, &action, 0);
		else
			fatal(xsignal, x, SIG_IGN);
	}
	for(x = SIGRTMIN; x <= SIGRTMAX; x++)
		fatal(xsignal, x, SIG_IGN);

	// std file descriptors
#if 0
	for(x = 0; x < 64; x++)
		close(x);
	fatal(xopen, "/dev/null", O_RDONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
	fatal(xopen, "/dev/null", O_WRONLY, 0);
#endif

	// process parameter gathering
	fatal(params_setup);

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
		snprintf(space, sizeof(space), "%s/../fabd/fabd.devel", g_params.exedir);
		char * w;
		while((w = strstr(argv[0], "fabw")))
			w[3] = 'd';
		execv(space, argv);
#else
		execvp("fabd", argv, argc);
#endif

		tfail(perrtab_SYS, errno);
	}

	// wait for fabd to die
	int status;
	fatal(xwaitpid, fabd_pid, &status, 0);

printf("status : %d\n", WEXITSTATUS(status));
printf("signal : %d\n", WTERMSIG(status));

	// fab-exit file
	snprintf(stem + z, sizeof(stem) - z, "/fabd/exit");
	fatal(xopen_mode, stem, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
	fatal(axwrite, fd, &status, sizeof(status));
	fatal(ixclose, &fd);

	// read fab/pid
	snprintf(stem + z, sizeof(stem) - z, "/fab/pid");
	fatal(gxopen, stem, O_RDONLY, &fd);
	fatal(axread, fd, &fab_pid, sizeof(fab_pid));
	
finally:
	fatal(ixclose, &fd);

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
