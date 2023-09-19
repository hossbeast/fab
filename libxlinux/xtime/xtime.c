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

#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xtime/xtime.h"

void API xlocaltime_r(const time_t * timep, struct tm * result)
{
  RUNTIME_ASSERT(localtime_r(timep, result) != 0);
}

void API xclock_gettime(clockid_t clk_id, struct timespec * tp)
{
  RUNTIME_ASSERT(clock_gettime(clk_id, tp) == 0);
}


void API xclock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain)
{
  RUNTIME_ASSERT(clock_nanosleep(clock_id, flags, request, remain) != 0);
}

int API uxclock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain)
{
  int r;

  r = clock_nanosleep(clock_id, flags, request, remain);
  RUNTIME_ASSERT(r == 0 || r == EINTR);

  return r;
}
