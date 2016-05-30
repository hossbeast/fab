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

#ifndef _XTIME_H
#define _XTIME_H

#include <time.h>

#include "xapi.h"

/// xlocaltime_r
//
// SUMMARY
//  proxy for localtime_r
//
xapi xlocaltime_r(const time_t * timep, struct tm * result);

/// clock_gettime
//
// SUMMARY
//  xapi proxy for clock_gettime
//
xapi xclock_gettime(clockid_t clk_id, struct timespec * tp)
  __attribute__((nonnull));

#endif
