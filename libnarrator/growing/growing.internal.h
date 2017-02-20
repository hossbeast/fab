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

#ifndef _NARRATOR_GROWING_INTERNAL_H
#define _NARRATOR_GROWING_INTERNAL_H

#include <sys/types.h>
#include <stdarg.h>

#include "xapi.h"
#include "growing.h"

typedef struct narrator_growing
{
  char *  s;    // the buffer
  size_t  l;    // position
  size_t  a;    // allocated size
  size_t  m;    // maximum position
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

xapi growing_sayw(narrator_growing * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

/// growing_seek
//
// SUMMARY
//  reposition the narrator to offset according to whence
//
// PARAMETERS
//  n      - growing narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
xapi growing_seek(narrator_growing * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/// narrator_destroy
//
// SUMMARY
//  destroy a growing narrator
//
void growing_destroy(narrator_growing * const restrict n)
  __attribute__((nonnull));

#undef restrict
#endif
