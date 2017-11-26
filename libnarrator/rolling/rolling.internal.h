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

#ifndef _NARRATOR_ROLLING_INTERNAL_H
#define _NARRATOR_ROLLING_INTERNAL_H

#include <fcntl.h>

#include "xapi.h"
#include "types.h"

#include "rolling.h"

typedef struct narrator_rolling
{
  int fd;           // file descriptor
  size_t written;
  uint16_t counter;
  int scanned;

  uint16_t max_files;
  uint32_t threshold;
  mode_t mode;
  char * directory;
  char * name_base;
} narrator_rolling;

/// rolling_say
//
// SUMMARY
//  write to a rolling_narrator
//
// PARAMETERS
//  n     - rolling narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
//

xapi rolling_xsayvf(narrator_rolling * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi rolling_xsayw(narrator_rolling * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

/// rolling_seek
//
// SUMMARY
//  returns ENOTSUPP
//
xapi rolling_xseek(narrator_rolling * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull));

/// rolling_xdestroy
//
// SUMMARY
//  release resources owned by a rolling narrator, does not free n
//
xapi rolling_xdestroy(narrator_rolling * const restrict n)
  __attribute__((nonnull));

xapi rolling_xread(narrator_rolling * restrict n, void * dst, size_t count)
  __attribute__((nonnull));

#endif
