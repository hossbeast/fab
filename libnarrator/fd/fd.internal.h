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

#ifndef _NARRATOR_FD_INTERNAL_H
#define _NARRATOR_FD_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "fd.h"

typedef struct narrator_fd
{
  int fd;           // file descriptor
} narrator_fd;

/// fd_setup
//
// SUMMARY
//  module initialization
//
xapi fd_setup(void);

/// fd_cleanup
//
// SUMMARY
//  module cleanup
//
xapi fd_cleanup(void);

/// fd_say
//
// SUMMARY
//  write to a fd_narrator
//
// PARAMETERS
//  n     - fd narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
//
xapi fd_xsayvf(narrator_fd * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

int fd_sayvf(narrator_fd * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi fd_xsayw(narrator_fd * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

int fd_sayw(narrator_fd * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

/// fd_seek
//
// SUMMARY
//  reposition a fd narrator
//
// PARAMETERS
//  n      - narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
// REMARKS
//  support depends on the underlying fd
//
xapi fd_xseek(narrator_fd * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull));

off_t fd_seek(narrator_fd * const restrict n, off_t offset, int whence)
  __attribute__((nonnull));

/// fd_destroy
//
// SUMMARY
//  destroy a fd narrator
//
void fd_destroy(narrator_fd * const restrict n)
  __attribute__((nonnull));

/// narrator_fd_read
//
// SUMMARY
//  read from the underlying file descriptor
//
// PARAMETERS
//  n     - fd narrator
//  dst   - destination buffer
//  count - number of bytes to read
//
// RETURNS
//  number of bytes actually read <= count
//
xapi fd_xread(narrator_fd * restrict n, void * dst, size_t count, size_t * restrict r)
  __attribute__((nonnull(1, 2)));

#endif
