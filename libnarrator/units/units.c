/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "units.h"

#include "macros.h"

//
// static
//

typedef struct
{
  char *    label;  // required ;
  char *    fmt;    // required ;
  double    scale;  // required ; number of next-smaller units which comprise 1 of this unit
} unit;

/// decompose
//
// SUMMARY
//  convert a quantity from its fundamental units into a set of successively larger categories in
//  which each category is sized as a multiple of the preceeding category
//
// PARAMETERS
//  base  - quantity, in fundamental units
//  cfg   -
//  parts -
//
static void __attribute__((nonnull)) decompose(double base, unit * cfg, double * parts)
{
  size_t components = sentinel(cfg);

  int x;
  for(x = components - 1; x >= 0; x--)
  {
    double factor = 1;

    int y;
    for(y = 0; y <= x; y++)
      factor *= cfg[y].scale;

    if(x)
      parts[x] = (uint32_t)(base / factor);
    else
      parts[x] = base / factor;

    base -= parts[x] * factor;
  }
}

/// compose
//
// SUMMARY
//
static void __attribute__((nonnull)) compose(unit * cfg, double * parts, narrator * const restrict N)
{
  size_t components = sentinel(cfg);

  int first = 1;
  int x;
  for(x = components - 1; x >= 0; x--)
  {
    if(parts[x] || cfg[x].fmt)
    {
      // the fundamental unit is always rendered
      if(parts[x] || x == 0)
      {
        if(!first)
          xsays(" ");

        xsayf(cfg[x].fmt, parts[x]);
        xsayf("%s", cfg[x].label);

        first = 0;
      }
    }
  }
}

//
// api
//

void API interval_say(time_t seconds, narrator * const restrict N)
{
  double parts[6];

  unit cfg[] = {
      { label : " sec(s)" , scale : 1    , fmt : "%.0f" }
    , { label : " min(s)" , scale : 60   , fmt : "%.0f" }
    , { label : " hr(s)"  , scale : 60   , fmt : "%.0f" }
    , { label : " day(s)" , scale : 24   , fmt : "%.0f" }
    , { label : " yr(s)"  , scale : 365  , fmt : "%.0f" }
    , { }
  };

  memset(parts, 0, sizeof(*parts));
  decompose(seconds, cfg, parts);
  compose(cfg, parts, N);
}

void API bytesize_say(size_t bytes, narrator * const restrict N)
{
  double parts[6];

  unit cfg[] = {
      { label : " byte(s)" , scale : 1        , fmt : "%.0f" }
    , { label : "K"        , scale : 1 << 10  , fmt : "%.0f" }
    , { label : "M"        , scale : 1 << 10  , fmt : "%.0f" }
    , { label : "G"        , scale : 1 << 10  , fmt : "%.0f" }
    , { }
  };

  memset(parts, 0, sizeof(*parts));
  decompose(bytes, cfg, parts);
  compose(cfg, parts, N);
}

void API elapsed_say(const struct timespec * const restrict start, const struct timespec * const restrict end, narrator * const restrict N)
{
  double parts[6];
  double nsec;

  unit cfg[] = {
      { label : " sec(s)"  , scale : 1000000000  , fmt : "%4.02f" }
    , { label : " min(s)"  , scale : 60          , fmt : "%.0f" }
    , { label : " hr(s)"   , scale : 60          , fmt : "%.0f" }
    , { label : " day(s)"  , scale : 24          , fmt : "%.0f" }
    , { label : " yr(s)"   , scale : 365         , fmt : "%.0f" }
    , { }
  };

  // calculate interval size in nanoseconds
  nsec = end->tv_sec - start->tv_sec;
  if(end->tv_nsec < start->tv_nsec)
  {
    nsec -= 1;
    nsec *= 1000000000;
    nsec += (end->tv_nsec + 1000000000) - start->tv_nsec;
  }
  else
  {
    nsec *= 1000000000;
    nsec += end->tv_nsec - start->tv_nsec;
  }

  memset(parts, 0, sizeof(*parts));
  decompose(nsec, cfg, parts);
  compose(cfg, parts, N);
}
