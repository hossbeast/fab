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
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "xlinux.h"
#include "xlinux/mempolicy.h"

#include "global.h"

#include "macros.h"
#include "memblk.h"

// signal handling
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
	char space[2048];
	char * dst = space;
	size_t sz = sizeof(space);
	size_t z = 0;

	z += znprintf(dst + z, sz - z, "fab[%u] received %d { from pid : %ld", getpid(), signum, (long)info->si_pid);
	if(signum == SIGCHLD)
		z += znprintf(dst + z, sz - z, ", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);

	z += znprintf(dst + z, sz - z, " }\n");
	int __attribute__((unused)) r = write(1, space, z);
}

static int fabdcred_throw(uid_t actual_uid, gid_t actual_gid)
{
	char space[256];
	char space2[256];
	char space3[256];

	struct passwd pwd;
	struct passwd * ppwd;
	fatal(uxgetpwuid_r, actual_uid, &pwd, space, sizeof(space), &ppwd);

	struct group grp;
	struct group * pgrp;
	fatal(uxgetgrgid_r, actual_gid, &grp, space2, sizeof(space2), &pgrp);

	fail(FAB_FABDCRED);

finally:
	if(XAPI_ERRCODE == FAB_FABDCRED)
	{
		snprintf(space3, sizeof(space3), "%.*s/%d:%.*s/%d"
			, g_params.ruid_namel, g_params.ruid_name, g_params.ruid
			, g_params.rgid_namel, g_params.rgid_name, g_params.rgid
		);
		XAPI_INFOS("expected", space3);

		if(ppwd && pgrp)
			snprintf(space3, sizeof(space3), "%s/%d:%s/%d", ppwd->pw_name, actual_uid, pgrp->gr_name, actual_gid);
		else if(ppwd)
			snprintf(space3, sizeof(space3), "%s/%d:%d", ppwd->pw_name, actual_uid, actual_gid);
		else if(pgrp)
			snprintf(space3, sizeof(space3), "%d:%s/%d", actual_uid, pgrp->gr_name, actual_gid);

		XAPI_INFOS("actual", space3);
	}
coda;
}

