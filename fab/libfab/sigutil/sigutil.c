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

#include "types.h"

#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "xlinux/xwait.h"
#include "narrator.h"

#include "sigutil.h"
#include "ipc.h"
#include "probes.h"

#include "macros.h"
#include "zbuffer.h"

#define SIGUTIL_HANDLER_DEBUG 0

bool APIDATA g_sigterm;

static void term_handler(int signum, siginfo_t *info, void *ctx)
{
  g_sigterm = true;

  sigrx_probe(info);
}

static void intr_handler(int signum, siginfo_t * info, void * ctx)
{
  sigrx_probe(info);
}

void API sigutil_install_handlers()
{
  int x;
  struct sigaction intr_act;
  struct sigaction term_act;

  memset(&intr_act, 0, sizeof(intr_act));
  intr_act.sa_flags = SA_SIGINFO;
  intr_act.sa_sigaction = intr_handler;

  memset(&term_act, 0, sizeof(term_act));
  term_act.sa_flags = SA_SIGINFO;
  term_act.sa_sigaction = term_handler;

  // low signals
  for(x = 1; x < SIGSYS; x++)
  {
    if(x == SIGKILL || x == SIGSTOP || x == SIGABRT) { }
    else if(x == SIGINT || x == SIGTERM || x == SIGQUIT) {
      xsigaction(x, &term_act, 0);
    } else {
      xsigaction(x, &intr_act, 0);
    }
  }

  // realtime signals
  for(x = SIGRTMIN; x <= SIGRTMAX; x++)
  {
    if(x == SIGABRT) {
      continue;
    }
    xsigaction(x, &intr_act, 0);
  }
}

void API sigutil_wait(const sigset_t * sigs, siginfo_t * info)
{
  siginfo_t linfo;
  int r;

  if(!info)
    info = &linfo;

  // retry EINTR
  r = -1;
  while(r == -1) {
    r = uxsigwaitinfo(sigs, info);
  }

/* sigwaitinfo does *not* cause the signal handler to be invoked */
/* TODO : move this waitpid out into callers if any */
  if(info->si_signo == SIGCHLD)
  {
    // sigwaitinfo returns only part of the child status
    xwaitpid(info->si_pid, &info->si_status, 0);
  }

  sigrx_probe(info);
}

void API sigutil_timedwait(int * restrict errp, const sigset_t * restrict sigs, siginfo_t * restrict info, const struct timespec * restrict timeout)
{
  int err;

  err = uxsigtimedwait(sigs, info, timeout);

  if(err == 0) {
    sigrx_probe(info);
  } else {
    memset(info, 0, sizeof(*info));
  }

  *errp = err;
}

void API sigutil_exchange(int sig, pid_t pid, const sigset_t * sigs, siginfo_t * info)
{
  // signal target
  xkill(pid, sig);

  // receive a signal in sigs
  sigutil_wait(sigs, info);
}

void API sigutil_kill(pid_t pid, int signo)
{
  sigtx_probe(signo, pid, 0);
  xkill(pid, signo);
}

int API sigutil_uxkill(pid_t pid, int signo)
{
  sigtx_probe(signo, pid, 0);

  return uxkill(pid, signo);
}

void API sigutil_tgkill(pid_t pid, pid_t tid, int signo)
{
  sigtx_probe(signo, pid, tid);

  xtgkill(pid, tid, signo);
}

int API sigutil_uxtgkill(pid_t pid, pid_t tid, int signo)
{
  sigtx_probe(signo, pid, tid);

  return uxtgkill(pid, tid, signo);
}

int API sigutil_uxrt_sigqueueinfo(pid_t pid, int signo, union sigval value)
{
  sigtx_probe(signo, pid, 0);

  siginfo_t info;
  memset(&info, 0, sizeof(info));
  info.si_code = -10;
  info.si_pid = getpid();
  info.si_uid = getuid();
  info.si_value = value;

  return uxrt_sigqueueinfo(pid, signo, &info);
}

int API sigutil_uxrt_tgsigqueueinfo(pid_t pid, pid_t tid, int signo, union sigval value)
{
  sigtx_probe(signo, pid, tid);

  siginfo_t info;
  memset(&info, 0, sizeof(info));
  info.si_code = -10;
  info.si_pid = getpid();
  info.si_uid = getuid();
  info.si_value = value;

  return uxrt_tgsigqueueinfo(pid, tid, signo, &info);
}
