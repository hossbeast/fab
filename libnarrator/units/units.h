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

#ifndef _NARRATOR_UNITS_H
#define _NARRATOR_UNITS_H

#include <time.h>

struct timespec;
struct narrator;

#define restrict __restrict

/// interval_say
//
// SUMMARY
//  describe an interval, specified in seconds, in terms of years / days / hours / minutes / seconds
//
// PARAMETERS
//  seconds - the interval, in seconds
//  N       - narrator to write to
//
xapi interval_say(time_t seconds, struct narrator * const restrict N)
  __attribute__((nonnull));

/// elapsed_say
//
// SUMMARY
//  describe an interval, specified as timespec start / end, in terms of years / days / hours /
//  minutes / seconds
//
// PARAMETERS
//  start - start of the interval
//  end   - end of the interval
//  N     - narrator to write to
//
xapi elapsed_say(const struct timespec * const restrict start, const struct timespec * const restrict end, struct narrator * const restrict N)
  __attribute__((nonnull));

/// bytesize_say
//
// SUMMARY
//  describe a quantity of bytes in terms of gigabytes / megabytes / kilobytes / bytes
//
// PARAMETERS
//  bytes - number of bytes
//  N     - narrator to write to
// 
xapi bytesize_say(size_t bytes, struct narrator * const restrict N)
  __attribute__((nonnull));

#undef restrict
#endif
