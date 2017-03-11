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

#ifndef _FAB_IPC_H
#define _FAB_IPC_H

#include <signal.h>
#include <string.h>

#define FABIPC_SIGSYN   (SIGRTMIN + 0)
#define FABIPC_SIGACK   (SIGRTMIN + 1)
#define FABIPC_SIGEND   (SIGRTMIN + 2)
#define FABIPC_SIGSCH   (SIGRTMIN + 3)
#define FABIPC_SIGINTR  (SIGRTMIN + 4)

#define FABIPC_SIGNAME(x)                     \
    (x) == FABIPC_SIGSYN  ? "FABIPC_SIGSYN"   \
  : (x) == FABIPC_SIGACK  ? "FABIPC_SIGACK"   \
  : (x) == FABIPC_SIGEND  ? "FABIPC_SIGEND"   \
  : (x) == FABIPC_SIGSCH  ? "FABIPC_SIGSCH"   \
  : (x) == FABIPC_SIGINTR ? "FABIPC_SIGINTR"  \
  : strsignal(x)

#define FABIPC_TOKREQ   0x13
#define FABIPC_TOKRES   0x7c

/// FABIPC_DATA
//
// LOCATION
//  {FABIPCDIR,FABTMPDIR}/*   files which need not be executable
//
// DESCRIPTION
//  ugo+rw   world read/write
//
// RATIONALE
//
#define FABIPC_MODE_DATA 0666

/// FABIPC_CODE
//
// LOCATION
//  {FABIPCDIR,FABTMPDIR}/*    files which need to be executable
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//
#define FABIPC_MODE_CODE 0777

/// FABIPC_DIR
//
// LOCATION
//  {FABIPCDIR,FABTMPDIR}/*    directories
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//
#define FABIPC_MODE_DIR  0777

/// ipc_lock_obtain
//
// SUMMARY
//  attempt to obtain an exclusive lock
//
// PARAMETERS
//  pgid  - (returns) zero on success, otherwise pgid of lock holder
//  fmt   - printf-style format string for the path to the lockfile
//
// REMARKS
//  after the call,
//   if *pgid == 0, then the lock was obtained
//   if *pgid > 0, the lock is already held, and *pgid identifies the lock holder
//   if *pgid < 0, the operation should be re-tried
//
xapi ipc_lock_obtain(pid_t * pgid, char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

/// ipc_lock_update
//
// SUMMARY
//  write the pid of the current process to a lockfile
//
// PARAMETERS
//  fmt   - printf-style format string for the path to the lockfile
//
xapi ipc_lock_update(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

/// ipc_lock_release
//
// SUMMARY
//  unlink a lockfile
//
// PARAMETERS
//  fmt   - printf-style format string for the path to the lockfile
//
xapi ipc_lock_release(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

#endif
