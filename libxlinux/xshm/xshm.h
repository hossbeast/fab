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

#include <sys/types.h>
#include <sys/shm.h>

#include "xapi.h"

#define restrict __restrict

/// uxshmget
//
// SUMMARY
//  xapi proxy for shmget that only fails when errno != ENOENT
//
xapi uxshmget(key_t key, size_t size, int shmflg, int * const restrict shmid);

/// xshmget
//
// SUMMARY
//  xapi proxy for shmget
//
xapi xshmget(key_t key, size_t size, int shmflg, int * const restrict shmid);

/// xshmctl
//
// SUMMARY
//  xapi proxy for shmctl
//
xapi xshmctl(int shmid, int cmd, struct shmid_ds * const restrict buf);

/// xshmat
//
// SUMMARY
//  xapi proxy for shmat
//
xapi xshmat(int shmid, const void * shmaddr, int shmflg, void ** const restrict addr)
  __attribute__((nonnull(4)));

/// xshmdt
//
// SUMMARY
//  xapi proxy for shmdt - free semantics
//
// PARAMETERS
//  shmaddr
//
xapi xshmdt(const void * shmaddr);

/// ixshmdt
//
// SUMMARY
//  idempotent xapi proxy for shmdt - iwfree semantics
//
// PARAMETERS
//  shmaddr
//
xapi ixshmdt(void ** shmaddr)
  __attribute__((nonnull));

#undef restrict
#endif
