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

#ifndef _NARRATOR_GROWING_H
#define _NARRATOR_GROWING_H

/*

MODULE
 narrator/growing

SUMMARY
 narrator that accumulates all writes, growing without bound

*/

#include "xapi.h"
#include "types.h"
#include "narrator.h"

/// narrator_growing_create
//
// SUMMARY
//  allocate a growing narrator
//
// PARAMETERS
//  n - (returns) narrator
//
xapi narrator_growing_create(narrator ** const restrict n)
  __attribute__((nonnull));

/// narrator_growing_init
//
// SUMMARY
//  initialize a growing narrator
//
narrator * narrator_growing_init(char stor[NARRATOR_STATIC_SIZE])
  __attribute__((nonnull));

/// narrator_growing_buffer
//
// SUMMARY
//  get a pointer to the underlying store
//
// PARAMETERS
//  n - growing narrator
//
// REMARKS
//  further operations on the narrator may free the buffer
//
const char * narrator_growing_buffer(narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_growing_size
//
// SUMMARY
//  get the size in bytes of the data written to the underlying store
//
// PARAMETERS
//  n - growing narrator
//
size_t narrator_growing_size(narrator * const restrict n)
  __attribute__((nonnull));

#endif