int main(int argc, char** argv)
{
	char space[2048];
#if DEVEL
	char space2[2048];
#endif

#if DEBUG || DEVEL
	int mode_backtrace = DEFAULT_MODE_BACKTRACE;
#endif
	int fd = -1;
	int lockfd = -1;
	uint32_t canhash;
	int r = -1;
	int fabd_exit = 0;

	memblk * mb = 0;

printf("fab[%ld]\n", (long)getpid());

	int x;
	size_t tracesz = 0;

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// handle all signals
	struct sigaction action = {
		  .sa_sigaction = signal_handler
		, .sa_flags = SA_SIGINFO
	};
	for(x = 1; x < SIGUNUSED; x++)
	{
		if(x != SIGKILL && x != SIGSTOP && x != SIGSEGV)
			fatal(xsigaction, x, &action, 0);
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

		fatal(xsigaction, x, &action, 0);
	}

	// initalize logger
	fatal(log_init);

	// allocate memblock to store the args
	fatal(memblk_mk, &mb);

	// redirect allocations to the memblock
	mempolicy_engage(memblk_getpolicy(mb));

	// parse cmdline arguments into the block
	fatal(args_parse);
	mempolicy_release(memblk_getpolicy(mb));

	// save a few options that pertain to this process
#if DEBUG || DEVEL
	mode_backtrace = g_args->mode_backtrace;
#endif
	canhash = g_args->init_fabfile_path->can_hash;

/*
/FABIPCDIR/<hash>/fabfile				<-- init fabfile path, symlink
/FABIPCDIR/<hash>/args					<-- args, binary
/FABIPCDIR/<hash>/logs					<-- logs, ascii
/FABIPCDIR/<hash>/fab/pid				<-- fab pid, ascii
/FABIPCDIR/<hash>/fab/lock			<-- fab lockfile, empty
/FABIPCDIR/<hash>/fabd/pgid			<-- fabd pgid, ascii
/FABIPCDIR/<hash>/fabd/lock			<-- fabd lockfile, empty
/FABIPCDIR/<hash>/fabd/cred			<-- fabd ruid/rgid, binary
/FABIPCDIR/<hash>/fabd/exit			<-- fabd exit status, binary
*/

	// assume fabsys identity
	fatal(identity_assume_fabsys);

	// ipc-dir stem
	size_t z = snprintf(space, sizeof(space), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	// fab directory
	snprintf(space + z, sizeof(space) - z, "/fab");
	fatal(mkdirp, space, S_IRWXU | S_IRWXG);

	// canonical fabfile symlink
	snprintf(space + z, sizeof(space) - z, "/fabfile");
	fatal(uxsymlink, g_args->init_fabfile_path->can, space);

	// fab-lock file
	snprintf(space + z, sizeof(space) - z, "/fab/lock");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &lockfd);
	fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

	// fab-pid file
	snprintf(space + z, sizeof(space) - z, "/fab/pid");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
	fatal(axwrite, fd, &g_params.pid, sizeof(g_params.pid));
	fatal(ixclose, &fd);

	// fabd directory
	snprintf(space + z, sizeof(space) - z, "/fabd");
	fatal(mkdirp, space, S_IRWXU | S_IRWXG);

	// open args file for writing
	snprintf(space + z, sizeof(space) - z, "/args");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);

	// set the filesize
	fatal(xftruncate, fd, memblk_size(mb));

	// g_args unusable beyond this point
	args_freeze(mb);

	// write the block to the file
	fatal(memblk_writeto, mb, fd);
	fatal(ixclose, &fd);

	// open logs file for writing
	snprintf(space + z, sizeof(space) - z, "/logs");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);

	// set the filesize
	fatal(xftruncate, fd, g_logvsl);

	// write the logs : g_logs unusable beyond this point
	fatal(axwrite, fd, g_logvs, g_logvsl);
	fatal(ixclose, &fd);

	// fabd-pgid file
	pid_t fabd_pgid;
	snprintf(space + z, sizeof(space) - z, "/fabd/pgid");

	// terminate extant fabd process, if any
	if(g_args->invalidationsz)
	{
		// fabd-pgid file
		fatal(gxopen, space, O_RDONLY, &fd);

		if(fd != -1)
		{
			// read and validate the pgid
			fatal(axread, fd, &fabd_pgid, sizeof(fabd_pgid));

			if(fabd_pgid <= 0)
				failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)fabd_pgid);

			// kill signal
			r = 0;
			fatal(uxkill, -fabd_pgid, 15, &r);

			// wait for it to terminate ; signal from fabw will interrupt sleep
			while(r == 0)
			{
				sleep(1);
				fatal(uxkill, -fabd_pgid, 0, &r);
			}
		}
	}

	fatal(ixclose, &fd);
	fatal(gxopen, space, O_RDONLY, &fd);

	r = -1;
	if(fd != -1)
	{
		// read and validate the pgid
		fatal(axread, fd, &fabd_pgid, sizeof(fabd_pgid));

		if(fabd_pgid <= 0)
			failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)fabd_pgid);

		// existence check
		fatal(uxkill, -fabd_pgid, 0, &r);

		if(r == 0)
		{
			// open and read fabd-cred file
			snprintf(space + z, sizeof(space) - z, "/fabd/cred");
			fatal(ixclose, &fd);
			fatal(xopen, space, O_RDONLY, &fd);
			typeof(g_params.ruid) fabd_ruid;
			typeof(g_params.rgid) fabd_rgid;
			fatal(axreadv, fd, (struct iovec[]) {{ .iov_base = &fabd_ruid, .iov_len = sizeof(fabd_ruid) }, { .iov_base = &fabd_rgid, .iov_len = sizeof(fabd_rgid) }}, 2);

			// assert credentials equality
			if(fabd_ruid != g_params.ruid || fabd_rgid != g_params.rgid)
				fatal(fabdcred_throw, fabd_ruid, fabd_rgid);
		}
	}

	// ensure fresh results
	snprintf(space + z, sizeof(space) - z, "/fabd/exit");
	fatal(uxunlink, space, 0);

	if(r)
	{
		// fabd is not running
		fatal(xfork, &fabd_pgid);
		if(fabd_pgid == 0)
		{
			// fabd-pgid file
			snprintf(space + z, sizeof(space) - z, "/fabd/pgid");
			fatal(ixclose, &fd);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
			fatal(axwrite, fd, (pid_t[]) { getpid() }, sizeof(pid_t));
			fatal(ixclose, &fd);

			// fabd-lock file
			snprintf(space + z, sizeof(space) - z, "/fabd/lock");
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &lockfd);
			fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

			// fabd-cred file
			snprintf(space + z, sizeof(space) - z, "/fabd/cred");
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
			fatal(axwritev, fd, (struct iovec[]) {{ .iov_base = &g_params.ruid, .iov_len = sizeof(g_params.ruid) }, { .iov_base = &g_params.rgid, .iov_len = sizeof(g_params.rgid) }}, 2);
			fatal(ixclose, &fd);

			// files done : reassume user identity
			fatal(identity_assume_user);

			// new process group
			fatal(xsetpgid, 0, 0);

			// fabd args
			z = snprintf(space, sizeof(space), "%u", canhash);

#if DEVEL
			snprintf(space2, sizeof(space2), "%s/../fabw/fabw.devel", g_params.exedir);
			execl(space2, space2, space, (void*)0);
#else
			execlp("fabw", "fabw", space, (void*)0);
#endif

			tfail(perrtab_SYS, errno);
		}
	}
	else
	{
		// awaken
		fatal(xkill, -fabd_pgid, 1);
	}
	
	// wait to hear back from fabd
	pause();

	// check fabd-exit file
	snprintf(space + z, sizeof(space) - z, "/fabd/exit");
	fatal(ixclose, &fd);
	fatal(gxopen, space, O_RDONLY, &fd);

	if(fd == 0)
	{
		fatal(axread, fd, &fabd_exit, sizeof(fabd_exit));
		if(WTERMSIG(fabd_exit))
		{
			fail(FAB_FABDFAIL);
		}
	}
	else
	{
		// in this case, fabd should still be running
		fatal(xkill, -fabd_pgid, 0);
	}

finally:
	fatal(ixclose, &fd);
	fatal(ixclose, &lockfd);

	if(XAPI_ERRCODE == FAB_FABDFAIL)
	{
		if(fabd_exit)
		{
			XAPI_INFOF("status", "%d", WEXITSTATUS(fabd_exit));
			XAPI_INFOF("signal", "%d", WTERMSIG(fabd_exit));
		}
	}

	if(XAPI_UNWINDING)
	{
#if DEBUG || DEVEL
		if(mode_backtrace == MODE_BACKTRACE_FULL)
			tracesz = xapi_trace_full(space, sizeof(space));
		else
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));

		logw(L_RED, space, tracesz);
	}
	else
	{
		logs(L_INFO, "exiting with status : 0");
	}

	log_teardown();
	memblk_free(mb);
coda;
}



