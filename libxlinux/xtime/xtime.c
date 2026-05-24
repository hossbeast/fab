/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "xtime.h"

void xlocaltime_r(const time_t * timep, struct tm * result)
{
  RUNTIME_ASSERT(localtime_r(timep, result) != 0);
}

void xclock_gettime(clockid_t clk_id, struct timespec * tp)
{
  RUNTIME_ASSERT(clock_gettime(clk_id, tp) == 0);
}


void xclock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain)
{
  RUNTIME_ASSERT(clock_nanosleep(clock_id, flags, request, remain) != 0);
}

int uxclock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain)
{
  int r;

  r = clock_nanosleep(clock_id, flags, request, remain);
  RUNTIME_ASSERT(r == 0 || r == EINTR);

  return r;
}

void xtimer_create(clockid_t clockid, struct sigevent * restrict evp, timer_t * restrict timerid)
{
  int ret;

  ret = timer_create(clockid, evp, timerid);

  RUNTIME_ASSERT(ret == 0);
}

void xtimer_settime(timer_t timerid, int flags, const struct itimerspec * restrict value, struct itimerspec * restrict ovalue)
{
  int ret;

  ret = timer_settime(timerid, flags, value, ovalue);

  RUNTIME_ASSERT(ret == 0);
}
