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
#include <sys/wait.h>

#include "xlinux.h"
#include "xlinux/mempolicy.h"
#include "xapi/SYS.errtab.h"

#include "pstring.h"

#include "global.h"

#include "macros.h"
#include "memblk.h"
#include "parseint.h"
#include "unitstring.h"
#include "sigbank.h"

#ifndef DEBUG_IPC
# define DEBUG_IPC 0
#endif

//
// static
//

/// launch_if_not_running
//
// SUMMARY
//  launnch the specified process for this ipc-hash if it is not already running
//
// PARAMETERS
//  child - one of { faba, fabd }
//  pgid  - (returns) pgid of the process
//
static int launch_if_not_running(char * const restrict child, pid_t * const restrict child_pgid)
{
  char space[2048];
  int fd = -1;
  pid_t pgid;

  // open the pgid file if it exists
  snprintf(space, sizeof(space), "%s/%s/pgid", g_params.ipcdir, child);
	fatal(uxopen, space, O_RDONLY, &fd);

	r = -1;
	if(fd != -1)
	{
		// read and validate the pgid
		fatal(axread, fd, &pgid, sizeof(pgid));

		if(pgid <= 0)
			failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)pgid);

		// existence check
		fatal(uxfabsyskill, -pgid, 0, &r);
	}

  // delete the exit/error files for the child to ensure fresh results
  snprintf(space, sizeof(space), "%s/%s/exit", g_params.ipcdir, child);
	fatal(uxunlink, space, 0);

  snprintf(space, sizeof(space), "%s/%s/error", g_params.ipcdir, child);
	fatal(uxunlink, space, 0);

  // launch child if it is not running
	if(r)
	{
		fatal(xfork, &pgid);
		if(pgid == 0)
		{
			// save pgid in child-pgid file
      snprintf(space, sizeof(space), "%s/%s/pgid", g_params.ipcdir, child);
			fatal(ixclose, &fd);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
			fatal(axwrite, fd, MM(g_params.pid));
			fatal(ixclose, &fd);

			// obtain the child-lock file
      snprintf(space, sizeof(space), "%s/%s/lock", g_params.ipcdir, child);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &lockfd);
			fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

			// new process group
			fatal(xsetpgid, 0, 0);

			// child args
			z = snprintf(space, sizeof(space), "%s %u", child, g_params.canhash);

#if DEVEL
			snprintf(space2, sizeof(space2), "%s/../fabw/fabw.devel", g_params.exedir);
			execl(space2, space2, space, (void*)0);
#else
			execlp("fabw", "fabw", space, (void*)0);
#endif

      // exec failed
			tfail(perrtab_SYS, errno);
		}
	}

  // return pgid
  (*child_pgid) = pgid;

finally:
  fatal(ixclose, &fd);
coda;
}

//
// public
//

int main(int argc, char** argv, char ** envp)
{
	char space[2048];
	char space2[2048];

#if DEBUG || DEVEL
	int mode_backtrace = DEFAULT_MODE_BACKTRACE;
#endif
	int fd = -1;
	int lockfd = -1;
	int r = -1;
	int fabd_exit = 0;
	int fabd_error = 0;
	int faba_exit = 0;
	int faba_error = 0;

	memblk * mb = 0;

	struct timespec time_start = {};
	struct timespec time_end = {};

	fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);

	int x;
	size_t tracesz = 0;

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);
	
	// get user identity
	fatal(identity_assume_user);

	// ability to create world-rw files
	umask(0);

  fatal(sigbank_init
#if DEBUG_IPC
    , "fab",  getpid()
#endif
  );

	unsigned long * auxv = 0;
#if __linux__
	// locate auxiliary vector
	while(*envp)
		envp++;
	envp++;
	auxv = (void*)envp;
#endif

	// initalize logger
	fatal(log_init, auxv);

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
	g_params.canhash = g_args->init_fabfile_path->can_hash;

