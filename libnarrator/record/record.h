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

#ifndef _NARRATOR_RECORD_H
#define _NARRATOR_RECORD_H

/*

MODULE
 narrator/record

SUMMARY
 narrator that accumulates writes and propagates records to an underlying narrator

*/

#include "xapi.h"
#include "types.h"

/// narrator_record_create
//
// SUMMARY
//  allocate a record narrator
//
// PARAMETERS
//  n  - (returns) narrator
//  np - underlying narrator
//
xapi narrator_record_create(narrator ** const restrict n, narrator * const restrict np)
  __attribute__((nonnull(1)));

/// narrator_record_reset
//
// SUMMARY
//  resets the underlying narrator
//
// PARAMETERS
//  n - record narrator
//
void narrator_record_reset(narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_record_flush
//
// SUMMARY
//  propagate to the underlying narrator with a single write
//
xapi narrator_record_flush(narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_record_size
//
// SUMMARY
//  get the size in bytes of the unwritten record
//
// PARAMETERS
//  n - record narrator
//
size_t narrator_record_size(narrator * const restrict n)
  __attribute__((nonnull));

#endif
