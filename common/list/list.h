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

/// list_append
//
// SUMMARY
//
// PARAMETERS
//  s  - list
//  el - pointer to element to append
//
xapi list_append(list * const restrict s, void * const restrict el)
  __attribute__((nonnull));

/// list_append_range
//
// SUMMARY
//
// PARAMETERS
//  s   - list
//  el  - pointer to first element to append
//  len - number of elements
//
xapi list_append_range(list * const restrict s, void * const restrict el, size_t len)
  __attribute__((nonnull));

/// list_insert
//
// SUMMARY
//  insert an element into the list at the specified index. when index ==
//  list_size, decays to list_append
//
// PARAMETERS
//  s     - list
//  index - 0 <= index <= list_size(s)
//  el    - pointer to element to insert
//
xapi list_insert(list * const restrict s, size_t index, void * const restrict el)
  __attribute__((nonnull));

/// list_grow
//
// SUMMARY
// 
// PARAMETERS
//
xapi list_grow(list * const restrict s, size_t len)
  __attribute__((nonnull));

/// list_ensure
//
// SUMMARY
// 
// PARAMETERS
//
xapi list_ensure(list * const restrict s, size_t len)
  __attribute__((nonnull));

/// list_clear
//
// SUMMARY
//  reset the number of elements in the list - the allocation remains intact
//
void list_clear(list * const restrict s)
  __attribute__((nonnull));

void list_sort(list * const restrict s, int (*compar)(const void *, const void *, void *), void * arg)
  __attribute__((nonnull(1, 2)));

#undef restrict
#endif
