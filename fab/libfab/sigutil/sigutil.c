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
#include <string.h>

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

#define SIGUTIL_HANDLER_DEBUG 0

bool APIDATA g_sigterm;

#if DEBUG || DEVEL || XUNIT
#if SIGUTIL_HANDLER_DEBUG
static void print_signal()
{
  char buf[128];
  size_t z = 0;

  z += znloadf(buf + z, sizeof(buf) - z, "rx %s { signo %d", signame(info->si_signo), info->si_signo);

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
  else if(info->si_signo < SIGRTMIN)
  {
    z += znloadf(buf + z, sizeof(buf) - z, " pid %ld code %d", (long)info->si_pid, info->si_code);
  }
  else
  {
    z += znloadf(buf + z, sizeof(buf) - z, " pid %ld code %d val %lld", (long)info->si_pid, info->si_code, (long long)info->si_value.sival_ptr);
  }
  z += znloadf(buf + z, sizeof(buf) - z, " }");

  printf("SIGNAL HANDLER :::: pid[%5d]/tid[%5d] %s\n", getpid(), gettid(), buf);
}
#endif
#endif

static void term_handler(int signum, siginfo_t *info, void *ctx)
{
  g_sigterm = true;

#if DEBUG || DEVEL || XUNIT
#if SIGUTIL_HANDLER_DEBUG
  print_signal(signum, info, ctx);
#endif
#endif
}

static void intr_handler(int signum, siginfo_t * info, void * ctx)
{
#if DEBUG || DEVEL || XUNIT
#if SIGUTIL_HANDLER_DEBUG
  print_signal(signum, info, ctx);
#endif
#endif
}

xapi API sigutil_install_handlers()
{
  enter;

  struct sigaction intr_act;
  struct sigaction term_act;

  memset(&intr_act, 0, sizeof(intr_act));
  intr_act.sa_flags = SA_SIGINFO;
  intr_act.sa_sigaction = intr_handler;

  memset(&term_act, 0, sizeof(term_act));
  term_act.sa_flags = SA_SIGINFO;
  term_act.sa_sigaction = term_handler;

  fatal(xsigaction, SIGINT, &term_act, 0);
  fatal(xsigaction, SIGTERM, &term_act, 0);
  fatal(xsigaction, SIGQUIT, &term_act, 0);
  fatal(xsigaction, SIGCHLD, &intr_act, 0);
  fatal(xsigaction, SIGUSR1, &intr_act, 0);

  /* SIGRTMIN - carries int-valued signal from the client */
  fatal(xsigaction, SIGRTMIN, &intr_act, 0);

#if 0
  // low signals
  for(x = 1; x < SIGSYS; x++)
  {
    if(x == SIGKILL || x == SIGSTOP) {

    } else if(x == SIGABRT) {
      fatal(xsignal, x, SIG_DFL);
    } else if(x == SIGINT || x == SIGTERM || x == SIGQUIT) {
      fatal(xsigaction, x, &term_act, 0);
    } else {
      fatal(xsigaction, x, &intr_act, 0);
    }
  }

  // realtime signals
  for(x = SIGRTMIN; x <= SIGRTMAX; x++)
  {
    fatal(xsigaction, x, &intr_act, 0);
  }
#endif

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

/* sigwaitinfo does *not* cause the signal handler to be invoked */
/* TODO : move this waitpid out into callers if any */
  if(info->si_signo == SIGCHLD)
  {
    // sigwaitinfo returns only part of the child status
    fatal(xwaitpid, info->si_pid, &info->si_status, 0);
  }

  fatal(sigutil_log, info);

  finally : coda;
}

xapi API sigutil_timedwait(int * restrict err, const sigset_t * restrict sigs, siginfo_t * restrict info, const struct timespec * restrict timeout)
{
  enter;

  fatal(uxsigtimedwait, err, sigs, info, timeout);

  if(*err == 0) {
    fatal(sigutil_log, info);
  } else {
    memset(info, 0, sizeof(*info));
  }

  finally : coda;
}

xapi API sigutil_log(const siginfo_t * info)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  narrator * N;
  fatal(log_start, L_IPC, &N);
  xsayf("rx %s { signo %d", signame(info->si_signo), info->si_signo);

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
  else if(info->si_signo < SIGRTMIN)
  {
    xsayf(" pid %ld", (long)info->si_pid);
  }
  else
  {
    xsayf(" pid %ld value %lld", (long)info->si_pid, (long long)info->si_value.sival_ptr);
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
      xapi_info_pushs("expected signal", signame(expsig));
    xapi_info_pushs("actual signal", signame(actual->si_signo));

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
  logf(L_IPC, "tx %s { signo %d pid %ld }", signame(signo), signo, (long)pid);
#endif

  fatal(xkill, pid, signo);

  finally : coda;
}

xapi API sigutil_uxkill(int * restrict r, pid_t pid, int signo)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld }", signame(signo), signo, (long)pid);
#endif

  fatal(uxkill, r, pid, signo);

  finally : coda;
}

xapi API sigutil_tgkill(pid_t pid, pid_t tid, int signo)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld tid %ld }", signame(signo), signo, (long)pid, (long)tid);
#endif

  fatal(xtgkill, pid, tid, signo);

  finally : coda;
}

xapi API sigutil_uxtgkill(int * restrict r, pid_t pid, pid_t tid, int signo)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld tid %ld }", signame(signo), signo, (long)pid, (long)tid);
#endif

  fatal(uxtgkill, r, pid, tid, signo);

  finally : coda;
}

xapi API sigutil_uxrt_sigqueueinfo(int * restrict r, pid_t pid, int signo, union sigval value)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld val %lld }", signame(signo), signo, (long)pid, (long long)value.sival_ptr);
#endif

  siginfo_t info;
  memset(&info, 0, sizeof(info));
  info.si_code = -10;
  info.si_pid = getpid();
  info.si_uid = getuid();
  info.si_value = value;

  fatal(uxrt_sigqueueinfo, r, pid, signo, &info);

  finally : coda;
}

xapi API sigutil_uxrt_tgsigqueueinfo(int * restrict r, pid_t pid, pid_t tid, int signo, union sigval value)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  logf(L_IPC, "tx %s { signo %d pid %ld tid %ld val %lld }", signame(signo), signo, (long)pid, (long)tid, (long long)value.sival_ptr);
#endif

  siginfo_t info;
  memset(&info, 0, sizeof(info));
  info.si_code = -10;
  info.si_pid = getpid();
  info.si_uid = getuid();
  info.si_value = value;

  fatal(uxrt_tgsigqueueinfo, r, pid, tid, signo, &info);

  finally : coda;
}
