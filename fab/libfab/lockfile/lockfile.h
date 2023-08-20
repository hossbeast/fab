/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/*
 * attempt to obtain an exclusive lock
 *
 * pid  - (returns) zero if the lock was obtained, otherwise pid of lock holder
 * fmt  - printf-style format string for the path to the lockfile
 */
void fabipc_lockfile_obtain(pid_t * restrict pid, int * restrict fd, char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2, 3)))
  __attribute__((format(printf, 3, 4)));

/*
 * write the pid of the current process to a lockfile
 *
 * fmt - printf-style format string for the path to the lockfile
 */
void fabipc_lockfile_update(char * const restrict fmt, ...)
  __attribute__((nonnull(1)))
  __attribute__((format(printf, 1, 2)));

/*
 * unlink a lockfile
 *
 * fmt - printf-style format string for the path to the lockfile
 */
void fabipc_lockfile_release(char * const restrict fmt, ...)
  __attribute__((nonnull(1)))
  __attribute__((format(printf, 1, 2)));
