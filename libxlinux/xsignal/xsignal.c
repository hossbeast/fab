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

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "types.h"

#include "xsignal/xsignal.h"
#include "sigtable.h"

void API xkill(pid_t pid, int sig)
{
  RUNTIME_ASSERT(kill(pid, sig) == 0);
}

int API uxkill(pid_t pid, int sig)
{
  int r;

  r = kill(pid, sig);
  RUNTIME_ASSERT(r == 0 || errno == ESRCH);

  return r;
}

void API xtgkill(pid_t pid, pid_t tid, int sig)
{
  RUNTIME_ASSERT(syscall(SYS_tgkill, pid, tid, sig) == 0);
}

int API uxtgkill(pid_t pid, pid_t tid, int sig)
{
  int r;

  if(tid == 0) {
    return 0;
  }

  r = syscall(SYS_tgkill, pid, tid, sig);
  RUNTIME_ASSERT(r == 0 || errno == ESRCH);

  return r;
}

void API xsigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
{
  RUNTIME_ASSERT(sigaction(signum, act, oldact) == 0);
}

void API xsigprocmask(int how, const sigset_t * set, sigset_t * oldset)
{
  RUNTIME_ASSERT(sigprocmask(how, set, oldset) == 0);
}

void API uxsigsuspend(const sigset_t * mask)
{
  int r;

  r = sigsuspend(mask);
  RUNTIME_ASSERT(r == 0 || errno == EINTR);
}

void API xsigwaitinfo(const sigset_t * mask, siginfo_t * info)
{
  RUNTIME_ASSERT(sigwaitinfo(mask, info) == 0);
}

int API uxsigwaitinfo(const sigset_t * mask, siginfo_t * info)
{
  int r;

  r = sigwaitinfo(mask, info);
  RUNTIME_ASSERT(r == 0 || errno == EINTR);

  return r;
}

int API uxsigtimedwait(const sigset_t * restrict set, siginfo_t * restrict info, const struct timespec * restrict timeout)
{
  int r;

  if((r = sigtimedwait(set, info, timeout)) == -1)
  {
    r = errno;
    RUNTIME_ASSERT(r == EAGAIN || r == EINTR);
  }
  else
  {
    r = 0;
  }

  return r;
}

void API xsignal(int signum, sighandler_t handler)
{
  RUNTIME_ASSERT(signal(signum, handler) != SIG_ERR);
}

int API uxrt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *info)
{
  int r;

  r = syscall(SYS_rt_sigqueueinfo, tgid, sig, info);
  RUNTIME_ASSERT(r == 0 || errno == ESRCH);

  return r;
}

int API uxrt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *info)
{
  int r;

  r = syscall(SYS_rt_tgsigqueueinfo, tgid, tid, sig, info);
  RUNTIME_ASSERT(r == 0 || errno == ESRCH);

  return r;
}

const char * API signame(int signo)
{
  if(signo < 0 || signo > sigtable_max) {
    return 0;
  }

  return sigtable[signo];
}
