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

#ifndef _NARRATOR_FIXED_INTERNAL_H
#define _NARRATOR_FIXED_INTERNAL_H

#include "types.h"

#include "fixed.h"

typedef struct narrator_fixed
{
  char *  s;      // buffer
  size_t  l;      // position
  size_t  a;      // buffer size
  size_t  m;      // maximum position
} narrator_fixed;

/// narrator_fixed_say
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
// RETURNS
//  returns the number of bytes written
//
int fixed_sayvf(narrator_fixed * restrict n, const char * restrict fmt, va_list va)
  __attribute__((nonnull));

int fixed_sayw(narrator_fixed * restrict n, const char * restrict b, size_t l)
  __attribute__((nonnull));

/// narrator_fixed_seek
//
// SUMMARY
//  reposition the narrator to offset according to whence
//
// PARAMETERS
//  n      - fixed narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
off_t fixed_seek(narrator_fixed * restrict n, off_t offset, int whence)
  __attribute__((nonnull));

off_t fixed_reset(narrator_fixed * restrict n)
  __attribute__((nonnull));

/// narrator_fixed_read
//
// SUMMARY
//  copy bytes from the underlying buffer and reposition
//
// PARAMETERS
//  n     - fixed narrator
//  dst   - destination buffer
//  count - number of bytes to copy
//
// RETURNS
//  number of bytes actually copied <= count
//
size_t fixed_read(narrator_fixed * restrict n, void * dst, size_t count)
  __attribute__((nonnull));

#endif
