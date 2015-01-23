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
	if(!o_signum)
		o_signum = signum;

#if 0
	char space[2048];
	char * dst = space;
	size_t sz = sizeof(space);
	size_t z = 0;

	if(signum != SIGCHLD)
	{
		z += znprintf(dst + z, sz - z, "fab[%u] received %d { from pid : %ld", getpid(), signum, (long)info->si_pid);
		if(signum == SIGCHLD)
			z += znprintf(dst + z, sz - z, ", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);

		z += znprintf(dst + z, sz - z, " }\n");
	}
	int __attribute__((unused)) r = write(1, space, z);
#endif
}

static int bp_exec(int * good)
{
	pstring * tmp = 0;

	pstring * stdo = 0;
	pstring * stde = 0;
	int fd = -1;

	struct
	{
		int			num;			// stage number
		struct
		{
			pid_t		pid;
			int			num;		
			int			stdo_fd;
			int			stde_fd;
			char **	prod_type;
			char ** prod_id;
			size_t	prodl;
		} * 		cmds;
		size_t	cmdsl;
		size_t	cmdsa;
	} * stages = 0;
	size_t stagesl = 0;
	size_t stagesa = 0;
	size_t cmdstot = 0;

	int x;
	int y;
	int i;

	struct stat stb;

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
				T->stdo_fd = -1;
				T->stde_fd = -1;
				stages[stagesl - 1].cmdsl++;
				cmdstot++;
			}
		}
	finally : 
		XAPI_INFOS("path", fpath);
	coda;
	};

	// traverse the buildplan directory structure
	fatal(xnftw, tmp->s, fn, 32, 0);

	// sort the stages ascending
	int compar(const void * _A, const void * _B)
	{
		const typeof(*stages) * A = _A;
		const typeof(*stages) * B = _B;

		return A->num - B->num;
	};
	qsort(stages, stagesl, sizeof(*stages), compar);

	// load the build products
	for(x = 0; x < stagesl; x++)
	{
		for(i = 0; i < stages[x].cmdsl; i++)
		{
			fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/prod", g_params.pid, x, i);
			fatal(xopen, tmp->s, O_RDONLY, &fd);

			// get the filesize
			fatal(xfstat, fd, &stb);	

			// read the whole file
			fatal(psgrow, &tmp, stb.st_size);
			fatal(axread, fd, tmp->s, stb.st_size);

			// fields are delimited by null bytes and come in pairs
			int a = -1;
			int b = -1;
			while((a < stb.st_size) && (b < stb.st_size))
			{
				b = a + 1 + strlen(tmp->s + a + 1);
				
				if(b < stb.st_size)
				{
					fatal(xrealloc, &stages[x].cmds[i].prod_type, sizeof(*stages[x].cmds[0].prod_type), stages[x].cmds[i].prodl + 1, stages[x].cmds[i].prodl);
					fatal(ixstrdup, &stages[x].cmds[i].prod_type[stages[x].cmds[i].prodl], tmp->s + a + 1);

					fatal(xrealloc, &stages[x].cmds[i].prod_id, sizeof(*stages[x].cmds[0].prod_id), stages[x].cmds[i].prodl + 1, stages[x].cmds[i].prodl);
					fatal(ixstrdup, &stages[x].cmds[i].prod_id[stages[x].cmds[i].prodl], tmp->s + b + 1);

					stages[x].cmds[i].prodl++;

					a = b + 1 + strlen(tmp->s + b + 1);
				}
			}
		}
	}

	logf(L_BP | L_BPEXEC, "buildplan @ " XQUOTE(FABTMPDIR) "/pid/%d/bp", g_params.pid);

	// execute stages
	for(x = 0; x < stagesl; x++)
	{
		logf(L_BP | L_BPINFO, "stage %2d of %2d executing %3d of %3d", x, stagesl, stages[x].cmdsl, cmdstot);

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

			for(i = j; i < (j + cmdsl); i++)
			{
				// open file for stdout
				fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/stdo", g_params.pid, x, i);
				fatal(ixclose, &stages[x].cmds[i].stdo_fd);
				fatal(xopen_mode, tmp->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &stages[x].cmds[i].stdo_fd);

				// open file for stderr
				fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/stde", g_params.pid, x, i);
				fatal(ixclose, &stages[x].cmds[i].stde_fd);
				fatal(xopen_mode, tmp->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &stages[x].cmds[i].stde_fd);

				// path to the command
				fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/cmd", g_params.pid, x, i);

				// fork off the child
				fatal(xfork, &stages[x].cmds[i].pid);
				if(stages[x].cmds[i].pid == 0)
				{
					// reopen stdin
					fatal(xclose, 0);
					fatal(xopen, "/dev/null", O_RDONLY, 0);

					// save stdout @ 501, then redirect to the file (dup2 calls close)
					fatal(xdup2, 1, 501);
					fatal(xdup2, stages[x].cmds[i].stdo_fd, 1);

					// save stderr @ 502, then redirect to the file (dup2 calls close)
					fatal(xdup2, 2, 502);
					fatal(xdup2, stages[x].cmds[i].stde_fd, 2);

					// exec doesnt return
					execl(tmp->s, tmp->s, (void*)0);
					fail(0);
				}
			}

			// harvest children 
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
				fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/exit", g_params.pid, x, i);
				fatal(ixclose, &fd);
				fatal(xopen_mode, tmp->s, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &fd);
				fatal(axwrite, fd, &r, sizeof(r));

				// not a clean exit
				int bad = 0;
				if((WIFEXITED(r) && WEXITSTATUS(r)) || WIFSIGNALED(r))
				{
					bad = 1;
				}
				else
				{
					// wrote to stderr
					off_t fsiz;
					fatal(xlseek, stages[x].cmds[i].stde_fd, 0, SEEK_END, &fsiz);
					if(fsiz)
					{
						bad = 1;
					}
					else
					{
						good++;
					}
				}

				uint64_t tag = L_BP | L_BPEXEC;
				if(bad)
					tag |= L_ERROR;

				if(log_would(tag))
				{
					for(y = 0; y < stages[x].cmds[i].prodl; y++)
					{
						if(y == 0)
						{
							log_start(tag);
							logf(0, "[%2d,%3d] %s %s", x, i, stages[x].cmds[i].prod_type[y], stages[x].cmds[i].prod_id[y]);

							if(WIFEXITED(r) && WEXITSTATUS(r))
								logf(0, ", exit status : %d", x, i, stages[x].cmds[i].prod_type[y], stages[x].cmds[i].prod_id[y], WEXITSTATUS(r));
							else if(WIFSIGNALED(r))
								logf(0, ", signal : %d", x, i, stages[x].cmds[i].prod_type[y], stages[x].cmds[i].prod_id[y], WTERMSIG(r));

							if(bad)
								logf(0, ", details @ " XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d", g_params.pid, x, i);

							log_finish();
						}
						else
						{
							logf(tag, "%*s %s %s", 8, "", stages[x].cmds[i].prod_type[y], stages[x].cmds[i].prod_id[y]);
						}
					}
				}

				if(bad)
				{
#if 0
					// snarf command
					fatal(ixclose, &fd);
					fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/cmd", g_params.pid, x, i);
					fatal(xopen, tmp->s, O_RDONLY, &fd);
					fatal(xfstat, fd, &stb);
					fatal(psgrow, &tmp, stb.st_size);
					fatal(axread, fd, tmp->s, stb.st_size);

					fatal(axwrite, 1, tmp->s, stb.st_size);

					// snarf stdout
					fatal(xfstat, stages[x].cmds[i].stdo_fd, &stb);
					fatal(psgrow, &tmp, stb.st_size);
					fatal(xlseek, stages[x].cmds[i].stdo_fd, 0, SEEK_SET, 0);
					fatal(axread, stages[x].cmds[i].stdo_fd, tmp->s, stb.st_size);

					fatal(axwrite, 1, tmp->s, stb.st_size);
#endif

					// snarf stderr
					fatal(xfstat, stages[x].cmds[i].stde_fd, &stb);
					size_t lim = 100;
					size_t act = MIN(stb.st_size, lim);

					fatal(psgrow, &tmp, stb.st_size);
					fatal(xlseek, stages[x].cmds[i].stde_fd, 0, SEEK_SET, 0);
					fatal(axread, stages[x].cmds[i].stde_fd, tmp->s, act);
					
					// chomp trailing whitespace
					size_t end = act - 1;
					while(end >= 0 && (tmp->s[end] == ' ' || tmp->s[end] == '\t' || tmp->s[end] == '\r' || tmp->s[end] == '\n'))
						end--;
					end++;

					fatal(axwrite, 1, tmp->s, end);
					fatal(axwrite, 1, "\n", 1);

					if(stb.st_size > act)
					{
						fatal(psprintf, &tmp, " ... %d more\n", stb.st_size - end);
						fatal(axwrite, 1, tmp->s, tmp->l);
					}
				}

				if(bad)
					fail++;
				if((good + fail) == cmdsl)
					break;
			}

			if(fail)
				break;

			// close all the files for that wave
			for(i = j; i < (j + cmdsl); i++)
			{
				fatal(ixclose, &stages[x].cmds[i].stdo_fd);
				fatal(ixclose, &stages[x].cmds[i].stde_fd);
			}
		}

		if(j < stages[x].cmdsl)
			break;
	}

	if(x == stagesl)
		*good = 1;

