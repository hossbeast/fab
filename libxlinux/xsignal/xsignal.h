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

#ifndef _XLINUX_XSIGNAL_H
#define _XLINUX_XSIGNAL_H

#include <sys/types.h>
#include <signal.h>

#include "xapi.h"

/// xkill
//
// SUMMARY
//  proxy for kill
//
// PARAMETERS
//  pid - pid
//  sig - signal
//
xapi xkill(pid_t pid, int sig);

/// uxkill
//
// SUMMARY
//  proxy for kill that only fails when errno != ESRCH (no such pid)
//
// PARAMETERS
//  [r] - (returns) the return value from kill
//  pid - pid
//  sig - signal
//
xapi uxkill(int * restrict r, pid_t pid, int sig);

/// xtgkill
//
// SUMMARY
//  proxy for tgkill
//
xapi xtgkill(int tgid, int tid, int sig);

/// xtgkill
//
// SUMMARY
//  proxy for tgkill that only fails when errno != ESRCH (no such pid/thread)
//
xapi uxtgkill(int * r, int tgid, int tid, int sig);

/// xsigaction
//
// SUMMARY
//  xapi proxy for sigaction
//
xapi xsigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
  __attribute__((nonnull(2)));

/// xsigprocmask
//
// SUMMARY
//  xapi proxy for sigprocmask
//
xapi xsigprocmask(int how, const sigset_t * set, sigset_t * oldset);

/// uxsigsuspend
//
// SUMMARY
//  xapi proxy for sigsuspend that only fails when errno != EINTR
//  (it is kind of the whole point ...)
//
xapi uxsigsuspend(const sigset_t * mask)
  __attribute__((nonnull));

/// sigwaitinfo
//
// SUMMARY
//  xapi proxy for sigwaitinfo
//
xapi xsigwaitinfo(const sigset_t * mask, siginfo_t * info)
  __attribute__((nonnull(1)));

/// uxsigwaitinfo
//
// SUMMARY
//  xapi proxy for sigwaitinfo that only fails when errno != EINTR
//
xapi uxsigwaitinfo(int * r, const sigset_t * mask, siginfo_t * info)
  __attribute__((nonnull(2)));

/**
 * xapi wrapper for sigtimedwait that only fails when errno != { EINTR, EAGAIN }
 *
 * err - (returns) zero on success, errno otherwise
 */
xapi uxsigtimedwait(
    int * restrict err
  , const sigset_t * restrict mask
  , siginfo_t * restrict info
  , const struct timespec * restrict timeout
)
  __attribute__((nonnull));

/// xsignal
//
// SUMMARY
//  xapi proxy for signal
//
xapi xsignal(int signum, sighandler_t handler);

xapi uxrt_sigqueueinfo(int * restrict r, pid_t tgid, int sig, siginfo_t *info)
  __attribute__((nonnull));

xapi uxrt_tgsigqueueinfo(int * restrict r, pid_t tgid, pid_t tid, int sig, siginfo_t *info)
  __attribute__((nonnull));

const char * API signame(int signo);

#endif