#if DEBUG || DEVEL
	if(g_args->mode_logtrace == MODE_LOGTRACE_FULL)
		fatal(log_config, ~0, ~0);
	else
		fatal(log_config, ~0, 0);
#else
	fatal(log_config, ~0);
#endif

	// default logger configuration
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0, L_INFO);

	// ipc-dir stem
	snprintf(g_params.ipcstem, sizeof(g_params.ipcstem), "%s/%u", XQUOTE(FABIPCDIR), g_params.canhash);
	size_t z = snprintf(space, sizeof(space), "%s", g_params.ipcstem);

	// fab directory
	snprintf(space + z, sizeof(space) - z, "/fab");
	fatal(mkdirp, space, FABIPC_DIR);

	// fab-lock file
	snprintf(space + z, sizeof(space) - z, "/fab/lock");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &lockfd);
	fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

	// canonical fabfile symlink
	snprintf(space + z, sizeof(space) - z, "/fabfile");
	fatal(uxsymlink, g_args->init_fabfile_path->can, space);

	// fab-cwd symlink
	snprintf(space + z, sizeof(space) - z, "/fab/cwd");
	fatal(uxunlink, space, 0);
	fatal(xsymlink, g_params.cwd, space);

	// fab-out symlink
	snprintf(space2, sizeof(space2), "/proc/%ld/fd/1", (long)g_params.pid);
	ssize_t ss = 0;
	fatal(xreadlink, space2, space2, sizeof(space2), &ss);
	if(ss >= sizeof(space2))
		ss = sizeof(space2) - 1;
	if(ss >= 0)
		space2[ss] = 0;
	snprintf(space + z, sizeof(space) - z, "/fab/out");
	fatal(uxunlink, space, 0);
	fatal(xsymlink, space2, space);

	// fab-err symlink
	snprintf(space2, sizeof(space2), "/proc/%ld/fd/2", (long)g_params.pid);
	ss = 0;
	fatal(xreadlink, space2, space2, sizeof(space2), &ss);
	if(ss >= sizeof(space2))
		ss = sizeof(space2) - 1;
	if(ss >= 0)
		space2[ss] = 0;
	snprintf(space + z, sizeof(space) - z, "/fab/err");
	fatal(uxunlink, space, 0);
	fatal(xsymlink, space2, space);

	// fab-pid file
	snprintf(space + z, sizeof(space) - z, "/fab/pid");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
	fatal(axwrite, fd, &g_params.pid, sizeof(g_params.pid));
	fatal(ixclose, &fd);

	// faba/fabd directories
	snprintf(space + z, sizeof(space) - z, "/faba");
	fatal(mkdirp, space, FABIPC_DIR);
	snprintf(space + z, sizeof(space) - z, "/fabd");
	fatal(mkdirp, space, FABIPC_DIR);

	// open args file for writing
	snprintf(space + z, sizeof(space) - z, "/args");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);

	// set the filesize
	fatal(xftruncate, fd, memblk_size(mb));

	// g_args is unusable beyond this point
	args_freeze(mb);

	// write the block to the file
	fatal(memblk_writeto, mb, fd);
	fatal(ixclose, &fd);

	// open logs file for writing
	snprintf(space + z, sizeof(space) - z, "/logs");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);

	// set the filesize
	fatal(xftruncate, fd, g_logvsl);

	// write the logs : g_logs is unusable beyond this point
	fatal(axwrite, fd, g_logvs, g_logvsl);
	fatal(ixclose, &fd);

	// ensure faba/fabd can write to my stdout
	fatal(xfchmod, 1, 0777);
	fatal(xfchmod, 2, 0777);

  // pgids for faba and fabd
  pid_t faba_pgid;
	pid_t fabd_pgid;

	// if -B terminate fabd process if any
	if(g_args->invalidationsz)
	{
		// open fabd pgid file
    snprintf(space, sizeof(space), "%s/%s/pgid", g_params.ipcdir, "fabd");
		fatal(uxopen, space, O_RDONLY, &fd);

		if(fd != -1)
		{
			// read and validate the pgid
			fatal(axread, fd, &fabd_pgid, sizeof(fabd_pgid));

			if(fabd_pgid <= 0)
				failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)fabd_pgid);

			// kill signal
			r = 0;
			fatal(uxfabsyskill, -fabd_pgid, 15, &r);

			// wait for it to terminate ; signal from fabw will interrupt sleep
			while(r == 0)
			{
				sleep(1);
				fatal(uxfabsyskill, -fabd_pgid, 0, &r);
			}
		}
	}

  // ensure both are running
  fatal(launch_if_not_running, "faba", &faba_pgid);
  fatal(launch_if_not_running, "fabd", &fabd_pgid);

  // assume fabsys identity for signal exchange
  fatal(identity_assume_fabsys);

  // awaken fabd for new command
  fatal(xfabsyskill, -fabd_pgid, FABSIG_START);

  // suspend, receive signal from fabd
  int sig;
  fatal(sigreceive, &sig);

  // resume user identity
  fatal(identity_assume_user);

  if(sig != FABSIG_DONE)
    failf(FAB_BADIPC, "expected signal %d, actual %d", FABSIG_DONE, sig);

	// check fabd-exit file
	snprintf(space + z, sizeof(space) - z, "/fabd/exit");
	fatal(ixclose, &fd);
	fatal(uxopen, space, O_RDONLY, &fd);

	if(fd != -1)
	{
		fatal(axread, fd, &fabd_exit, sizeof(fabd_exit));
		if((WIFEXITED(fabd_exit) && WTERMSIG(fabd_exit)) || WIFSIGNALED(fabd_exit))
		{
			fail(FAB_FABDFAIL);
		}
	}
	else
	{
		// check fab-error file
		snprintf(space + z, sizeof(space) - z, "/fabd/error");
		fatal(ixclose, &fd);
		fatal(uxopen, space, O_RDONLY, &fd);

		if(fd != -1)
		{
			// propagate
			fatal(axread, fd, &fabd_error, sizeof(fabd_error));
			if(fabd_error)
			{
				fail(fabd_error);
			}
		}
		else
		{
			// in this case, fabd should still be running
			fatal(xfabsyskill, -fabd_pgid, 0);
		}
	}

