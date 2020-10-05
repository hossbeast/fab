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

#include <time.h>

#include "xapi.h"

#include "stats.h"
#include "logging.h"

#include "timing.h"
#include "macros.h"

struct g_stats g_stats;

//
// public
//

xapi stats_setup()
{
  enter;

  finally : coda;
}

xapi stats_cleanup()
{
  enter;

  finally : coda;
}

xapi stats_report()
{
  enter;

#undef DEF
#define DEF(x, t) do {                                      \
  logf(L_USAGE, "%20s %"PRIu64, #x, (uint64_t)g_stats.x);   \
} while(0);
STATS_TABLE

  finally : coda;
}

void stats_timing_start(struct timespec * restrict starts, uint8_t n)
{
  int x;

  RUNTIME_ASSERT(clock_gettime(CLOCK_MONOTONIC, &starts[0]) == 0);

  for(x = 1; x < n; x++) {
    memcpy(&starts[x], &starts[0], sizeof(*starts));
  }
}

static void timing_stop(struct timespec * restrict start, struct timespec * restrict end, uint64_t * restrict stat)
{
  time_t sec;

  RUNTIME_ASSERT(clock_gettime(CLOCK_MONOTONIC, end) == 0);

  sec = end->tv_sec - start->tv_sec;
  if(sec > 0) {
    if(sec > 1) {
      *stat = (sec - 1) * NSEC_PER_SEC;
    }
    *stat += (NSEC_PER_SEC - start->tv_nsec);
    *stat += end->tv_nsec;
  } else {
    *stat += end->tv_nsec - start->tv_nsec;
  }
}

void stats_timing_stop(struct timespec * restrict start, uint64_t * restrict stat)
{
  struct timespec end = {};

  timing_stop(start, &end, stat);
printf("ns %lu\n", *stat);
}

void stats_timing_mark(struct timespec * restrict start, uint64_t * restrict stat)
{
  struct timespec end = {};

  timing_stop(start, &end, stat);
printf("ns %lu\n", *stat);

  memcpy(start, &end, sizeof(*start));
}
