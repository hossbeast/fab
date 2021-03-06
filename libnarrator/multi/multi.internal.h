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

#ifndef _NARRATOR_MULTI_INTERNAL_H
#define _NARRATOR_MULTI_INTERNAL_H

#include <stdarg.h>

#include "xapi.h"
#include "types.h"

#include "multi.h"

/// multi_say
//
// SUMMARY
//  write to a multi narrator
//
// PARAMETERS
//  n     - multi narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
//
xapi multi_xsayvf(narrator_multi * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

void multi_sayvf(narrator_multi * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi multi_xsayw(narrator_multi * const restrict n, const char * const restrict b, size_t l);

void multi_sayw(narrator_multi * const restrict n, const char * const restrict b, size_t l);

/// multi_seek
//
// SUMMARY
//  Seek all of the narrators underlying a multi narrator. It is unspecified which of them res is updated to reflect.
//
// PARAMETERS
//  n      - narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
xapi multi_xseek(narrator_multi * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull));

/// multi_seek
//
// RETURNS
//  a negative value if the operation failed for any of the underlying narrators, and zero otherwise
//
off_t multi_seek(narrator_multi * const restrict n, off_t offset, int whence)
  __attribute__((nonnull));

/// multi_destroy
//
// SUMMARY
//  destroy a multi narrator
//
void multi_destroy(narrator_multi * const restrict n)
  __attribute__((nonnull));

#endif