finally:

	fatal(xclose, fd);

	for(x = 0; x < stagesl; x++)
	{
		for(y = 0; y < stages[x].cmdsl; y++)
		{
			fatal(ixclose, &stages[x].cmds[y].stdo_fd);
			fatal(ixclose, &stages[x].cmds[y].stde_fd);

			free(stages[x].cmds[y].prod_type);
			free(stages[x].cmds[y].prod_id);
		}
		free(stages[x].cmds);
	}
	free(stages);

	psfree(tmp);
	psfree(stdo);
	psfree(stde);
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

#if 0
printf("fab[%ld] started\n", (long)getpid());
#endif

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

	// default logger configuration
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0, L_INFO);

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
		fatal(uxopen, space, O_RDONLY, &fd);

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
	fatal(uxopen, space, O_RDONLY, &fd);

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
		fatal(xkill, -fabd_pgid, FABSIG_START);
	}
	
	// wait to hear back from fabd
	o_signum = 0;
	pause();
	if(o_signum == FABSIG_BPSTART)
	{
		// execute the buildplan
		int good = 0;
		fatal(bp_exec, &good);

		// notify fabd
		if(good)
			fatal(uxkill, -fabd_pgid, FABSIG_BPGOOD, &r);
		else
			fatal(uxkill, -fabd_pgid, FABSIG_BPBAD, &r);

		o_signum = 0;
		pause();
	}

#if 0
	if(o_signum == FABSIG_DONE)
	{
		// no buildplan
	}
	else
	{
		fail(FAB_BADIPC);
	}
#endif

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
		// in this case, fabd should still be running
		fatal(xkill, -fabd_pgid, 0);
	}

	// touch stamp file
	snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/stamp", g_params.pid);
	fatal(ixclose, &fd);
	fatal(uxopen_mode, space, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &fd);
	if(fd != -1)
		fatal(xfutimens, fd, 0);

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

	log_teardown();
	memblk_free(mb);
coda;
}
