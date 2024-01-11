/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct __dirstream;

/*
 * attempt to obtain an exclusive lock on a lockfile
 *
 * If the lockfile was taken,
 *  *pid is -1 and *fd is >= 0
 *
 * If the lockfile was not taken,
 *  *fd is -1 and,
 *  *pid is > 0 if the lockfile could be read
 *  *pid is -1 if the lockfile could not be read
 *
 * path - path to lockfile
 */
void bamipc_lockfile_acquire(int64_t * restrict pid, int * restrict fd, const char * const restrict path)
  __attribute__((nonnull));

void bamipc_lockfile_validate(int64_t * restrict pid, int * restrict fd, const char * const restrict path)
  __attribute__((nonnull));

/* write the pid of the current process to a lockfile */
void bamipc_lockfile_update(int fd);

/* scan the ipcdir for bamd lockfiles */
struct __dirstream *bamipc_lockfile_scan_start(const char *ipcdir_base)
  __attribute__((nonnull));

void bamipc_lockfile_scan_stop(struct __dirstream * restrict dd)
  __attribute__((nonnull));

/* scan the next lockfile
 *
 * hash        - (returns) the hash for the basedir
 * basedir     - basedir written here
 * basedir_sz  - size of basedir
 * basedir_len - (returns) number of bytes written to basedir
 *
 * returns pid > 0 for the lockfile, or 0 when the scan is complete
 */
int64_t bamipc_lockfile_scan_scan(
    struct __dirstream * restrict dd
  , uint64_t * restrict hash
  , char *basedir
  , size_t basedir_sz
  , size_t *basedir_len
)
  __attribute__((nonnull));
