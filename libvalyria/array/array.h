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

#ifndef _VALYRIA_ARRAY_H
#define _VALYRIA_ARRAY_H

/*

SUMMARY
 dynamically resizing ordered collection of elements addressable by index

REMARKS
 meant to be used as the primary storage for the elements in the collection

*/

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

typedef struct array
{
  size_t l;

#ifndef ARRAY_INTERNALS
# define ARRAY_INTERNALS
#endif
  ARRAY_INTERNALS;
} array;

#ifndef ARRAY_ELEMENT_TYPE
# define ARRAY_ELEMENT_TYPE void
#endif

#define restrict __restrict

/// array_create
//
// SUMMARY
//  create an empty array
//
// PARAMETERS
//  array        - created array goes here
//  esz          - element size > 0
//  [destructor] - invoked with key/value just before freeing their associated storage
//  [capacity]   - initial capacity
//
// REMARKS
//  either attr == ARRAY_PRIMARY and esz != 0 OR attr == ARRAY_SECONDARY and esz == 0
//
xapi array_create(array ** const restrict ar, size_t esz)
  __attribute__((nonnull));

xapi array_createx(array ** const restrict ar, size_t esz, void (*destructor)(ARRAY_ELEMENT_TYPE *), size_t capacity)
  __attribute__((nonnull(1)));

/// array_free
//
// SUMMARY
//  free a array with free semantics
//
void array_free(array * const ar);

/// array_xfree
//
// SUMMARY
//  free a array with xfree semantics
//
void array_xfree(array ** const ar)
  __attribute__((nonnull));

/// array_size
//
// SUMMARY
//  get the number of elements in the array
//
size_t array_size(const array * const ar)
  __attribute__((nonnull));

/// array_get
//
// SUMMARY
//  retrieve an element from the array by index
//
// PARAMETERS
//  ar - array
//  x  - index
//
// RETURNS
//  pointer to element
//
ARRAY_ELEMENT_TYPE * array_get(const array * const restrict ar, int x)
  __attribute__((nonnull));

/// array_shift
//
// SUMMARY
//  remove the first element of the array
//
// PARAMETERS
//  ar   - array
//  [el] - (returns) pointer to element
//
// REMARKS
//  the destructor is called before this function returns
//
xapi array_shift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull(1)));

/// array_pop
//
// SUMMARY
//  remove the last element of the array
//
// PARAMETERS
//  ar   - array
//  [el] - (returns) pointer to element
//
// REMARKS
//  the destructor is called before this function returns
//
xapi array_pop(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull(1)));

/// array_push
//
// SUMMARY
//  add an element at the end of an array
//
// PARAMETERS
//  ar - array
//  el - (returns) pointer to element
//
xapi array_push(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull));

/// array_push_range
//
// SUMMARY
//  add elements to the end of the array
//
// PARAMETERS
//  ar  - array
//  len - number of elements
//  el  - (returns) pointers to elements
//
xapi array_push_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull));

/// array_unshift
//
// SUMMARY
//  add an element to the beginning of an array
//
// PARAMETERS
//  ar - array
//  el - (returns) pointer to element
//
xapi array_unshift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull));

/// array_unshift_range
//
// SUMMARY
//  add elements to the beginning of an array
//
// REMARKS
//  the order of the elements is not reversed
//
// PARAMETERS
//  ar  - array
//  len - number of elements
//  el  - (returns) pointers to elements
//
xapi array_unshift_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull));

/// array_insert
//
// SUMMARY
//  add an element to an array at a specific location
//
// PARAMETERS
//  ar    - array
//  index - 0 <= index <= array_size(s)
//  el    - (returns) pointer to element
//
xapi array_insert(array * const restrict ar, size_t index, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull));

/// array_insert_range
//
// SUMMARY
//  add elements to an array at a specific location
//
// PARAMETERS
//  ar    - array
//  index - 0 <= index <= array_size(s)
//  len   - number of elements to insert
//  el    - (returns) pointers to elements
//
xapi array_insert_range(array * const restrict ar, size_t index, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull));

/// array_clear
//
// SUMMARY
//  remove all elements from the array
//
// REMARKS
//  allocations remain intact
//
void array_clear(array * const ar)
  __attribute__((nonnull));

/// array_sort
//
// SUMMARY
//  sort an array - see qsort
//
// PARAMETERS
//  ar     - array
//  compar - comparison function
//  [arg]  - passed to compar
//
void array_sort(array * const restrict ar, int (*compar)(const ARRAY_ELEMENT_TYPE *, const ARRAY_ELEMENT_TYPE *, void *), void * arg)
  __attribute__((nonnull(1, 2)));

#undef restrict
#endif
