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

#ifndef _NARRATE_FIXED_H
#define _NARRATE_FIXED_H

/*

MODULE
 narrator/fixed

SUMMARY
 narrator that writes to a fixed-size buffer, discarding overflow

*/

#define restrict __restrict

/// narrator_fixed_create
//
// SUMMARY
//  allocate a file narrator
//
// PARAMETERS
//  n    - (returns) narrator
//  size - size of the buffer
//
xapi narrator_fixed_create(narrator ** const restrict n, size_t size)
  __attribute__((nonnull));

/// narrator_fixed_buffer
//
// SUMMARY
//  get a pointer to the underlying store
//
// PARAMETERS
//  n - fixed narrator
//
// REMARKS
//  further operations on the narrator may free the buffer
//
const char * narrator_fixed_buffer(narrator * const restrict n)
  __attribute__((nonnull));

#undef restrict
#endif
