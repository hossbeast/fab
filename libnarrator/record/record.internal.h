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

#ifndef _NARRATOR_RECORD_INTERNAL_H
#define _NARRATOR_RECORD_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "record.h"

/// record_say
//
// SUMMARY
//  accumulate to the buffer
//
xapi record_xsayvf(narrator_record * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi record_xsayw(narrator_record * const restrict n, const char * const restrict b, size_t l);

/// record_seek
//
// SUMMARY
//  seek the underlying narrator
//
xapi record_xseek(narrator_record * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull));

/// record_destroy
//
// SUMMARY
//  destroy a record narrator
//
void record_destroy(narrator_record * const restrict n)
  __attribute__((nonnull));

xapi record_xread(narrator_record * restrict n, void * dst, size_t count, size_t * restrict r)
  __attribute__((nonnull(1, 2)));

#endif
