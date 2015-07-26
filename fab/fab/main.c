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

#define restrict __restrict

//
// static
//

/// launch_if_not_running
//
// SUMMARY
//  launch the specified process for this ipc-hash if it is not already running
//
// PARAMETERS
//  child - one of { faba, fabd }
//  pgid  - (returns) pgid of the process
//
static int launch_if_not_running(int fablockfd, char * const restrict child, pid_t * const restrict child_pgid)
{
  char space[2048];
  int fd = -1;
  pid_t pgid;

  // open the pgid file if it exists
  snprintf(space, sizeof(space), "%s/%s/pgid", g_params.ipcdir, child);
	fatal(uxopen, space, O_RDONLY, &fd);

	int r = -1;
	if(fd != -1)
	{
		// read and validate the pgid
		fatal(axread, fd, &pgid, sizeof(pgid));

		if(pgid <= 0)
			failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)pgid);

		// existence check
		fatal(uxkill, -pgid, 0, &r);
	}

  // delete the exit/error files for the child to ensure fresh results
  snprintf(space, sizeof(space), "%s/%s/exit", g_params.ipcdir, child);
	fatal(uxunlink, space, 0);

  snprintf(space, sizeof(space), "%s/%s/error", g_params.ipcdir, child);
	fatal(uxunlink, space, 0);

  // launch child if it is not running
	if(r)
	{
    fatal(identity_assume_fabsys);

		fatal(xfork, &pgid);
		if(pgid == 0)
		{
      // close fab-lock
      fatal(xclose, fablockfd);

			// save pgid in child-pgid file
      snprintf(space, sizeof(space), "%s/%s/pgid", g_params.ipcdir, child);
			fatal(ixclose, &fd);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
			fatal(axwrite, fd, MM(g_params.pid));
			fatal(ixclose, &fd);

			// obtain the child-lock file, keep it open
      snprintf(space, sizeof(space), "%s/%s/lock", g_params.ipcdir, child);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
			fatal(xflock, fd, LOCK_EX | LOCK_NB);

			// new process group
			fatal(xsetpgid, 0, 0);

			// child args
			snprintf(space, sizeof(space), "%u", g_params.canhash);

#if DEVEL
      char space2[2048];
			snprintf(space2, sizeof(space2), "%s/../fabw/fabw.devel", g_params.exedir);
			execl(space2, "fabw.devel", child, space, (void*)0);
#else
			execlp("fabw", "fabw", child, space, (void*)0);
#endif

      // exec failed
			tfail(perrtab_SYS, errno);
		}

    // await ready signal from the child
    fatal(sigreceive, (int[]) { FABSIG_READY, 0 }, 0, 0, 0);
    fatal(identity_assume_user);
	}

  // return pgid
  (*child_pgid) = pgid;

finally:
  fatal(ixclose, &fd);
coda;
}

