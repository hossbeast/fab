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

#ifndef _NARRATE_GROWING_INTERNAL_H
#define _NARRATE_GROWING_INTERNAL_H

/*

MODULE
 growing

SUMMARY
 narrator which writes to a dynamically resizing buffer that grows without bound

*/

#include <sys/types.h>
#include <stdarg.h>

#include "xapi.h"

struct pstring;     // pstring.h

typedef struct narrator_growing
{
  struct pstring * ps;
} narrator_growing;

#define restrict __restrict

/// growing_say
//
// SUMMARY
//  write to the narrator
//
// PARAMETERS
//  n     - growing narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
// 

xapi growing_vsayf(narrator_growing * const restrict n, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

xapi growing_sayw(narrator_growing * const restrict n, char * const restrict b, size_t l)
	__attribute__((nonnull));

/// growing_mark
//
// SUMMARY
//  get a mark indicating the current position
//
// PARAMETERS
//  n    - growing narrator
//  mark - (returns) mark indicating current position
//
xapi growing_mark(narrator_growing * const restrict n, size_t * const restrict mark)
  __attribute__((nonnull));

/// growing_first
//
// SUMMARY
//
const char * growing_first(narrator_growing * const restrict n)
  __attribute__((nonnull));

/// narrator_free
//
// SUMMARY
//  free a file narrator with free semantics
//
void growing_free(narrator_growing * restrict n);

#undef restrict
#endif
