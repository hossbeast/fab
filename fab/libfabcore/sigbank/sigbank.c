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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "logger.h"

#include "internal.h"
#include "sigbank.internal.h"
#include "identity.internal.h"
#include "params.internal.h"
#include "errtab/FAB.errtab.h"
#include "logging.internal.h"
#include "ipc.internal.h"

#include "macros.h"

#define restrict __restrict

//
// static
//

static int      signums[NSIG];
static pid_t    sigpids[NSIG];
static int      sigexists;

static sigset_t app_set;    // the default set of blocked signals consists exclusively of the application signals
static sigset_t empty_set;
static char *   program;

static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
  signums[signum] = 1;
  sigpids[signum] = info->si_pid;
}

#if DEBUG || DEVEL
static void signal_handler_logging(int signum, siginfo_t * info, void * ctx)
{
  enter_nochecks;

  signal_handler(signum, info, ctx);

  xapi __attribute__((unused)) R = 0;

  int token = 0;
  fatal(log_start, L_FABCORE | L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), program);
  off_t off = 0;
  fatal(narrator_seek, log_narrator(), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s signal %s { signo : %d, pid : %ld"
    , MAX(0, 20 - off), ""
    , info->si_signo == FABSIG_SYN ? "FABSIG_SYN" : info->si_signo == FABSIG_ACK ? "FABSIG_ACK" : strsignal(info->si_signo)
    , info->si_signo
    , (long)info->si_pid
  );

  if(signum == SIGCHLD)
  {
    if(WIFEXITED(info->si_status))
      logf(0, ", status : %d", WEXITSTATUS(info->si_status));

    if(WIFSIGNALED(info->si_status))
      logf(0, ", signal : %d", WTERMSIG(info->si_status));
  }
  logf(0, " }");

finally:
  fatal(log_finish, &token);

  if(XAPI_UNWINDING)
    xapi_backtrace_to(1);

conclude(&R);
}
#endif

//
// api
//

API xapi sigbank_setup(const char * restrict name)
{
  enter;

  fatal(ixstrdup, &program, name);

  // construct the application signal set
  sigemptyset(&empty_set);
  sigemptyset(&app_set);

  sigaddset(&app_set, FABSIG_SYN);
  sigaddset(&app_set, FABSIG_ACK);

  // establish the default signal mask
  fatal(xsigprocmask, SIG_SETMASK, &app_set, 0);

  // handle all signals
  struct sigaction action = {
      .sa_sigaction = signal_handler
    , .sa_flags = SA_SIGINFO
  };

#if DEBUG || DEVEL
  if(log_would(L_FABCORE | L_IPC))
//{
    action.sa_sigaction = signal_handler_logging; 
//printf("IPC signal handler\n");
//}
//else
//{
//printf("NORMAL signal handler\n");
//}
#endif

  int x;
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

API void sigbank_teardown()
{
  free(program);
}

API int sigbank_select(int * restrict sig, pid_t * restrict pid)
{
  int found = 0;

  int x;
  for(x = 0; x < sizeof(signums) / sizeof(signums[0]); x++)
  {
    if(signums[x])
    {
      if(++found == 2)
      {
        sigexists = 1;
        return 1;
      }

      *sig = signums[x];
      *pid = sigpids[x];

      signums[x] = 0;
      sigpids[x] = 0;
    }
  }

  sigexists = 0;
  return !!found;
}

API xapi sigbank_receive(int * restrict expsig, int exppid, int * restrict actsig, pid_t * restrict actpid)
{
  enter;

  char space[512];

  int signum = 0;
  pid_t sigpid = 0;

  int lactsig = 0;
  if(!actsig)
    actsig = &lactsig;

  pid_t lactpid = 0;
  if(!actpid)
    actpid = &lactpid;

  // suspend execution pending receipt of a signal in the receive set
  if(!sigexists)
    fatal(uxsigsuspend, &empty_set);

  sigbank_select(actsig, actpid); 

  // determine whether the signal received is one of the application-defined signals
  if(sigismember(&app_set, *actsig))
  {
    // determine whether actual signal is in expected list
    int * e = expsig;
    while(e && *e)
    {
      if(*actsig == *e)
        break;

      e++;
    }

    // actual signal not in expected list
    if((e && !*e) || (exppid && (exppid != *actpid)))
    {
      // list of expected signals
      char * p = space;
      size_t sz = sizeof(space);
      size_t z = 0;

      int * e = expsig;
      while(e && *e)
      {
        if(z)
          z += znloads(p + z, sz - z, ", ");
        z += znloadf(p + z, sz - z, "%d", *e);
        e++;
      }

      xapi_fail_intent();
      if(z)
        xapi_infof("expected signal", "{ %.*s }", (int)z, p);
      xapi_infof("actual signal", "%d", signum);

      if(exppid)
        xapi_infof("expected pid", "%lu", (long)exppid);
      xapi_infof("actual pid", "%lu", (long)sigpid);

      fail(FAB_BADIPC);
    }
  }

  finally : coda;
}
 
API xapi sigbank_exchange(pid_t pid, int sig, int * restrict expsig, int exppid, int * restrict actsig, pid_t * restrict actpid)
{
  enter;

  // assume fabsys identity
  fatal(identity_assume_fabsys);

  // send signal
  fatal(xkill, pid, sig);

  // suspend, receive signal
  fatal(sigbank_receive, expsig, exppid, actsig, actpid);

  // reassume user identity
  fatal(identity_assume_user);

  finally : coda;
}
