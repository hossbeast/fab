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

#ifndef _TIMING_H
#define _TIMING_H

#include "types.h"

#define NSEC_PER_SEC (1000 * 1000 * 1000)

#define USEC_PER_NSEC 1000
#define MSEC_PER_USEC 1000

#define MSEC_PER_SEC  1000

#define MSEC_AS_NSEC(x) ((x) * 1000 * 1000)
#define SEC_AS_MSEC(x) ((x) * MSEC_PER_SEC)

static inline uint64_t timespec_delta(struct timespec * restrict start, struct timespec * restrict end)
{
  uint64_t val;

  val = end->tv_sec - start->tv_sec;
  if(val > 0) {
    if(val > 1) {
      val = (val - 1) * NSEC_PER_SEC;
    }
    val += (NSEC_PER_SEC - start->tv_nsec);
    val += end->tv_nsec;
  } else {
    val = end->tv_nsec - start->tv_nsec;
  }

  return val;
}

static inline void timespec_add(struct timespec * restrict dst, uint64_t nsec)
{
  dst->tv_nsec += nsec;

  while(dst->tv_nsec > (1000 * 1000 * 1000))
  {
    dst->tv_nsec -= (1000 * 1000 * 1000);
    dst->tv_sec += 1;
  }
}

#endif
