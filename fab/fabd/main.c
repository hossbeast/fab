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

// signal handling
static int o_signum;
static void signal_handler(int signum)
{
	if(!o_signum)
		o_signum = signum;
}

int main(int argc, char** argv)
{
	char space[2048];

	int x;
	size_t tracesz = 0;

	// initialize error tables
	error_setup();

#if 0
	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);

	// assume identity of the executing user
	fatal(identity_assume_user);
#endif

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

	// get ipc dir
	uint32_t ipc = 0;
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &ipc, 0) != 0)
		fail(FAB_BADARGS);

	while(o_signum != SIGINT && o_signum != SIGQUIT && o_signum != SIGTERM)
	{
		

		pause();
	}

finally:
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
coda;
}
