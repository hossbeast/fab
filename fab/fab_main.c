/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "log.h"
#include "fab_control.h"

#include "args.h"
#include "params.h"
#include "tmp.h"
#include "identity.h"

#include "macros.h"
#include "xmem.h"

// signal handling
static int o_signum;
static void signal_handler(int signum)
{
	exit(0);
	if(!o_signum)
	{
		o_signum = signum;
	}
}

int main(int argc, char** argv)
{
//	char space[256];
	path * fabpath = 0;

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions
	fatal(identity_init);

	// assume identity of the executing user
	fatal(identity_assume_user);

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// ignore most signals
	int x;
	for(x = 0; x < NSIG; x++)
		signal(x, SIG_DFL);

	// handle these signals by terminating gracefully.
	signal(SIGINT		, signal_handler);	// terminate gracefully
	signal(SIGQUIT	, signal_handler);
	signal(SIGTERM	, signal_handler);

	// create/cleanup tmp 
	fatal(tmp_setup);

	// preliminary arguments parsing to get init-fabfile
	fatal(path_create_init, &fabpath, g_params.cwd, "%s", DEFAULT_INIT_FABFILE);

	for(x = 0; x < (argc - 1); x++)
	{
		if(strcmp(argv[x], "-f") == 0)
		{
			path_xfree(&fabpath);
			fatal(path_create_init, &fabpath, g_params.cwd, "%s", argv[x+1]);
			break;
		}
	}

	// attach to fabd for this init-fabfile
	int shmid = 0;
	if((shmid = shmget(fabpath->can_hash, 0, 0)) == -1)
	{
		if(errno != ENOENT)
		{
			fail("shmget failed : [%d][%s]", errno, strerror(errno));
		}
	}

	if(shmid == -1)
	{
		// launch a fabd instance
		pid_t pid = 0;
		if((pid = fork()) == -1)
		{
			fail("fork failed with: [%d][%s]", errno, strerror(errno));
		}
		else if(pid == 0)
		{
			execlp("./fabd", "fabd", fabpath->in_path, (char*)0);
		}

		log(L_INFO, "launched fabd[%u]", pid);
		pause();

		if((shmid = shmget(fabpath->can_hash, 0, 0)) == -1)
		{
			if(errno != ENOENT)
			{
				fail("shmget failed : [%d][%s]", errno, strerror(errno));
			}
		}
	}

printf("fab shmid %d\n", shmid);

	if(shmid == -1)
	{
		fail("unable to attach to fabd");
	}

	// parse cmdline arguments
	//  (args_parse also calls log_init with a default string)
	fatal(args_parse, argc, argv);

finally:
	path_free(fabpath);

	args_teardown();
	params_teardown();

	log(L_INFO, "exiting with status : %d", _coda_r);
	log_teardown();
	return _coda_r;
}
