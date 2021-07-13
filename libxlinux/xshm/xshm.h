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

#ifndef _XSHM_H
#define _XSHM_H

#include <sys/shm.h>

#include "types.h"

/// uxshmget
//
// SUMMARY
//  xapi proxy for shmget that only fails when errno != ENOENT
//
int uxshmget(key_t key, size_t size, int shmflg);

/// xshmget
//
// SUMMARY
//  xapi proxy for shmget
//
int xshmget(key_t key, size_t size, int shmflg);

/// xshmctl
//
// SUMMARY
//  xapi proxy for shmctl
//
void xshmctl(int shmid, int cmd, struct shmid_ds * const restrict buf);

/// xshmat
//
// SUMMARY
//  xapi proxy for shmat
//
void * xshmat(int shmid, const void * restrict shmaddr, int shmflg)
  __attribute__((nonnull));

/// xshmdt
//
// SUMMARY
//  xapi proxy for shmdt - free semantics
//
// PARAMETERS
//  shmaddr
//
void xshmdt(const void * restrict shmaddr);

/// ixshmdt
//
// SUMMARY
//  idempotent xapi proxy for shmdt - iwfree semantics
//
// PARAMETERS
//  shmaddr
//
void ixshmdt(void ** shmaddr)
  __attribute__((nonnull));

#endif
