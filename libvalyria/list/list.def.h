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

#ifndef _LIST_DEF_H
#define _LIST_DEF_H

/*

MODULE
 list.def

REMARKS
 low-level list create / mutate apis

*/

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

struct list;    // list.h

#define LIST_PRIMARY    0x01  /* primary storage of elements in the list */
#define LIST_SECONDARY  0x02  /* not the primary storage of elements in the list */

#ifndef LIST_ELEMENT_TYPE
# define LIST_ELEMENT_TYPE void
#endif

struct list_internals
{
  char *   v;     // storage
  size_t   a;     // allocated size in elements

  uint32_t attr;
  size_t   esz;   // element size, for primary storage
  void (*destructor)(LIST_ELEMENT_TYPE *);
};

#define restrict __restrict

/// list_allocate
//
// SUMMARY
//  create an empty list
//
// PARAMETERS
//  list         - (returns) list instance
//  attr         - bitwise combination of LIST_* options and modifiers
//  [esz]        - value size > 0
//  [destructor] - invoked with key/value just before freeing their associated storage
//  [capacity]   - initial capacity
//
// REMARKS
//  either attr & LIST_PRIMARY && esz != 0 or attr & LIST_SECONDARY && esz == 0
//
xapi list_allocate(struct list ** const restrict li, uint32_t attr, size_t esz, void (*destructor)(LIST_ELEMENT_TYPE *), size_t capacity)
  __attribute__((nonnull(1)));

/// list_add
//
// SUMMARY
//  add elements to the list
//
// PARAMETERS
//  li    - list
//  index - 0 <= index <= list_size(li)
//  len   - number of elements to add
//  [el]  - pointer to the first element to add
//  [rv]  - (returns) pointers to elements
//
xapi list_add(struct list * const restrict li, size_t index, size_t len, LIST_ELEMENT_TYPE * const * const el, LIST_ELEMENT_TYPE ** const restrict rv)
  __attribute__((nonnull(1)));

/// list_put
//
//
//
void list_put(struct list * const restrict li, size_t index, size_t len, LIST_ELEMENT_TYPE * const * const el, LIST_ELEMENT_TYPE ** const restrict rv)
  __attribute__((nonnull(1)));

#undef restrict
#endif
