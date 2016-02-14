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

#ifndef _NARRATE_FIXED_INTERNAL_H
#define _NARRATE_FIXED_INTERNAL_H

#include <sys/types.h>
#include "xapi.h"

/// fixed
//
// SUMMARY
//  write to a fixed size buffer, discarding overflow
//
// PARAMETERS
//  dst - buffer to write to
//  sz  - size of dst
//  szo - offset at which to write, and incremented by number of bytes written
//

#define restrict __restrict

xapi fixed_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

xapi fixed_sayw(narrator * const restrict n, char * const restrict b, size_t l)
	__attribute__((nonnull));

xapi fixed_mark(narrator * const restrict n, size_t * const restrict mark)
  __attribute__((nonnull));

#undef restrict
#endif
