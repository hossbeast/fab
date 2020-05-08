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

narrator * narrator_growing_init_from(char stor[NARRATOR_STATIC_SIZE], char * buf, size_t bufa)
  __attribute__((nonnull(1)));

/// narrator_growing_buffer
//
// SUMMARY
//  get a pointer to the underlying buffer
//
// PARAMETERS
//  n - growing narrator
//
// REMARKS
//  subsequent operations on the narrator may free the buffer
//
char * narrator_growing_buffer(narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_growing_claim_buffer
//
// SUMMARY
//  claim the underlying buffer and reset the narrator
//
void narrator_growing_claim_buffer(narrator * restrict n, void * bufp, size_t * allocp)
  __attribute__((nonnull(1)));

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

xapi narrator_growing_allocate(narrator * restrict n, size_t size)
  __attribute__((nonnull));

#endif
