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
#include <sys/wait.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xstring.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "logger.h"

#include "internal.h"
#include "sigbank.internal.h"
#include "identity.internal.h"
#include "errtab/FAB.errtab.h"
#include "logging.internal.h"
#include "ipc.internal.h"

#include "macros.h"

#define restrict __restrict

//
// static
//

static sigset_t empty_set;
static sigset_t app_set;
static char * program;
static siginfo_t info;

static void signal_handler(int signum, siginfo_t * _info, void * ctx)
{
  memcpy(&info, _info, sizeof(info));
}

//
// api
//

API xapi sigbank_setup(const char * restrict name)
{
  enter;

  int x;

  fatal(ixstrdup, &program, name);

  sigemptyset(&empty_set);
  sigemptyset(&app_set);
  sigaddset(&app_set, FABIPC_SIGSYN);
  sigaddset(&app_set, FABIPC_SIGACK);
  sigaddset(&app_set, FABIPC_SIGEND);
  sigaddset(&app_set, SIGINT);
  sigaddset(&app_set, SIGQUIT);
  sigaddset(&app_set, SIGTERM);

  fatal(xsigprocmask, SIG_SETMASK, &app_set, 0);

  struct sigaction action = {
      .sa_sigaction = signal_handler
    , .sa_flags = SA_SIGINFO
  };

  // default disposition for all signals
  for(x = 1; x < SIGUNUSED; x++)
  {
    if(sigismember(&app_set, x))
    {
      fatal(xsigaction, x, &action, 0);
    }
    else if(x != SIGKILL && x != SIGSTOP)
    {
      fatal(xsignal, x, SIG_DFL);
    }
  }

  for(x = SIGRTMIN; x <= SIGRTMAX; x++)
  {
    if(sigismember(&app_set, x))
    {
      fatal(xsigaction, x, &action, 0);
    }
    else
    {
      fatal(xsignal, x, SIG_DFL);
    }
  }

  finally : coda;
}

API void sigbank_teardown()
{
  wfree(program);
}

API xapi sigbank_receive(int * const restrict rsig, pid_t * const restrict rpid)
{
  enter;

  int token;

  fatal(uxsigsuspend, &empty_set);

#if DEBUG || DEVEL
  fatal(log_start, L_FAB | L_IPC, &token);
  logf(0, "signal %s { signo : %d, sender : %ld"
    , FABIPC_SIGNAME(info.si_signo)
    , info.si_signo
    , (long)info.si_pid
  );

  if(info.si_signo == SIGCHLD)
  {
    if(WIFEXITED(info.si_status))
      logf(0, ", status : %d", WEXITSTATUS(info.si_status));

    if(WIFSIGNALED(info.si_status))
      logf(0, ", signal : %d", WTERMSIG(info.si_status));
  }
  logf(0, " }");
  fatal(log_finish, &token);
#endif

  if(rsig)
    *rsig = info.si_signo;
  if(rpid)
    *rpid = info.si_pid;

finally:
  fatal(log_finish, &token);
coda;
}
 
API xapi sigbank_exchange(int sig, pid_t pid, int * const restrict rsig, pid_t * const restrict rpid)
{
  enter;

  // assume fabsys identity
  fatal(identity_assume_fabsys);

  // send signal
  fatal(xkill, pid, sig);

  // suspend, receive signal
  fatal(sigbank_receive, rsig, rpid);

  // reassume user identity
  fatal(identity_assume_user);

  finally : coda;
}

API xapi sigbank_assert(int expsig, int actsig, pid_t exppid, pid_t actpid)
{
  enter;

  if((exppid && (exppid != actpid)) || (expsig && (expsig != actsig)))
  {
    xapi_fail_intent();
    if(expsig)
      xapi_info_adds("expected signal", FABIPC_SIGNAME(expsig));
    xapi_info_adds("actual signal", FABIPC_SIGNAME(actsig));

    if(exppid)
      xapi_info_addf("expected pid", "%lu", (long)exppid);
    xapi_info_addf("actual pid", "%lu", (long)actpid);

    fail(FAB_BADIPC);
  }

  finally : coda;
}
