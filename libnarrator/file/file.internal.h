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

#ifndef _NARRATOR_FILE_INTERNAL_H
#define _NARRATOR_FILE_INTERNAL_H

#include <sys/types.h>

#include "xapi.h"
#include "file.h"

typedef struct narrator_file
{
  int fd;           // file descriptor
} narrator_file;

#define restrict __restrict

/// file_setup
//
// SUMMARY
//  module initialization
//
xapi file_setup(void);

/// file_teardown
//
// SUMMARY
//  module cleanup
//
void file_teardown(void);

/// file_say
//
// SUMMARY
//  write to a file_narrator
//
// PARAMETERS
//  n     - file narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
//

xapi file_vsayf(narrator_file * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi file_sayw(narrator_file * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

/// file_seek
//
// SUMMARY
//  reposition a file narrator
//
// PARAMETERS
//  n      - narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
// REMARKS
//  not supported for all file types
//
xapi file_seek(narrator_file * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/// file_destroy
//
// SUMMARY
//  destroy a file narrator
//
void file_destroy(narrator_file * restrict n);

#undef restrict
#endif
