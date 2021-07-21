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

#ifndef _ARRAY_INTERNAL_H
#define _ARRAY_INTERNAL_H

/*

MODULE
 array.internal

*/

#include "types.h"

#include "array.h"
#include "macros.h"

struct array_t;
typedef struct ar_operations {
  /// compare_items
  //
  // SUMMARY
  //  compare items
  //
  int (*compare_items)(const struct array_t * ht, const void * a, const void * b, int (*cmp_fn)(const void *, size_t, const void *, size_t))
    __attribute__((nonnull(1, 2, 3)));

  /// destroy_item
  //
  // SUMMARY
  //  called when an item is destroyed (in array_destroy)
  //
  // PARAMETRS
  //  ht    - array
  //  item - item
  //
  void (*destroy_item)(const struct array_t * ht, void * item);

  /// store_item
  //
  // SUMMARY
  //   called when an item is stored in a bucket
  //
  // PARAMETRS
  //  ht    - array
  //  dst   - pointer to storage within a bucket of size ht->esz
  //  item - pointer to the item to store
  //
  void (*store_items)(const struct array_t * ht, void * dst, void * items, size_t len);
} ar_operations;

typedef struct array_t {
  union {
    array arx;
    struct {
      size_t size;
    };
  };

  char *   v;     // storage
  size_t   a;     // allocated size in items
  size_t   esz;   // item size, for primary storage

  // implementation
  ar_operations * ops;

  // user operations
  int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz);
  void (*init_fn)(void * item);
  void (*xinit_fn)(void * item);
  void (*destroy_fn)(void * item);
} array_t;

STATIC_ASSERT(offsetof(array, size) == offsetof(array_t, size));

/// array_init
//
// SUMMARY
//  initialize an already allocated array
//
void array_init(
    array_t * restrict ar
  , size_t esz
  , size_t capacity
  , ar_operations * restrict ops
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
)
  __attribute__((nonnull(1, 4)));

/// array_destroy
//
// SUMMARY
//
//
void array_xdestroy(array_t * restrict ar)
  __attribute__((nonnull));

/// array_put
//
// SUMMARY
//  add elements to the array
//
// PARAMETERS
//  li    - array
//  index - 0 <= index <= array_size(li)
//  len   - number of elements to add
//  [rv]  - (returns) pointers to elements
//
void array_put(array * const restrict li, size_t index, size_t len, void * restrict rv)
  __attribute__((nonnull(1)));

/// array_destroy_range
//
// SUMMARY
//  free a range of elements in a array
//
// PARAMETERS
//  ar    - pointer to array
//  start - index of the first element
//  len   - number of elements
//
void array_destroy_range(array_t * const restrict ar, size_t start, size_t len)
  __attribute__((nonnull(1)));

/// array_store
//
// SUMMARY
//  overwrite elements in the array
//
void array_store(array_t * const restrict ar, size_t index, size_t len, void * restrict items, void ** const restrict rv)
  __attribute__((nonnull(1)));

#endif
