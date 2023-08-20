/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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
