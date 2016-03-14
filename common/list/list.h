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

#ifndef _LIST_H
#define _LIST_H

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

struct list;
typedef struct list list;

#ifndef LIST_ELEMENT_TYPE
# define LIST_ELEMENT_TYPE void
#endif

/*

SUMMARY
 dynamically resizing ordered collection of elements addressable by index

REMARKS
 meant to be used as secondary storage for the elements in the collection

*/

#define restrict __restrict

/// list_create
//
// SUMMARY
//  create an empty list
//
// PARAMETERS
//  list         - created list goes here
//  [destructor] - invoked with key/value just before freeing their associated storage
//  [attr]       - bitwise combination of LIST_* options and modifiers
//  [capacity]   - initial capacity
//
xapi list_create(list ** const restrict li, void (*destructor)(LIST_ELEMENT_TYPE *))
  __attribute__((nonnull(1)));

xapi list_createx(list ** const restrict li, void (*destructor)(LIST_ELEMENT_TYPE *), size_t capacity)
  __attribute__((nonnull(1)));

/// list_free
//
// SUMMARY
//  free a list with free semantics
//
void list_free(list * const restrict li);

/// list_xfree
//
// SUMMARY
//  free a list with xfree semantics
//
void list_xfree(list ** const restrict li)
  __attribute__((nonnull));

/// list_size
//
// SUMMARY
//  get the number of elements in the list
//
size_t list_size(const list * const restrict li)
  __attribute__((nonnull));

/// list_get
//
// SUMMARY
//  retrieve an element from the list by index
//
LIST_ELEMENT_TYPE * list_get(const list * const restrict li, int x)
  __attribute__((nonnull));

/// list_shift
//
// SUMMARY
//  remove the first element of the list
//
// PARAMETERS
//  li - 
//  el - 
//
// REMARKS
//  the destructor is called on the element before this function returns
//
xapi list_shift(list * const restrict li, LIST_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull(1)));

/// list_pop
//
// SUMMARY
//  remove the last element of the list
//
// REMARKS
//  the destructor is called on the element before this function returns
//
xapi list_pop(list * const restrict li, LIST_ELEMENT_TYPE ** const restrict el)
  __attribute__((nonnull(1)));

/// list_push
//
// SUMMARY
//  add an element to the end of the list
//
// PARAMETERS
//  li - list
//  el - pointer to element
//
xapi list_push(list * const restrict li, LIST_ELEMENT_TYPE * const el)
  __attribute__((nonnull));

/// list_push_range
//
// SUMMARY
//  add elements to the end of the list
//
// PARAMETERS
//  s   - list
//  el  - pointer to first element to append
//  len - number of elements
//
xapi list_push_range(list * const restrict li, LIST_ELEMENT_TYPE * const * const el, size_t len)
  __attribute__((nonnull));

/// list_unshift
//
// SUMMARY
//  add an element to the beginning of the list
//
// PARAMETERS
//  s  - list
//  el - pointer to element
//
xapi list_unshift(list * const restrict li, LIST_ELEMENT_TYPE * const el)
  __attribute__((nonnull));

/// list_unshift_range
//
// SUMMARY
//  add elements to the beginning of the list
//
// REMARKS
//  the order of the elements is not reversed
//
// PARAMETERS
//  s   - list
//  el  - pointer to first element to append
//  len - number of elements
//
xapi list_unshift_range(list * const restrict li, LIST_ELEMENT_TYPE * const * const el, size_t len)
  __attribute__((nonnull));

/// list_insert
//
// SUMMARY
//  insert an element into the list at the specified index
//
// PARAMETERS
//  s     - list
//  index - 0 <= index <= list_size(s)
//  el    - pointer to element to insert
//
xapi list_insert(list * const restrict li, size_t index, LIST_ELEMENT_TYPE * const el)
  __attribute__((nonnull));

/// list_insert_range
//
// SUMMARY
//  insert elements into the list at the specified index
//
// PARAMETERS
//  s     - list
//  index - 0 <= index <= list_size(s)
//  el    - pointer to the first element to insert
//  len   - number of elements to insert
//
xapi list_insert_range(list * const restrict li, size_t index, LIST_ELEMENT_TYPE * const * const el, size_t len)
  __attribute__((nonnull));

/// list_clear
//
// SUMMARY
//  reset the number of elements in the list - the allocation remains intact
//
void list_clear(list * const restrict li)
  __attribute__((nonnull));

/// list_sort
//
// SUMMARY
//  sort the list
//
void list_sort(list * const restrict li, int (*compar)(const LIST_ELEMENT_TYPE *, const LIST_ELEMENT_TYPE *, void *), void * arg)
  __attribute__((nonnull(1, 2)));

/// list_sublist
//
// SUMMARY
//  create subset of a list - the two lists contain pointers to the same underlying objects
//
// PARAMETERS
//  list  - list to copy from
//  index - index at which to begin copying
//  len   - number of elements to copy
//  rv    - (returns) pointer to list, or reuses an existing list
//
xapi list_sublist(list * const restrict lip, size_t index, size_t len, list ** const restrict rv)
  __attribute__((nonnull));

#undef restrict
#endif
