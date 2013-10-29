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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>

#include "identity.h"

#include "fab_control.h"
#include "path.h"
#include "params.h"

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
// char space[256];
	path * fabpath = 0;
	int shmid = 0;

	// process parameters
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
	int x;
	for(x = 0; x < NSIG; x++)
		signal(x, SIG_DFL);

	// handle these signals by terminating gracefully.
	signal(SIGINT		, signal_handler);	// terminate gracefully
	signal(SIGQUIT	, signal_handler);
	signal(SIGTERM	, signal_handler);

	// create shm segment for this init-fabfile
	if(argc < 2 || argv[1] == 0)
		fail("unable to create init-fabfile-path");
	fatal(path_create_init, &fabpath, g_params.cwd, "%s", argv[1]);

	if((shmid = shmget(fabpath->can_hash, 1000, IPC_CREAT | IPC_EXCL)) == -1)
		fail("shmget failed : [%d][%s]", errno, strerror(errno));

	printf("fabd shmid %d\n", shmid);

	kill(g_params.ppid, 1);

finally:
	path_free(fabpath);

	log(L_INFO, "exiting with status : %d", _coda_r);
	log_teardown();
	return _coda_r;
}
