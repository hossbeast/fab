/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _WSTDLIB_H
#define _WSTDLIB_H

#include <stdlib.h>
#include <sys/types.h>

/// xmalloc
//
// SUMMARY
//  does this: *t = calloc(sz, 1);
//  recall that calloc zeroes out the memory it allocates
//
// RETURNS
//  returns 0 on success
//  
int wmalloc(void* t, size_t sz)
	__attribute__((nonnull));

/// xrealloc
//
// SUMMARY
//  does this: realloc(*t, sz) AND ensures that any new portion of memory is zeroed out
//
// PARAMETERS
//  es  - element size
//  ec  - element count
//  oec - old element count
//
// RETURNS
//  returns 0 on success
//
int wrealloc(void* t, size_t es, size_t ec, size_t oec)
	__attribute__((nonnull));

#endif
