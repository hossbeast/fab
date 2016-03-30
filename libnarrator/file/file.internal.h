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

#ifndef _NARRATE_FILE_INTERNAL_H
#define _NARRATE_FILE_INTERNAL_H

/*

MODULE
 file

SUMMARY
 narrator which writes to a file descriptor

*/

#include <sys/types.h>

#include "xapi.h"

typedef struct narrator_file
{
  int fd;           // file descriptor
} narrator_file;

#define restrict __restrict

/// file
//
// SUMMARY
//  write to the narrator
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

xapi file_sayw(narrator_file * const restrict n, char * const restrict b, size_t l)
	__attribute__((nonnull));

/// file_mark
//
// SUMMARY
//  get a mark indicating the current position
//
// PARAMETERS
//  n    - file narrator
//  mark - (returns) mark indicating current position, or -1 if marks are not supported
//
// REMARKS
//  not supported for all file types
//
xapi file_mark(narrator_file * const restrict n, size_t * const restrict mark)
  __attribute__((nonnull));

/// file_first
//
// SUMMARY
//
const char * file_first(narrator_file * const restrict n)
  __attribute__((nonnull));

/// narrator_free
//
// SUMMARY
//  free a file narrator with free semantics
//
void file_free(narrator_file * restrict n);

#undef restrict
#endif
