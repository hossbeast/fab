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

#ifndef _SORT_H
#define _SORT_H

#include <sys/types.h>

/*
All of these functions are based on qsort, see :

see http://man7.org/linux/man-pages/man3/qsort.3.html
*/

#define restrict __restrict

/// sort_r
//
// SUMMARY
//  like qsort_r
//
// PARAMETERS
//  key    - identifies the element to be found
//  base   - pointer to the first element in the array
//  nmemb  - number of elements in the array
//  size   - size of elements in the array
//  compar - invoked to compare key to an array member
//  [arg]  - passed through to compar
//  [el]   - (returns) pointer to the last element considered
//
// RETURNS
//  boolean value indicating whether or not a matching element was found
//
void sort_r(void * restrict base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void * arg)
  __attribute__((nonnull(1, 4)));

#undef restrict
#endif
