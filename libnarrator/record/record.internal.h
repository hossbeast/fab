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
#include "record.h"

struct pstring;
struct narrator;

typedef struct narrator_record
{
  char *  s;    // buffer
  size_t  l;    // position
  size_t  a;    // allocated size

  struct narrator * n;    // underlying narrator
} narrator_record;

#define restrict __restrict

/// record_say
//
// SUMMARY
//  accumulate to the buffer
//

xapi record_vsayf(narrator_record * const restrict n, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

xapi record_sayw(narrator_record * const restrict n, const char * const restrict b, size_t l)
	__attribute__((nonnull));

/// record_write
//
// SUMMARY
//  flush the buffer to the underlying narrator
//
xapi record_write(narrator_record * const restrict n)
  __attribute__((nonnull));

/// record_seek
//
// SUMMARY
//  seek the underlying narrator
//
xapi record_seek(narrator_record * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/// record_destroy
//
// SUMMARY
//  destroy a record narrator
//
void record_destroy(narrator_record * restrict n);

#undef restrict
#endif
