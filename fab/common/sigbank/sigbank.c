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

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "xlinux.h"
#include "xapi.h"

#include "sigbank.h"
#include "params.h"

#include "macros.h"

#define restrict __restrict

//
// public
//
volatile int g_signum;

//
// static
//

static sigset_t o_default_set;
static sigset_t o_receive_set;

#if DEBUG_IPC
static pid_t o_pid;
static char * o_name;
#endif

static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
  g_signum = signum;

#if DEBUG_IPC
	char space[2048];
	char * dst = space;
	size_t sz = sizeof(space);
	size_t z = 0;

	if(signum != SIGCHLD)
	{
		z += znloadf(dst + z, sz - z, "%s[%lu] received %d { from pid : %ld", o_name, (long)o_pid, signum, (long)info->si_pid);

		if(signum == SIGCHLD)
		{
			z += znloadf(dst + z, sz - z, ", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);
		}
		z += znloadf(dst + z, sz - z, " }\n");
	}

	int __attribute__((unused)) r = write(1, space, z);
#endif
}

//
// public
//

int sigbank_init(
#if DEBUG_IPC
    char * name
  , pid_t pid
#endif
)
{
#if DEBUG_IPC
  o_pid = pid;
  fatal(ixstrdup, &o_name, name);

  dprintf(1, "%s[%ld] started\n", o_name, (long)o_pid);
#endif

  int x;

  // the default set of blocked signals consists exclusively of the fab ipc signals
  sigemptyset(&o_default_set);
  for(x = FABSIG_BASE; x < (FABSIG_BASE + FABSIG_SIZE); x++)
    sigaddset(&o_default_set, x);

  // the set of signals used for receipt is the empty set
  sigemptyset(&o_receive_set);

  // establish the default signal mask
	fatal(xsigprocmask, SIG_SETMASK, &o_default_set, 0);

	// handle all signals
	struct sigaction action = {
		  .sa_sigaction = signal_handler
		, .sa_flags = SA_SIGINFO
	};
	for(x = 1; x < SIGUNUSED; x++)
	{
		if(x != SIGKILL && x != SIGSTOP && x != SIGSEGV)
		{
			fatal(xsigaction, x, &action, 0);
		}
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

  finally : coda;
}

void sigbank_teardown()
{
#if DEBUG_IPC
  free(o_name);
#endif
}

int sigreceive(int * const restrict sig)
{
  // suspend execution pending receipt of a signal in the receive set
  fatal(uxsigsuspend, &o_receive_set);

  // re-establish default signal mask
	fatal(xsigprocmask, SIG_SETMASK, &o_default_set, 0);

  // return received signal
  *sig = g_signum;
  g_signum = 0;

  finally : coda;
}
