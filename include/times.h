/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <inttypes.h>

#include "types.h"
#include "zcat.h"

/* conversion constants */
#define NSEC_PER_SEC (1000UL * 1000UL * 1000UL)
#define NSEC_PER_MIN (1000UL * 1000UL * 1000UL * 60UL)

#define USEC_PER_NSEC (1000UL)
#define MSEC_PER_USEC (1000UL)
#define NSEC_PER_MSEC (1000UL * 1000UL)

#define MSEC_PER_SEC  (1000UL)

/* milliseconds */
#define SEC_AS_MSEC(x)  ((x) * MSEC_PER_SEC)
#define MSEC_AS_MSEC(x) (x)

/* nanoseconds */
#define MIN_AS_NSEC(x)  ((x) * NSEC_PER_MIN)
#define SEC_AS_NSEC(x)  ((x) * NSEC_PER_SEC)
#define MSEC_AS_NSEC(x) ((x) * NSEC_PER_MSEC)
#define NSEC_AS_NSEC(x) (x)

static inline uint64_t timespec_delta(const struct timespec * restrict start, const struct timespec * restrict end)
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

  while(dst->tv_nsec >= (1000 * 1000 * 1000))
  {
    dst->tv_nsec -= (1000 * 1000 * 1000);
    dst->tv_sec += 1;
  }
}

static inline uint64_t timespec_to_ns(const struct timespec * restrict ts)
{
  uint64_t ns;

  ns = 0;
  ns += ts->tv_sec * 1000 * 1000 * 1000;
  ns += ts->tv_nsec;

  return ns;
}

static inline size_t timespec_ns_cat(void * restrict dst, size_t sz, uint64_t ns)
{
  uint64_t us, ms, s;

  s = ns / (1000 * 1000 * 1000);
  ns -= s * (1000 * 1000 * 1000);

  ms = ns / (1000 * 1000);
  ns -= ms * (1000 * 1000);

  us = ns / 1000;
  ns -= us * 1000;

  return zcatf(dst, sz, "%02"PRIu64"s.%03"PRIu64"ms.%03"PRIu64"us.%03"PRIu64"ns", s, ms, us, ns);
}

static inline size_t timespec_cat(void * restrict dst, size_t sz, struct timespec * restrict interval)
{
  uint64_t ns;

  ns = timespec_to_ns(interval);

  return timespec_ns_cat(dst, sz, ns);
}
