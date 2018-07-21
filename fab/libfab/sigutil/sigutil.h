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

#include "xapi.h"
#include "types.h"

/// sigutil_defaults
//
// SUMMARY
//  establish default disposition and mask for all signals
//
xapi sigutil_defaults(void);

/// sigutil_wait
//
// SUMMARY
//  block until receipt of one of a set of signals
//
// PARAMETERS
//  sigs   - set of signals
//  [info] - returns information about the signal received
//
xapi sigutil_wait(const sigset_t * restrict sigs, siginfo_t * restrict info)
  __attribute__((nonnull(1)));

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
xapi sigutil_exchange(int sig, pid_t pid, const sigset_t * restrict sigs, siginfo_t * restrict info)
  __attribute__((nonnull(3)));

/// sigutil_assert
//
// SUMMARY
//  throw an error if a signal info does not match certain constraints
//
// PARAMETERS
//  [expsig] - expected signal
//  [exppid] - expected sender pid
//  actual   - actual signal information
//
// THROWS
//  FAB_BADIPC
//
xapi sigutil_assert(int expsig, pid_t exppid, const siginfo_t * restrict actual)
  __attribute__((nonnull));

/// sigutil_kill
//
// SUMMARY
//  call xkill and log under L_IPC
//
xapi sigutil_kill(pid_t pid, int sig);

/// sigutil_ukill
//
// SUMMARY
//  call uxkill and log under L_IPC
//
xapi sigutil_ukill(pid_t pid, int sig);

#endif
