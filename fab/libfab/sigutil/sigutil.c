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

#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "narrator.h"

#include "internal.h"
#include "sigutil.h"
#include "logging.h"
#include "FAB.errtab.h"
#include "ipc.h"

#include "identity.h"

#define restrict __restrict

static void handler(int signum, siginfo_t * info, void * ctx) { }

API xapi sigutil_defaults()
{
  enter;

  struct sigaction act = (typeof(act)) { .sa_sigaction = handler, .sa_flags = SA_SIGINFO };

  // default disposition for all low signals
  int x;
  for(x = 1; x < SIGSYS; x++)
  {
    if(x != SIGKILL && x != SIGSTOP)
    {
      fatal(xsigaction, x, &act, 0);
    }
  }

  // application signals used as interrupts
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, FABIPC_SIGINTR);

  for(x = SIGRTMIN; x <= SIGRTMAX; x++)
  {
    if(sigismember(&sigs, x))
      fatal(xsigaction, x, &act, 0);
    else
      fatal(xsignal, x, SIG_DFL);
  }

  // all signals blocked
  sigfillset(&sigs);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

  finally : coda;
}

API xapi sigutil_wait(const sigset_t * sigs, siginfo_t * r_info)
{
  enter;

  siginfo_t info;
  int r = -1;

  while(r == -1)
    fatal(uxsigwaitinfo, &r, sigs, &info);

#if DEBUG || DEVEL || XUNIT
  narrator * N;
  fatal(log_start, L_IPC, &N);
  sayf("rx %s { signo : %d, sender : %ld"
    , FABIPC_SIGNAME(info.si_signo)
    , info.si_signo
    , (long)info.si_pid
  );

  if(info.si_signo == SIGCHLD)
  {
    if(WIFEXITED(info.si_status))
      sayf(", status : %d", WEXITSTATUS(info.si_status));

    if(WIFSIGNALED(info.si_status))
      sayf(", signal : %d", WTERMSIG(info.si_status));
  }
  sayf(" }");
  fatal(log_finish);
#endif

  if(r_info)
    memcpy(r_info, &info, sizeof(*r_info));

  finally : coda;
}

API xapi sigutil_exchange(int sig, pid_t pid, const sigset_t * sigs, siginfo_t * info)
{
  enter;

  // assume fabsys identity
  fatal(identity_assume_effective);

  fatal(xkill, pid, sig);

  // receive a signal in sigs
  fatal(sigutil_wait, sigs, info);

  // reassume user identity
  fatal(identity_assume_real);

  finally : coda;
}

API xapi sigutil_assert(int expsig, pid_t exppid, const siginfo_t * actual)
{
  enter;

  if((exppid && (exppid != actual->si_pid)) || (expsig && (expsig != actual->si_signo)))
  {
    if(expsig)
      xapi_info_pushs("expected signal", FABIPC_SIGNAME(expsig));
    xapi_info_pushs("actual signal", FABIPC_SIGNAME(actual->si_signo));

    if(exppid)
      xapi_info_pushf("expected pid", "%lu", (long)exppid);
    xapi_info_pushf("actual pid", "%lu", (long)actual->si_pid);

    fail(FAB_BADIPC);
  }

  finally : coda;
}
