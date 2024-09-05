/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <inttypes.h>

#include "types.h"
#include "zcat.h"

#define NSEC_PER_SEC (1000ULL * 1000 * 1000)
#define NSEC_PER_MIN (NSEC_PER_SEC * 60)

#define USEC_PER_NSEC 1000ULL
#define MSEC_PER_USEC 1000ULL

#define MSEC_PER_SEC  1000ULL

#define MSEC_AS_NSEC(x) (MSEC_PER_USEC * USEC_PER_NSEC * (x))
#define SEC_AS_MSEC(x)  (MSEC_PER_SEC * (x))
#define SEC_AS_NSEC(x)  (NSEC_PER_SEC * (x))
#define MIN_AS_NSEC(x)  (NSEC_PER_MIN * (x))

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

  while(dst->tv_nsec > (1000 * 1000 * 1000))
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

  return zcatf(dst, sz, "%02"PRIu64".%03"PRIu64".%03"PRIu64".%03"PRIu64"", s, ms, us, ns);
}


static inline size_t timespec_cat(void * restrict dst, size_t sz, struct timespec * restrict interval)
{
  uint64_t ns;

  ns = timespec_to_ns(interval);

  return timespec_ns_cat(dst, sz, ns);
}
