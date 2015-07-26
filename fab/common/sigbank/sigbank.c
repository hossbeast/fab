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

#include "xapi.h"
#include "xlinux.h"
#include "narrate.h"
#include "narrate/fixed.h"

#include "sigbank.h"
#include "identity.h"
#include "params.h"
#include "FAB.errtab.h"

#include "macros.h"

#define restrict __restrict

//
// public
//
volatile int g_signum;
volatile pid_t g_sigpid;

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
  g_sigpid = info->si_pid;

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

  // the set of blocked signals used for receipt is the empty set
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

int sigreceive(int * restrict expsig, int exppid, int * restrict actsig, pid_t * restrict actpid)
{
  char space[256];
  int signum = 0;
  pid_t sigpid = 0;

  int lresponse = 0;
  if(!actsig)
    actsig = &lresponse;

  // suspend execution pending receipt of a signal in the receive set
  fatal(uxsigsuspend, &o_receive_set);

  // re-establish default signal mask
	fatal(xsigprocmask, SIG_SETMASK, &o_default_set, 0);

  // determine whether the signal received is one of those that is blocked by default
  if(sigismember(&o_default_set, g_signum))
  {
    // return the received signal
    signum = g_signum;
    g_signum = 0;
    if(actsig)
      *actsig = signum;

    sigpid = g_sigpid;
    g_sigpid = 0;
    if(actpid)
      *actpid = sigpid;

    // determine whether actual signal is in expected list
    int * e = expsig;
    while(e && *e)
    {
      if(*actsig == *e)
        break;

      e++;
    }

    // actual signal not in expected list
    if((e && !*e) || (exppid && (exppid != g_sigpid)))
      tfail(perrtab_FAB, FAB_BADIPC);
  }
  else
  {
    // the main loop should check for unblocked signals directly in g_signum
  }

finally:
  if(XAPI_ERRTAB == perrtab_FAB && XAPI_ERRCODE == FAB_BADIPC)
  {
    // list of expected signals
    if(expsig)
    {
      narrationw(space, sizeof(space));
      space[0] = 0;

      int * e = expsig;
      while(e && *e)
      {
        fsayf("%d", *e);
        e++;
        if(*e)
          says(", ");
      }


      XAPI_INFOF("expected signal", "{%s}", space);
    }
    XAPI_INFOF("actual signal", "%d", signum);

    if(exppid)
      XAPI_INFOF("expected pid", "%lu", (long)exppid);
    XAPI_INFOF("actual pid", "%lu", (long)sigpid);
  }
coda;
}
 
int sigexchange(pid_t pid, int sig, int * restrict expsig, int exppid, int * restrict actsig, pid_t * restrict actpid)
{
  // assume fabsys identity
  fatal(identity_assume_fabsys);

  // send signal
  fatal(xkill, pid, sig);

  // suspend, receive signal
  fatal(sigreceive, expsig, exppid, actsig, actpid);

  // reassume user identity
  fatal(identity_assume_user);

  finally : coda;
}
