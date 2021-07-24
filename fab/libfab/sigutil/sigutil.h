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

#ifndef _SIGUTIL_H
#define _SIGUTIL_H

#include <signal.h>

#include "types.h"

extern bool g_sigterm;

void sigutil_install_handlers(void);

/// sigutil_wait
//
// SUMMARY
//  block until receipt of one of a set of signals
//
// PARAMETERS
//  sigs   - set of signals
//  [info] - returns information about the signal received
//
void sigutil_wait(const sigset_t * restrict sigs, siginfo_t * restrict info)
  __attribute__((nonnull(1)));

/// sigutil_timedwait
//
// SUMMARY
//  block until receipt of one of a set of signals
//
// PARAMETERS
//  err   - (returns) zero on sucess (one of the signals in sigs caught), errno otherwise (one of EINTR, EAGAIN)
//  sigs  - set of signals
//  info  - returns information about the signal received
//
void sigutil_timedwait(int * restrict err, const sigset_t * restrict sigs, siginfo_t * restrict info, const struct timespec * restrict timeout)
  __attribute__((nonnull));

/// sigutil_exchange
//
// SUMMARY
//  send a signal, then block until receipt of one of a set of signals
//
// PARAMETERS
//  sig    - signal to send
//  pid    - process to kill
//  sigs   - set of signals
//  [info] - returns information about the signal received
//
void sigutil_exchange(int sig, pid_t pid, const sigset_t * restrict sigs, siginfo_t * restrict info)
  __attribute__((nonnull(3)));

/// sigutil_kill
//
// SUMMARY
//  call xkill and log under L_IPC
//
void sigutil_kill(pid_t pid, int sig);
int sigutil_uxkill(pid_t pid, int sig);
void sigutil_tgkill(pid_t pid, pid_t tid, int sig);

int sigutil_uxtgkill(pid_t pid, pid_t tid, int signo)
  __attribute__((nonnull));

int sigutil_uxrt_sigqueueinfo(pid_t pid, int signo, union sigval value)
  __attribute__((nonnull));

int sigutil_uxrt_tgsigqueueinfo(pid_t pid, pid_t tid, int signo, union sigval value)
  __attribute__((nonnull));

#endif
