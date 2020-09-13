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

#include "xapi.h"
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
static xapi __attribute__((nonnull)) compose(unit * cfg, double * parts, narrator * const restrict N)
{
  enter;

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

  finally : coda;
}

//
// api
//

xapi API interval_say(time_t seconds, narrator * const restrict N)
{
  enter;

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
  fatal(compose, cfg, parts, N);

  finally : coda;
}

xapi API bytesize_say(size_t bytes, narrator * const restrict N)
{
  enter;

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
  fatal(compose, cfg, parts, N);

  finally : coda;
}

xapi API elapsed_say(const struct timespec * const restrict start, const struct timespec * const restrict end, narrator * const restrict N)
{
  enter;

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
  fatal(compose, cfg, parts, N);

  finally : coda;
}
