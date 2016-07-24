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

#ifndef _NARRATOR_NULLITY_INTERNAL_H
#define _NARRATOR_NULLITY_INTERNAL_H

#include "xapi.h"
#include "nullity.h"

typedef struct narrator_nullity
{
} narrator_nullity;

#define restrict __restrict

/// nullity_setup
//
// SUMMARY
//  module initialization
//
xapi nullity_setup(void);

/// nullity_teardown
//
// SUMMARY
//  module cleanup
//
void nullity_teardown(void);

/// nullity_say
//
// SUMMARY
//  no-op
//

xapi nullity_vsayf(narrator_nullity * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi nullity_sayw(narrator_nullity * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

/// nullity_seek
//
// SUMMARY
//  no-op
//
xapi nullity_seek(narrator_nullity * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/// narrator_destroy
//
// SUMMARY
//  destroy a nullity narrator
//
void nullity_destroy(narrator_nullity * restrict n);

#undef restrict
#endif
