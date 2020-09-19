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

#ifndef _FAB_LOCKFILE_H
#define _FAB_LOCKFILE_H

#include "xapi.h"
#include "types.h"

/*
 * attempt to obtain an exclusive lock
 *
 * pid  - (returns) zero if the lock was obtained, otherwise pid of lock holder
 * fmt  - printf-style format string for the path to the lockfile
 */
xapi fabipc_lockfile_obtain(pid_t * restrict pid, int * restrict fd, char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2, 3)))
  __attribute__((format(printf, 3, 4)));

/*
 * write the pid of the current process to a lockfile
 *
 * fmt - printf-style format string for the path to the lockfile
 */
xapi fabipc_lockfile_update(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

/*
 * unlink a lockfile
 *
 * fmt - printf-style format string for the path to the lockfile
 */
xapi fabipc_lockfile_release(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

#endif
