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

#ifndef _NARRATOR_FIXED_H
#define _NARRATOR_FIXED_H

/*

MODULE
 narrator/fixed

SUMMARY
 narrator that writes to a fixed-size buffer, discarding overflow

REMARKS
 a fixed narrator requires no dynamic allocation, and passing a fixed narrator to the narrator free
 apis is a no-op

*/

typedef struct narrator_fixed_storage
{
  uint8_t opaque;
  char *  s;      // pointer to a buffer to write to
  size_t  l;      // position
  size_t  a;      // buffer size
} narrator_fixed_storage;

#define restrict __restrict

/// narrator_fixed_init
//
// SUMMARY
//  initialize a fixed narrator
//
// PARAMETERS
//  fixed - storage for the fixed narrator
//
// RETURNS
//  fixed size narrator suitable for passing to other narrator apis
//
narrator * narrator_fixed_init(narrator_fixed_storage * restrict fixed)
  __attribute__((nonnull));

/// narrator_fixed_buffer
//
// SUMMARY
//  get a pointer to the underlying buffer, that is, storage->s
//
// PARAMETERS
//  n - fixed narrator
//
const char * narrator_fixed_buffer(narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_fixed_size
//
// SUMMARY
//  get the size in bytes of the data written to the underlying buffer, that is, storage->l
//
// PARAMETERS
//  n - fixed narrator
//
size_t narrator_fixed_size(narrator * const restrict n)
  __attribute__((nonnull));

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

void narrator_fixed_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

void narrator_fixed_sayw(narrator * const restrict n, const char * const restrict b, size_t l)
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
off_t narrator_fixed_seek(narrator * const restrict n, off_t offset, int whence)
  __attribute__((nonnull));

off_t narrator_fixed_reset(narrator * const restrict n)
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
size_t narrator_fixed_read(narrator * restrict n, void * dst, size_t count)
  __attribute__((nonnull));

#undef restrict
#endif
