/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#define NSEC_PER_SEC (1000 * 1000 * 1000)

#define USEC_PER_NSEC 1000
#define MSEC_PER_USEC 1000

#define MSEC_PER_SEC  1000

#define MSEC_AS_NSEC(x) ((x) * MSEC_PER_USEC * USEC_PER_NSEC)
#define SEC_AS_MSEC(x)  ((x) * MSEC_PER_SEC)

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