finally:
	fatal(ixclose, &fd);
	fatal(ixclose, &lockfd);

	if(XAPI_ERRCODE == FAB_FABDFAIL)
	{
		if(fabd_exit)
		{
			if(WIFEXITED(fabd_exit))
				XAPI_INFOF("status", "%d", WEXITSTATUS(fabd_exit));

			if(WIFSIGNALED(fabd_exit))
				XAPI_INFOF("signal", "%d", WTERMSIG(fabd_exit));
		}
	}

	if(XAPI_ERRCODE == FAB_FABAFAIL)
	{
		if(faba_exit)
		{
			if(WIFEXITED(faba_exit))
				XAPI_INFOF("status", "%d", WEXITSTATUS(faba_exit));

			if(WIFSIGNALED(faba_exit))
				XAPI_INFOF("signal", "%d", WTERMSIG(faba_exit));
		}
	}

#if DEBUG || DEVEL
  XAPI_INFOF("pid", "%ld", (long)getpid());
#endif

	/*
	** when failing due to an error propagated from fabd (fabd_error), do not log the
	** stacktrace, because fabd will have already done that. Do use it for the exit
	** status though
	*/
	if(XAPI_UNWINDING && !fabd_error)
	{
#if DEBUG || DEVEL
		if(mode_backtrace == MODE_BACKTRACE_FULL)
			tracesz = xapi_trace_full(space, sizeof(space));
		else
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));

		logw(L_RED, space, tracesz);
	}

	// stop the clock
	clock_gettime(CLOCK_MONOTONIC_RAW, &time_end);

	z = elapsed_string_timespec(&time_start, &time_end, space, sizeof(space));
	logf(L_INFO, "elapsed : %.*s", (int)z, space);

  params_teardown();
  sigbank_teardown();
	memblk_free(mb);
	log_teardown();
coda;
}
