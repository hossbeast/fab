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

#ifndef _XLINUX_XFUTEX_H
#define _XLINUX_XFUTEX_H

#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#include "types.h"

struct timespec;

/**
 * wrapper for futex - suppress failure cases { EAGAIN, EINTR, ETIMEDOUT }
 *
 * futex_wordp - must be 4 byte aligned
 */
int uxfutex(
    int32_t * restrict futex_wordp
  , int futex_op
  , int32_t val
  , const struct timespec * restrict timeout
  , int32_t * restrict uaddr2
  , int32_t val3
)
  __attribute__((nonnull(1)));

#endif
