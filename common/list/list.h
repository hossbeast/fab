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

#include "list.h"

struct list;
typedef struct list list;

#ifndef LIST_ELEMENT_TYPE
# define LIST_ELEMENT_TYPE void*
#endif

#define LIST_DEREF  0x01

#define restrict __restrict

/// list_create
//
// create an empty list
//
// parameters
//  list			   - created list goes here
//  esz          - element size > 0
//  [destructor] - invoked with key/value just before freeing their associated storage
//  [attr]       - bitwise combination of LIST_* options and modifiers
//
// returns nonzero on success
//
xapi list_create(list ** const restrict list, size_t esz, void (*destructor)(LIST_ELEMENT_TYPE), uint32_t attr)
	__attribute__((nonnull(1)));

/// list_free
//
// SUMMARY
//  free a list with free semantics
//
void list_free(list * const restrict list);

/// list_xfree
//
// SUMMARY
//  free a list with xfree semantics
//
void list_xfree(list ** const restrict list)
	__attribute__((nonnull));

/// list_size
//
// SUMMARY
//  get the number of elements in the list
//
size_t list_size(list * const restrict list)
  __attribute__((nonnull));

/// list_get
//
// SUMMARY
//  retrieve an element from the list by index
//
LIST_ELEMENT_TYPE list_get(list * const restrict list, int x)
  __attribute__((nonnull));

/// list_shift
//
// SUMMARY
//  remove the first element of the list
//
LIST_ELEMENT_TYPE list_shift(list * const restrict list)
  __attribute__((nonnull));

/// list_pop
//
// SUMMARY
//  remove the last element of the list
//
LIST_ELEMENT_TYPE list_pop(list * const restrict list)
  __attribute__((nonnull));

/// list_push
//
// SUMMARY
//  add an item to the end of the list
//
// PARAMETERS
//  s  - list
//  el - pointer to element to append
//
xapi list_push(list * const restrict list, void * const restrict el)
  __attribute__((nonnull));

/// list_push_range
//
// SUMMARY
//  add items to the end of the list
//
// PARAMETERS
//  s   - list
//  el  - pointer to first element to append
//  len - number of elements
//
xapi list_push_range(list * const restrict list, void * const restrict el, size_t len)
  __attribute__((nonnull));

/// list_unshift
//
// SUMMARY
//  add an item to the beginning of the list
//
// PARAMETERS
//  s  - list
//  el - pointer to element to append
//
xapi list_unshift(list * const restrict list, void * const restrict el)
  __attribute__((nonnull));

/// list_unshift_range
//
// SUMMARY
//  add items to the beginning of the list
//
// PARAMETERS
//  s   - list
//  el  - pointer to first element to append
//  len - number of elements
//
xapi list_unshift_range(list * const restrict list, void * const restrict el, size_t len)
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
xapi list_insert(list * const restrict list, size_t index, void * const restrict el)
  __attribute__((nonnull));

/// list_insert_range
//
// SUMMARY
//  insert elements into the list at the specified index
//
// PARAMETERS
//  s     - list
//  index - 0 <= index <= list_size(s)
//  el    - pointer to element to insert
//  len   - number of elements
//
xapi list_insert_range(list * const restrict list, size_t index, void * const restrict el, size_t len)
  __attribute__((nonnull));

/// list_grow
//
// SUMMARY
// 
// PARAMETERS
//
xapi list_grow(list * const restrict list, size_t len)
  __attribute__((nonnull));

/// list_ensure
//
// SUMMARY
// 
// PARAMETERS
//
xapi list_ensure(list * const restrict list, size_t len)
  __attribute__((nonnull));

/// list_clear
//
// SUMMARY
//  reset the number of elements in the list - the allocation remains intact
//
void list_clear(list * const restrict list)
  __attribute__((nonnull));

/// list_sort
//
// SUMMARY
//  sort the list
//
void list_sort(list * const restrict list, int (*compar)(const void *, const void *, void *), void * arg)
  __attribute__((nonnull(1, 2)));

/// list_sublist
//
// SUMMARY
//  create a copy of a subset of a list
//
// PARAMETERS
//  list  - list to copy from
//  index - index at which to begin copying
//  len   - number of elements to copy
//  rv    - (returns) pointer to sublist 
//
xapi list_sublist(list * const restrict listp, size_t index, size_t len, list ** const restrict rv)
  __attribute__((nonnull));

#undef restrict
#endif
