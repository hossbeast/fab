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

#include "pstring.h"

#include "global.h"

#include "macros.h"
#include "memblk.h"
#include "parseint.h"

// signal handling
static int o_signum;
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
	char space[2048];
	char * dst = space;
	size_t sz = sizeof(space);
	size_t z = 0;

	if(!o_signum)
		o_signum = signum;

	z += znprintf(dst + z, sz - z, "fab[%u] received %d { from pid : %ld", getpid(), signum, (long)info->si_pid);
	if(signum == SIGCHLD)
		z += znprintf(dst + z, sz - z, ", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);

	z += znprintf(dst + z, sz - z, " }\n");
	int __attribute__((unused)) r = write(1, space, z);
}

static int bp_exec(int * good)
{
	pstring * tmp = 0;
	int stdo_fd = -1;
	int stde_fd = -1;
	int exit_fd = -1;

	struct
	{
		int			num;			// stage number
		struct
		{
			pid_t		pid;
			int			num;		
			char *	cmd;		// cmd
			char *	stdo;		// out
			char *	stde;		// err
			char *	exit;		// exit
		} * 		cmds;
		size_t	cmdsl;
		size_t	cmdsa;
	} * stages = 0;
	size_t stagesl = 0;
	size_t stagesa = 0;
	size_t cmdstot = 0;

	int x;
	int y;

	// directory containing the buildplan
	fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp", g_params.pid);

	int fn(const char * fpath, const struct stat * sb, int typeflat, struct FTW * ftwbuf)
	{
		if(ftwbuf->level == 1 || ftwbuf->level == 2)
		{
			int num;
			if(parseint(fpath + ftwbuf->base, SCNd32, 0, INT32_MAX, 1, 10, &num, 0))
				failf(FAB_BADTMP, "expected: int32, actual: %s", fpath + ftwbuf->base);

			if(ftwbuf->level == 1)
			{
				if(stagesl == stagesa)
				{
					size_t ns = stagesa ?: 3;
					ns = ns * 2 + ns / 2;
					fatal(xrealloc, &stages, sizeof(*stages), ns, stagesa);
					stagesa = ns;
				}

				stages[stagesl].num = num;
				stagesl++;
			}
			else if(ftwbuf->level == 2)
			{
				if(stages[stagesl - 1].cmdsl == stages[stagesl - 1].cmdsa)
				{
					size_t ns = stages[stagesl - 1].cmdsa ?: 10;
					ns = ns * 2 + ns / 2;
					fatal(xrealloc, &stages[stagesl - 1].cmds, sizeof(*stages[0].cmds), ns, stages[stagesl - 1].cmdsa);
					stages[stagesl - 1].cmdsa = ns;
				}

				typeof(stages[0].cmds[0]) * T = &stages[stagesl - 1].cmds[stages[stagesl - 1].cmdsl];
				T->num = num;
				stages[stagesl - 1].cmdsl++;
				cmdstot++;
			}
		}
	finally : 
		XAPI_INFOS("path", fpath);
	coda;
	};

	// traverse the buildpath directory structure
	fatal(xnftw, tmp->s, fn, 32, 0);

	// sort the stages ascending
	int compar(const void * _A, const void * _B)
	{
		const typeof(*stages) * A = _A;
		const typeof(*stages) * B = _B;

		return A->num - B->num;
	};
	qsort(stages, stagesl, sizeof(*stages), compar);

	// execute stages
	for(x = 0; x < stagesl; x++)
	{
		logf(L_BP | L_BPINFO, "STAGE %d of %d executing %d of %d", x, stagesl, stages[x].cmdsl, cmdstot);

		int step = stages[x].cmdsl;
		if(g_args->concurrency > 0)
			step = g_args->concurrency;

		// execute the whole stage, but honor the concurrency parameter
		int j;
		for(j = 0; j < stages[x].cmdsl; j += step)
		{
			int cmdsl = step;
			if((j + step) > stages[x].cmdsl)
				cmdsl = stages[x].cmdsl - j;

			int i;
			for(i = j; i < (j + cmdsl); i++)
			{
				fatal(ixsprintf, &stages[x].cmds[i].cmd, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/cmd", g_params.pid, x, i);
				fatal(ixsprintf, &stages[x].cmds[i].stdo, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/stdo", g_params.pid, x, i);
				fatal(ixsprintf, &stages[x].cmds[i].stde, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/stde", g_params.pid, x, i);
				fatal(ixsprintf, &stages[x].cmds[i].exit, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/exit", g_params.pid, x, i);

				// open files for stdout/stderr
				fatal(ixclose, &stdo_fd);
				fatal(xopen_mode, stages[x].cmds[i].stdo, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &stdo_fd);
				fatal(ixclose, &stde_fd);
				fatal(xopen_mode, stages[x].cmds[i].stde, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &stde_fd);

				// fork off the child
				fatal(xfork, &stages[x].cmds[i].pid);
				if(stages[x].cmds[i].pid == 0)
				{
					// reopen stdin
					fatal(xclose, 0);
					fatal(xopen, "/dev/null", O_RDONLY, 0);

					// save stdout
					fatal(xdup2, 1, 501);

					// redirect stdout (dup2 calls close(1))
					fatal(xdup2, stdo_fd, 1);

					// save stderr
					fatal(xdup2, 2, 502);

					// redirect stderr
					fatal(xdup2, stde_fd, 2);

					// exec doesnt return
					execl(stages[x].cmds[i].cmd, stages[x].cmds[i].cmd, (void*)0);
					fail(0);
				}
			}

			// wait for each of them to complete
			int good = 0;
			int fail = 0;
			pid_t pid = 0;
			int r = 0;
			while((pid = wait(&r)) != -1)
			{
				for(i = j; i < (j + step) && i < stages[x].cmdsl; i++)
				{
					if(stages[x].cmds[i].pid == pid)
						break;
				}

				// save the exit status
				fatal(ixclose, &exit_fd);
				fatal(xopen_mode, stages[x].cmds[i].exit, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &exit_fd);
				fatal(axwrite, exit_fd, &r, sizeof(r));

				// not a clean exit
				if((WIFEXITED(r) && WEXITSTATUS(r)) || WIFSIGNALED(r))
				{
					fail++;
				}
				else
				{
					// wrote to stderr
					off_t fsiz;
					fatal(xlseek, stde_fd, 0, SEEK_END, &fsiz);
					if(fsiz)
					{
						fail++;
					}
					else
					{
						good++;
					}
				}

				if((good + fail) == cmdsl)
					break;
			}

			if(fail)
				failf(FAB_FMLFAIL, "%d formula(s) failed", fail);
		}
	}

	*good = 1;

finally:
	psfree(tmp);

	for(x = 0; x < stagesl; x++)
	{
		for(y = 0; y < stages[x].cmdsl; y++)
		{
			free(stages[x].cmds[y].cmd);
			free(stages[x].cmds[y].stdo);
			free(stages[x].cmds[y].stde);
			free(stages[x].cmds[y].exit);
		}
		free(stages[x].cmds);
	}
	free(stages);

	fatal(ixclose, &stdo_fd);
	fatal(ixclose, &stde_fd);
	fatal(ixclose, &exit_fd);
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

#if DEBUG || DEVEL
	if(g_args->mode_logtrace == MODE_LOGTRACE_FULL)
		fatal(log_config, ~0, ~0);
	else
		fatal(log_config, ~0, 0);
#else
	fatal(log_config, ~0);
#endif

/*
/FABIPCDIR/<hash>/fabfile				<-- init fabfile path, symlink
/FABIPCDIR/<hash>/args					<-- args, binary
/FABIPCDIR/<hash>/logs					<-- logs, ascii
/FABIPCDIR/<hash>/fab/pid				<-- fab pid, ascii
/FABIPCDIR/<hash>/fab/lock			<-- fab lockfile, empty
/FABIPCDIR/<hash>/fabd/pgid			<-- fabd pgid, ascii
/FABIPCDIR/<hash>/fabd/lock			<-- fabd lockfile, empty
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
	if(o_signum == 41)
	{
		// execute the buildplan
		int good = 0;
		fatal(bp_exec, &good);

		// notify fabd
		if(good)
			fatal(uxkill, -fabd_pgid, 41, &r);
		else
			fatal(uxkill, -fabd_pgid, 42, &r);
	}

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
