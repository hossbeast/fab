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

#ifndef _VALYRIA_SET_H
#define _VALYRIA_SET_H

/*

MODULE
 set

SUMMARY
 dynamically resizing unordered collection of unique elements

*/

#include "xapi.h"
#include "types.h"

#include "hashtable.h"

typedef struct set
{
  hashtable;

  void (*free_element)(void *, size_t);
  xapi (*xfree_element)(void *, size_t);
} set;

/// set_create
//
// SUMMARY
//  create an empty set
//
// PARAMETERS
//  s             - (returns) newly allocated set
//  [free_value]  - invoked with the element just before freeing it
//  [xfree_value] - invoked with the element just before freeing it
//  [capacity]    - initial capacity, the minimum number of elements which can be set without rehashing
//
xapi set_create(set ** restrict s)
  __attribute__((nonnull));

xapi set_createx(
    set ** restrict s
  , size_t capacity
  , void * free_element
  , void * xfree_element
)
  __attribute__((nonnull(1)));

/// set_xfree
//
// SUMMARY
//  free a set
//
xapi set_xfree(set * restrict s);

/// set_ixfree
//
// SUMMARY
//  free a set, zero its reference
//
xapi set_ixfree(set ** restrict s)
  __attribute__((nonnull));

/// set_put
//
// SUMMARY
//  add an element to the set
//
// PARAMETERS
//  set      - set
//  element  - pointer to element
//  elementl - element length > 0
//
xapi set_put(set * s, const void * e, uint16_t el)
  __attribute__((nonnull));

/// set_contains
//
// SUMMARY
//  determine whether a set contains an element
//
// PARAMETERS
//  s  - set
//  e  - pointer to element
//  el - element length > 0
//
// RETURNS
//  true if the element is in the set
//
bool set_contains(const set * s, const void * e, uint16_t el)
  __attribute__((nonnull));

bool set_equal(set * const A, set * const B)
  __attribute__((nonnull));

/// set_recycle
//
// disassociate all elements - internal structures in the set remain allocated
//
xapi set_recycle(set * restrict s)
  __attribute__((nonnull));

/// set_delete
//
// SUMMARY
//  remove an element from the set, if any
//
// PARAMETERS
//  s        - set
//  element  - pointer to element
//  elementl - length of element
//
xapi set_delete(set * restrict s, const void * restrict e, uint16_t el)
  __attribute__((nonnull));

/// set_elements
//
// SUMMARY
//  returns a list of elements in the set
//
// PARAMETERS
//  s    - set
//  es   - (returns) list of elements to be freed by the caller
//  esls - (returns) list of element lenghts to be freed by the caller
//  esl  - (returns) size of returned lists
//
xapi set_elements(const set * restrict s, const void *** restrict es, uint16_t * restrict esls, size_t * restrict esl)
  __attribute__((nonnull));

/// set_table_element
//
// SUMMARY
//  get the element at the specified slot, if any
//
// PARAMETERS
//  s  - set
//  x  - slot index
//  e  - (returns) the element
//  el - (returns) length of the element
//
// RETURNS
//  true if an element was found at the specified slot
//
bool set_table_element(const set * restrict s, size_t x, void * e, uint16_t * el)
  __attribute__((nonnull(1)));

#endif
