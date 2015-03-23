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

#ifndef DEBUG_IPC
# define DEBUG_IPC 0
#endif

// signal handling
static int o_signum;
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
	if(!o_signum)
		o_signum = signum;

#if DEBUG_IPC
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

static int bp_exec_stage(int stagesl, int commandsl, int stagex, int * success)
{
	pstring * tmp = 0;
	int fd = -1;

	struct
	{
		int			num;			// stage number
		struct
		{
			pid_t		pid;
			int			num;		
			int			cmd_fd;
			int			stdo_fd;
			int			stde_fd;
			char **	prod_type;
			char ** prod_id;
			size_t	prodl;
		} * 		cmds;
		size_t	cmdsl;
		size_t	cmdsa;
	} stage = { .num = stagex };

	int x;
	int y;
	int i;

	struct stat stb;

	// directory containing the buildplan stage
	fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d", g_params.pid, stagex);

	int fn(const char * fpath, const struct stat * sb, int typeflat, struct FTW * ftwbuf)
	{
		if(ftwbuf->level == 1)
		{
			int num;
			if(parseint(fpath + ftwbuf->base, SCNd32, 0, INT32_MAX, 1, 10, &num, 0))
				failf(FAB_BADTMP, "expected: int32, actual: %s", fpath + ftwbuf->base);

			if(stage.cmdsl == stage.cmdsa)
			{
				size_t ns = stage.cmdsa ?: 10;
				ns = ns * 2 + ns / 2;
				fatal(xrealloc, &stage.cmds, sizeof(*stage.cmds), ns, stage.cmdsa);
				stage.cmdsa = ns;
			}

			typeof(stage.cmds[0]) * T = &stage.cmds[stage.cmdsl];
			T->num = num;
			T->cmd_fd = -1;
			T->stdo_fd = -1;
			T->stde_fd = -1;
			stage.cmdsl++;
		}
	finally : 
		XAPI_INFOS("path", fpath);
	coda;
	};

	// traverse the buildplan stage directory
	fatal(xnftw, tmp->s, fn, 32, 0);

	// load the build products for the stage
	for(i = 0; i < stage.cmdsl; i++)
	{
		fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/prod", g_params.pid, stagex, i);
		fatal(ixclose, &fd);
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
				fatal(xrealloc, &stage.cmds[i].prod_type, sizeof(*stage.cmds[0].prod_type), stage.cmds[i].prodl + 1, stage.cmds[i].prodl);
				fatal(ixstrdup, &stage.cmds[i].prod_type[stage.cmds[i].prodl], tmp->s + a + 1);

				fatal(xrealloc, &stage.cmds[i].prod_id, sizeof(*stage.cmds[0].prod_id), stage.cmds[i].prodl + 1, stage.cmds[i].prodl);
				fatal(ixstrdup, &stage.cmds[i].prod_id[stage.cmds[i].prodl], tmp->s + b + 1);

				stage.cmds[i].prodl++;

				a = b + 1 + strlen(tmp->s + b + 1);
			}
		}
	}

	x = stagex;
	logf(L_BPINFO, "stage %2d of %2d executing %3d of %3d", x, stagesl, stage.cmdsl, commandsl);

	// execute stages
	int step = stage.cmdsl;
	if(g_args->concurrency > 0)
		step = g_args->concurrency;

	// execute the whole stage, but honor the concurrency parameter
	int j;
	for(j = 0; j < stage.cmdsl; j += step)
	{
		int cmdsl = step;
		if((j + step) > stage.cmdsl)
			cmdsl = stage.cmdsl - j;

		for(i = j; i < (j + cmdsl); i++)
		{
			// open file for stdout
			fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/out", g_params.pid, x, i);
			fatal(ixclose, &stage.cmds[i].stdo_fd);
			fatal(xopen_mode, tmp->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, FABIPC_DATA, &stage.cmds[i].stdo_fd);

			// open file for stderr
			fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/err", g_params.pid, x, i);
			fatal(ixclose, &stage.cmds[i].stde_fd);
			fatal(xopen_mode, tmp->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, FABIPC_DATA, &stage.cmds[i].stde_fd);

			// path to the command
			fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/cmd", g_params.pid, x, i);

			// open the file if we will later need to examine it
			if(log_would(L_BPCMD))
			{
				fatal(ixclose, &stage.cmds[i].cmd_fd);
				fatal(xopen, tmp->s, O_RDONLY | O_CLOEXEC, &stage.cmds[i].cmd_fd);
			}

			// fork off the child
			fatal(xfork, &stage.cmds[i].pid);
			if(stage.cmds[i].pid == 0)
			{
				// reopen stdin
				fatal(xclose, 0);
				fatal(xopen, "/dev/null", O_RDONLY, 0);

				// save stdout @ 501, then redirect to the file (dup2 calls close)
				fatal(xdup2, 1, 501);
				fatal(xdup2, stage.cmds[i].stdo_fd, 1);

				// save stderr @ 502, then redirect to the file (dup2 calls close)
				fatal(xdup2, 2, 502);
				fatal(xdup2, stage.cmds[i].stde_fd, 2);

				// irretrievably drop fabsys:fabsys identity
				fatal(xsetresuid, g_params.ruid, g_params.ruid, g_params.ruid);
				fatal(xsetresgid, g_params.rgid, g_params.rgid, g_params.rgid);

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
			for(i = j; i < (j + step) && i < stage.cmdsl; i++)
			{
				if(stage.cmds[i].pid == pid)
					break;
			}

			// save the exit status
			fatal(psprintf, &tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/exit", g_params.pid, x, i);
			fatal(ixclose, &fd);
			fatal(xopen_mode, tmp->s, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
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
				fatal(xlseek, stage.cmds[i].stde_fd, 0, SEEK_END, &fsiz);
				if(fsiz)
				{
					bad = 1;
				}
				else
				{
					good++;
				}
			}

			uint64_t tag = L_BPEXEC;
			if(bad)
				tag |= L_ERROR;

			if(log_would(tag))
			{
				for(y = 0; y < stage.cmds[i].prodl; y++)
				{
					if(y == 0)
					{
						log_start(tag);
						logf(0, "[%2d,%3d] %s %s", x, i, stage.cmds[i].prod_type[y], stage.cmds[i].prod_id[y]);

						if(WIFEXITED(r) && WEXITSTATUS(r))
							logf(0, ", exit status : %d", x, i, stage.cmds[i].prod_type[y], stage.cmds[i].prod_id[y], WEXITSTATUS(r));
						else if(WIFSIGNALED(r))
							logf(0, ", signal : %d", x, i, stage.cmds[i].prod_type[y], stage.cmds[i].prod_id[y], WTERMSIG(r));

						if(bad)
						{
							logf(0, ", details @ %s/bp/%d/%d", g_params.ipcstem, x, i);
						}

						log_finish();
					}
					else
					{
						logf(tag, "%*s %s %s", 8, "", stage.cmds[i].prod_type[y], stage.cmds[i].prod_id[y]);
					}
				}
			}

			if(log_would(L_BPCMD))
			{
				// snarf cmd
				fatal(xfstat, stage.cmds[i].cmd_fd, &stb);
				fatal(psgrow, &tmp, stb.st_size);
				fatal(xlseek, stage.cmds[i].cmd_fd, 0, SEEK_SET, 0);
				fatal(axread, stage.cmds[i].cmd_fd, tmp->s, stb.st_size);
			
				// chomp trailing whitespace
				size_t end = stb.st_size - 1;
				while(end >= 0 && (tmp->s[end] == ' ' || tmp->s[end] == '\t' || tmp->s[end] == '\r' || tmp->s[end] == '\n'))
					end--;
				end++;

				fatal(axwrite, 1, tmp->s, end);
				fatal(axwrite, 1, "\n", 1);
			}

			if(bad)
			{
				// snarf stderr
				fatal(xfstat, stage.cmds[i].stde_fd, &stb);
				size_t lim = 500;
				size_t act = MIN(stb.st_size, lim);

				fatal(psgrow, &tmp, act);
				fatal(xlseek, stage.cmds[i].stde_fd, 0, SEEK_SET, 0);
				fatal(axread, stage.cmds[i].stde_fd, tmp->s, act);
				
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
			fatal(ixclose, &stage.cmds[i].stdo_fd);
			fatal(ixclose, &stage.cmds[i].stde_fd);
		}
	}

	if(j >= stage.cmdsl)
		*success = 1;

finally:
	fatal(xclose, fd);

	for(y = 0; y < stage.cmdsl; y++)
	{
		fatal(ixclose, &stage.cmds[y].cmd_fd);
		fatal(ixclose, &stage.cmds[y].stdo_fd);
		fatal(ixclose, &stage.cmds[y].stde_fd);

		free(stage.cmds[y].prod_type);
		free(stage.cmds[y].prod_id);
	}
	free(stage.cmds);

	psfree(tmp);
coda;
}

static int xfabdkill(pid_t pid, int sig)
{
	fatal(identity_assume_fabsys);
	fatal(xkill, pid, sig);
	fatal(identity_assume_user);

	finally : coda;
}

static int uxfabdkill(pid_t pid, int sig, int * r)
{
	fatal(identity_assume_fabsys);
	fatal(uxkill, pid, sig, r);
	fatal(identity_assume_user);

	finally : coda;
}

int main(int argc, char** argv, char ** envp)
{
	char space[2048];
	char space2[2048];
	pstring * tmp = 0;
	struct stat stb;

	char * buildscript_path = 0;

#if DEBUG || DEVEL
	int mode_backtrace = DEFAULT_MODE_BACKTRACE;
#endif
	int fd = -1;
	int lockfd = -1;
	int r = -1;
	int fabd_exit = 0;
	int fabd_error = 0;

	memblk * mb = 0;

	struct timespec time_start = {};
	struct timespec time_end = {};

	fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);

#if DEBUG_IPC
printf("fab[%ld] started\n", (long)getpid());
#endif

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

	// unblock all signals
	sigset_t all;
	sigset_t none;
	sigfillset(&all);
	sigemptyset(&none);
	fatal(xsigprocmask, SIG_SETMASK, &none, 0);

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
	if(g_args->buildscript_path)
		fatal(ixstrdup, &buildscript_path, g_args->buildscript_path);

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
0666 /FABIPCDIR/<hash>/args						<-- args, binary
0666 /FABIPCDIR/<hash>/logs						<-- logs, ascii
     /FABIPCDIR/<hash>/bp             <-- bp dir, symlink
0666 /FABIPCDIR/<hash>/fab/pid				<-- fab pid, ascii
0666 /FABIPCDIR/<hash>/fab/lock				<-- fab lockfile, empty
     /FABIPCDIR/<hash>/fab/out				<-- fab stdout, symlink
     /FABIPCDIR/<hash>/fab/err				<-- fab stderr, symlink
     /FABIPCDIR/<hash>/fab/cwd				<-- fab cwd, symlink
0666 /FABIPCDIR/<hash>/fabd/pgid			<-- fabd pgid, ascii
0666 /FABIPCDIR/<hash>/fabd/lock			<-- fabd lockfile, empty
0666 /FABIPCDIR/<hash>/fabd/exit			<-- fabd exit status, binary
0666 /FABIPCDIR/<hash>/fabd/error			<-- fabd last error, binary
*/

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

	// fabd directory
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

	// ensure fabd can write to my stdout
	fatal(xfchmod, 1, 0777);
	fatal(xfchmod, 2, 0777);

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
			fatal(uxfabdkill, -fabd_pgid, 15, &r);

			// wait for it to terminate ; signal from fabw will interrupt sleep
			while(r == 0)
			{
				sleep(1);
				fatal(uxfabdkill, -fabd_pgid, 0, &r);
			}
		}
	}

	// fabd-pgid file
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
		fatal(identity_assume_fabsys);
		fatal(uxfabdkill, -fabd_pgid, 0, &r);
		fatal(identity_assume_user);
	}

	// ensure fresh results
	snprintf(space + z, sizeof(space) - z, "/fabd/exit");
	fatal(uxunlink, space, 0);

	snprintf(space + z, sizeof(space) - z, "/fabd/error");
	fatal(uxunlink, space, 0);

	// block signals
	fatal(xsigprocmask, SIG_SETMASK, &all, 0);

	if(r)
	{
		// fabd is not running
		fatal(xfork, &fabd_pgid);
		if(fabd_pgid == 0)
		{
			// fabd-pgid file
			snprintf(space + z, sizeof(space) - z, "/fabd/pgid");
			fatal(ixclose, &fd);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
			fatal(axwrite, fd, (pid_t[]) { getpid() }, sizeof(pid_t));
			fatal(ixclose, &fd);

			// fabd-lock file
			snprintf(space + z, sizeof(space) - z, "/fabd/lock");
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &lockfd);
			fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

			// new process group
			fatal(xsetpgid, 0, 0);

			// fabd args
			z = snprintf(space, sizeof(space), "%u", g_params.canhash);

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
		// awaken fabd
		fatal(xfabdkill, -fabd_pgid, FABSIG_START);
	}

	// perform tasks for fabd
	int stagesl;
	int commandsl;
	for(x = 0; 1; x++)
	{
		// await response signal from fabd
		o_signum = 0;
		sigsuspend(&none);
		fatal(xsigprocmask, SIG_SETMASK, &none, 0);
		
		if(o_signum != FABSIG_BPSTART)
			break;

		if(x == 0)
		{
			// load info about the entire build
			snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/bp/stages", g_params.pid);
			fatal(ixclose, &fd);
			fatal(xopen, space, O_RDONLY, &fd);
			fatal(axread, fd, &stagesl, fd);
			
			snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/bp/commands", g_params.pid);
			fatal(ixclose, &fd);
			fatal(xopen, space, O_RDONLY, &fd);
			fatal(axread, fd, &commandsl, fd);

			logf(L_BPEXEC, "buildplan @ %s/%u/bp", XQUOTE(FABIPCDIR), g_params.canhash);
		}

		// execute the buildplan stage
		int good = 0;
		fatal(bp_exec_stage, stagesl, commandsl, x, &good);

		// notify fabd
		if(good)
		{
			// block signals
			fatal(xsigprocmask, SIG_BLOCK, &all, 0);

			// awaken fabd
			fatal(uxfabdkill, -fabd_pgid, FABSIG_BPGOOD, &r);
		}
		else
		{
			// awaken fabd
			fatal(uxfabdkill, -fabd_pgid, FABSIG_BPBAD, &r);

			// some command failed
			fail(FAB_CMDFAIL);
		}
	}

	if(x)
	{
		// touch stamp file to refresh the expiration on the bp directory
		snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/stamp", g_params.pid);
		fatal(ixclose, &fd);
		fatal(uxopen_mode, space, O_CREAT | O_RDWR, FABIPC_DATA, &fd);
		if(fd != -1)
			fatal(xfutimens, fd, 0);
	}

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
			fatal(xfabdkill, -fabd_pgid, 0);
		}
	}

	// rewrite the buildscript, if any
	if(buildscript_path)
	{
		// open buildscript from the ipc-dir
		snprintf(space + z, sizeof(space) - z, "/bs");
		fatal(ixclose, &fd);
		fatal(xopen, space, O_RDONLY, &fd);

		// get the filesize
		fatal(xfstat, fd, &stb);

		// read the whole file
		fatal(psgrow, &tmp, stb.st_size);
		fatal(axread, fd, tmp->s, stb.st_size);

		// open the dest path
		fatal(ixclose, &fd);
		fatal(xopen_mode, buildscript_path, O_CREAT | O_TRUNC | O_WRONLY, 0755, &fd);		// u+rwx go+rx

		// rewrite
		fatal(axwrite, fd, tmp->s, stb.st_size);
		logf(L_INFO, "wrote %s", buildscript_path);
	}

finally:
	fatal(ixclose, &fd);
	fatal(ixclose, &lockfd);
	free(buildscript_path);
	psfree(tmp);

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

	memblk_free(mb);
	log_teardown();
coda;
}
