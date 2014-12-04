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
static int o_signum;
static void signal_handler(int signum)
{
	if(!o_signum)
		o_signum = signum;

	printf("fabd[%u] rcv %d\n", getpid(), signum);
}

int main(int argc, char** argv)
{
	char space[2048];
	char space2[2048];

	int fd = -1;
	int argsfd = -1;
	pid_t fab_pid = 0;

	int x;
	size_t tracesz = 0;

	// initialize error tables
	error_setup();

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);

	// assume identity of the executing user
	fatal(identity_assume_user);

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
	signal(SIGHUP		, signal_handler);

	// stdin is not needed
	close(0);
	fatal(xopen, "/dev/null", O_RDONLY, 0);

	// initialize logger
	fatal(log_init);

	// get ipc dir
	uint32_t canhash;
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	size_t z = snprintf(space, sizeof(space), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	snprintf(space2, sizeof(space2), "%s/args", space);

	// open args file
	fatal(xopen, space2, O_RDWR, &argsfd);

	struct stat stb;
	while(o_signum != SIGINT && o_signum != SIGQUIT && o_signum != SIGTERM)
	{
		// read fab/pid
		snprintf(space + z, sizeof(space) - z, "/fab/pid");
		fatal(ixclose, &fd);
		fatal(gxopen, space, O_RDONLY, &fd);
		fatal(axread, fd, &fab_pid, sizeof(fab_pid));

		// existence check
		fatal(xkill, fab_pid, 0);

		// reopen file descriptors
		fatal(xclose, 1);
		snprintf(space2, sizeof(space2), "/proc/%ld/fd/1", (long)fab_pid);
		fatal(xopen, space2, O_RDWR, 0);

		fatal(xclose, 2);
		snprintf(space2, sizeof(space2), "/proc/%ld/fd/2", (long)fab_pid);
		fatal(xopen, space2, O_RDWR, 0);
		
		// release previous args mapping
		fatal(ixmunmap, &g_args, stb.st_size);

		// map the file
		fatal(xfstat, argsfd, &stb);
		fatal(xmmap, 0, stb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, argsfd, 0, (void*)&g_args);

		// unpack the args
		args_thaw((void*)g_args);

#if 1
	// re-configure logger
#if DEBUG || DEVEL
	if(g_args->mode_logtrace == MODE_LOGTRACE_FULL)
	{
		fatal(log_config_and_describe
			, L_TAG																												// prefix
			, L_LWOPINFO | L_LWTOKEN | L_LWSTATE | L_FFTOKEN | L_FFSTATE	// trace
			, L_INFO																											// describe bits
		);
	}
	else
	{
		fatal(log_config_and_describe
			, L_TAG
			, 0
			, L_INFO
		);
	}
#else
	fatal(log_config, L_TAG);	// prefix
#endif
#endif

		// log args summary
		fatal(args_summarize);

		// task complete : notify fab
		fatal(xkill, fab_pid, 1);

		// wait for next command
		o_signum = 0;
		pause();
	}

finally:

	fatal(ixmunmap, &g_args, stb.st_size);
	fatal(ixclose, &argsfd);
	fatal(ixclose, &fd);

	if(XAPI_UNWINDING)
	{
#if 0
#if DEBUG || DEVEL
		if(g_args.mode_backtrace == MODE_BACKTRACE_PITHY)
		{
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEBUG || DEVEL
		}
		else
		{
			tracesz = xapi_trace_full(space, sizeof(space));
		}
#endif
#endif

		tracesz = xapi_trace_full(space, sizeof(space));
		logw(L_RED, space, tracesz);
	}
	else
	{
		logs(L_INFO, "exiting with status : 0");
	}

	if(fab_pid)
		kill(fab_pid, 1);
coda;
}
