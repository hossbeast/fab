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

#ifndef _XTIME_H
#define _XTIME_H

#include <time.h>

/// xlocaltime_r
//
// SUMMARY
//  proxy for localtime_r
//
void xlocaltime_r(const time_t * restrict timep, struct tm * restrict result)
  __attribute__((nonnull));

/// clock_gettime
//
// SUMMARY
//  xapi proxy for clock_gettime
//
void xclock_gettime(clockid_t clk_id, struct timespec * tp)
  __attribute__((nonnull));

void xclock_nanosleep(clockid_t clock_id, int flags, const struct timespec * restrict request, struct timespec * restrict remain)
  __attribute__((nonnull(3)));

int uxclock_nanosleep(clockid_t clock_id, int flags, const struct timespec * restrict request, struct timespec * restrict remain)
  __attribute__((nonnull(3)));

#endif
