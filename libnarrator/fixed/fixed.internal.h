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

/*

MODULE
 fixed

SUMMARY
 narrator that writes to a fixed size buffer, discarding overflow

*/

#include <sys/types.h>
#include "xapi.h"

typedef struct narrator_fixed
{
  char *  bb;       // buffer
  size_t  bsz;      // allocated size
  size_t  bz;       // 
} narrator_fixed;

#define restrict __restrict

/// fixed_say
//
// SUMMARY
//  write to the narrator
//
// PARAMETERS
//  n     - fixed narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
// 

xapi fixed_vsayf(narrator_fixed * const restrict n, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

xapi fixed_sayw(narrator_fixed * const restrict n, char * const restrict b, size_t l)
	__attribute__((nonnull));

/// fixed_mark
//
// SUMMARY
//  get a mark indicating the current position
//
// PARAMETERS
//  n    - fixed narrator
//  mark - (returns) mark indicating current position
//
xapi fixed_mark(narrator_fixed * const restrict n, size_t * const restrict mark)
  __attribute__((nonnull));

/// fixed_first
//
// SUMMARY
//
const char * fixed_first(narrator_fixed * const restrict n)
  __attribute__((nonnull));

/// fixed_free
//
// SUMMARY
//  free a file narrator with free semantics
//
void fixed_free(narrator_fixed * restrict n);

#undef restrict
#endif