/// verify_state
//
// SUMMARY
//  verify that the specified process is still running and has not reported any failure
//
// PARAMETERS
//  child - one of { faba, fabd }
//
static int verify_state(char * const restrict child, pid_t pid)
{
  char space[2048];
  int exit;
  int error;
  int fd = -1;

	// check child exit file
  snprintf(space, sizeof(space), "%s/%s/exit", g_params.ipcdir, child);
	fatal(ixclose, &fd);
	fatal(uxopen, space, O_RDONLY, &fd);

	if(fd != -1)
	{
		fatal(axread, fd, MM(exit));
		if((WIFEXITED(exit) && WTERMSIG(exit)) || WIFSIGNALED(exit))
		{
      if(strcmp(child, "fabd") == 0)
        fail(FAB_FABDFAIL);
		}
	}

  // check child error file
  snprintf(space, sizeof(space),"%s/%s/error", g_params.ipcdir, child);
  fatal(ixclose, &fd);
  fatal(uxopen, space, O_RDONLY, &fd);

  if(fd != -1)
  {
    // propagate
    fatal(axread, fd, MM(error));
    if(error)
      fail(error);
  }

  // check that child is still running
  fatal(xkill, pid, 0);

finally:
  // cleanup
  fatal(ixclose, &fd);

	if(XAPI_ERRCODE == FAB_FABDFAIL)
	{
		if(exit)
		{
			if(WIFEXITED(exit))
				XAPI_INFOF("status", "%d", WEXITSTATUS(exit));

			if(WIFSIGNALED(exit))
				XAPI_INFOF("signal", "%d", WTERMSIG(exit));
		}
	}
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

	memblk * mb = 0;
  int mpc = 0;

	struct timespec time_start = {};
	struct timespec time_end = {};

	fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);

	size_t tracesz = 0;

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);
	
	// get user identity
	fatal(identity_assume_user);

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
	fatal(mempolicy_push, memblk_getpolicy(mb), &mpc);

	// parse cmdline arguments into the block
	fatal(args_parse);
	mempolicy_pop(&mpc);

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
	snprintf(g_params.ipcdir, sizeof(g_params.ipcdir), "%s/%u", XQUOTE(FABIPCDIR), g_params.canhash);
	size_t z = snprintf(space, sizeof(space), "%s", g_params.ipcdir);

	// fab directory
	snprintf(space, sizeof(space),"%s/fab", g_params.ipcdir);
	fatal(mkdirp, space, FABIPC_DIR);

	// fab-lock file
	snprintf(space, sizeof(space),"%s/fab/lock", g_params.ipcdir);
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &lockfd);
	fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

	// canonical fabfile symlink
	snprintf(space, sizeof(space),"%s/fabfile", g_params.ipcdir);
	fatal(uxsymlink, g_args->init_fabfile_path->can, space);

	// fab-cwd symlink
	snprintf(space, sizeof(space),"%s/fab/cwd", g_params.ipcdir);
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
	snprintf(space, sizeof(space),"%s/fab/out", g_params.ipcdir);
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
	snprintf(space, sizeof(space),"%s/fab/err", g_params.ipcdir);
	fatal(uxunlink, space, 0);
	fatal(xsymlink, space2, space);

	// fab-pid file
	snprintf(space, sizeof(space),"%s/fab/pid", g_params.ipcdir);
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
	fatal(axwrite, fd, &g_params.pid, sizeof(g_params.pid));
	fatal(ixclose, &fd);

	// fabd directories
	snprintf(space, sizeof(space),"%s/fabd", g_params.ipcdir);
	fatal(mkdirp, space, FABIPC_DIR);

	// open args file for writing
	snprintf(space, sizeof(space),"%s/args", g_params.ipcdir);
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);

	// set the filesize
	fatal(xftruncate, fd, memblk_size(mb));

	// g_args is unusable beyond this point
	args_freeze(mb);

	// write the block to the file
	fatal(memblk_writeto, mb, fd);
	fatal(ixclose, &fd);

	// open logs file for writing
	snprintf(space, sizeof(space),"%s/logs", g_params.ipcdir);
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);

	// set the filesize
	fatal(xftruncate, fd, g_logvsl);

	// write the logs : g_logs is unusable beyond this point
	fatal(axwrite, fd, g_logvs, g_logvsl);
	fatal(ixclose, &fd);

	// ensure fabd can write to my stdout/stderr
	fatal(xfchmod, 1, 0777);
	fatal(xfchmod, 2, 0777);

  // load fabd pgid if necessary
	if(g_args->invalidationsz || /* changed credentials */ 0)
	{
		// open fabd pgid file
    snprintf(space, sizeof(space), "%s/%s/pgid", g_params.ipcdir, "fabd");
		fatal(uxopen, space, O_RDONLY, &fd);

		if(fd != -1)
		{
			// read and validate the pgid
			fatal(axread, fd, &g_params.fabd_pgid, sizeof(g_params.fabd_pgid));
			if(g_params.fabd_pgid <= 0)
				failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)g_params.fabd_pgid);
    }
	}

  // kill existing fabd when -B
	if(g_args->invalidationsz)
  {
    fatal(sigexchange, -g_params.fabd_pgid, 15, (int[]) { FABSIG_DONE, 0 }, 0, 0, 0);
  }

  // otherwise if credentials have changed cause fabd to save the graph to shm and exit
  else if(/* changed credentials */ 0)
  {
    // prepare to recycle
    fatal(sigexchange, -g_params.fabd_pgid, 15, (int[]) { FABSIG_DONE, 0 }, 0, 0, 0);

    // attach to shm

    // terminate
    fatal(sigexchange, -g_params.fabd_pgid, 15, (int[]) { FABSIG_DONE, 0 }, 0, 0, 0);
  }

  // start fabd if its not running
  fatal(launch_if_not_running, lockfd, "fabd", &g_params.fabd_pgid);

  // awaken fabd and await response
  fatal(sigexchange, -g_params.fabd_pgid, FABSIG_START, (int[]) { FABSIG_DONE, 0 }, 0, 0, 0);

  // propagate exit status / error code from fabd
  fatal(verify_state, "fabd", -g_params.fabd_pgid);

finally:
  // cleanup
	fatal(ixclose, &fd);
	fatal(ixclose, &lockfd);
  mempolicy_unwind(&mpc);

#if DEBUG || DEVEL
  XAPI_INFOS("name", "fab");
  XAPI_INFOF("pid", "%ld", (long)getpid());
  XAPI_INFOF("hash", "%u", g_params.canhash);
#endif

	// when failing due to an error propagated from fabd (fabd_error), do not log the
	// stacktrace, because fabd will have already done that
	if(XAPI_UNWINDING && XAPI_ERRCODE != FAB_FABDFAIL)
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

  // module teardown
  params_teardown();
  sigbank_teardown();
	memblk_free(mb);
	log_teardown();
coda;
}
