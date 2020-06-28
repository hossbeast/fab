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

#include <sys/wait.h>

#include "xapi.h"
#include "types.h"

#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "xlinux/xwait.h"
#include "narrator.h"

#include "sigutil.h"
#include "FAB.errtab.h"
#include "ipc.h"
#include "logging.h"

#include "macros.h"
#include "zbuffer.h"

/* for use with FABIPC_SIGINTR only */
static void handler(int signum, siginfo_t * info, void * ctx)
{
#if DEBUG || DEVEL || XUNIT
#if SIGUTIL_HANDLER_DEBUG
  char buf[128];
  size_t z = 0;

  z += znloadf(buf + z, sizeof(buf) - z, "rx %s { signo %d", FABIPC_SIGNAME(info->si_signo), info->si_signo);

  if(info->si_signo == SIGCHLD)
  {
    z += znloadf(buf + z, sizeof(buf) - z, " pid %ld", (long)info->si_pid);

    if(WIFEXITED(info->si_status))
      z += znloadf(buf + z, sizeof(buf) - z, " status %d", WEXITSTATUS(info->si_status));

    if(WIFSIGNALED(info->si_status))
    {
      z += znloadf(buf + z, sizeof(buf) - z, " signal %d", WTERMSIG(info->si_status));
      z += znloadf(buf + z, sizeof(buf) - z, " core %s", WCOREDUMP(info->si_status) ? "yes" : "no");
    }
  }
  else
  {
    z += znloadf(buf + z, sizeof(buf) - z, " pid %ld", (long)info->si_pid);
  }
  z += znloadf(buf + z, sizeof(buf) - z, " }");

  printf("SIGNAL HANDLER :::: pid[%5d]/tid[%5d] %s\n", getpid(), gettid(), buf);
#endif
#endif
}

xapi API sigutil_defaults()
{
  enter;

  struct sigaction act = (typeof(act)) {
      .sa_sigaction = handler
    , .sa_flags = SA_SIGINFO
  };

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
  sigaddset(&sigs, FABIPC_SIGSCH);
  sigaddset(&sigs, SIGCHLD);

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

xapi API sigutil_wait(const sigset_t * sigs, siginfo_t * info)
{
  enter;

  siginfo_t linfo;
  int r;

  if(!info)
    info = &linfo;

  // retry EINTR
  r = -1;
  while(r == -1) {
    fatal(uxsigwaitinfo, &r, sigs, info);
  }

/*

sigwaitinfo does *not* cause the signal handler to be invoked

*/

  if(info->si_signo == SIGCHLD)
  {
    // sigwaitinfo returns only part of the child status
    fatal(xwaitpid, info->si_pid, &info->si_status, 0);
  }

  fatal(sigutil_log, info);

  finally : coda;
}

xapi API sigutil_log(const siginfo_t * info)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  narrator * N;
  fatal(log_start, L_IPC, &N);
  xsayf("rx %s { signo %d", FABIPC_SIGNAME(info->si_signo), info->si_signo);

  if(info->si_signo == SIGCHLD)
  {
    xsayf(" pid %ld", (long)info->si_pid);

    if(WIFEXITED(info->si_status))
      xsayf(" status %d", WEXITSTATUS(info->si_status));

    if(WIFSIGNALED(info->si_status))
    {
      xsayf(" signal %d", WTERMSIG(info->si_status));
      xsayf(" core %s", WCOREDUMP(info->si_status) ? "yes" : "no");
    }
  }
  else
  {
    xsayf(" pid %ld", (long)info->si_pid);
  }
  xsayf(" }");
  fatal(log_finish);
#endif

  finally : coda;
}

xapi API sigutil_exchange(int sig, pid_t pid, const sigset_t * sigs, siginfo_t * info)
{
  enter;

  // signal target
  fatal(xkill, pid, sig);

  // receive a signal in sigs
  fatal(sigutil_wait, sigs, info);

  finally : coda;
}

xapi API sigutil_assert(int expsig, pid_t exppid, const siginfo_t * actual)
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

xapi API sigutil_kill(pid_t pid, int signo)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld }", FABIPC_SIGNAME(signo), signo, (long)pid);
#endif

  fatal(xkill, pid, signo);

  finally : coda;
}

xapi API sigutil_tgkill(pid_t pid, pid_t tid, int signo)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld tid %ld }", FABIPC_SIGNAME(signo), signo, (long)pid, (long)tid);
#endif

  fatal(xtgkill, pid, tid, signo);

  finally : coda;
}
