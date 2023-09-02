/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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

void xtimer_create(clockid_t clockid, struct sigevent * restrict evp, timer_t * restrict timerid)
  __attribute__((nonnull));

void xtimer_settime(timer_t timerid, int flags, const struct itimerspec * restrict value, struct itimerspec * restrict ovalue)
  __attribute__((nonnull));
