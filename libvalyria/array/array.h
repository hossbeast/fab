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
 dynamically resizing ordered collection of items addressable by index

REMARKS
 array stores objects - the put operations make a copy

*/

#include "xapi.h"
#include "types.h"

typedef struct array {
  size_t size;
} array;

/// array_create
//
// SUMMARY
//  create an empty array
//
// PARAMETERS
//  array       - created array goes here
//  esz         - item size > 0
//  [capacity]  - initial capacity
//  [init]      - invoked on an item when its storage becomes used
//  [xinit]     - invoked on an item when its storage becomes used
//  [destroy]   - invoked on an item when its storage becomes unused
//  [xdestroy]  - invoked on an item when its storage becomes unused
//
// REMARKS
//  destroy_item follows the destroy idiom, not the free idiom, i.e. it should not free the pointer
//  itself, as this memory is owned by the array
//
xapi array_create(array ** const restrict ar, size_t esz)
  __attribute__((nonnull));

xapi array_createx(
    array ** const restrict ar
  , size_t esz
  , size_t capacity
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * init_fn
  , void * xinit_fn
  , void * destroy_fn
  , void * xdestroy_fn
)
  __attribute__((nonnull(1)));

/// array_xfree
//
// SUMMARY
//  free an array
//
xapi array_xfree(array * const restrict ar);

/// array_ifree
//
// SUMMARY
//  free an array, zero its reference
//
xapi array_ixfree(array ** const restrict ar)
  __attribute__((nonnull));

/// array_recycle
//
// SUMMARY
//  remove all items from the array
//
// REMARKS
//  allocations remain intact
//
xapi array_recycle(array * const ar)
  __attribute__((nonnull));

/// array_truncate
//
// SUMMARY
//  set the size of the array
//
// PARAMETERS
//  len - new size of the array, 0 <= len <= ar->l
//
xapi array_truncate(array * const restrict ar, size_t len)
  __attribute__((nonnull));

/// array_get
//
// SUMMARY
//  retrieve an item from the array by index
//
// PARAMETERS
//  ar - array
//  x  - index
//
// RETURNS
//  pointer to item
//
void * array_get(const array * const restrict ar, int x)
  __attribute__((nonnull));



/// array_push
//
// SUMMARY
//  add an item at the end of an array
//
// PARAMETERS
//  ar - array
//  el - (returns) pointer to item
//
xapi array_push(array * const restrict ar, void * const el)
  __attribute__((nonnull(1)));

/// array_push_range
//
// SUMMARY
//  add items to the end of the array
//
// PARAMETERS
//  ar  - array
//  len - number of items
//  el  - (returns) pointers to items
//
xapi array_push_range(array * const restrict ar, void * items, size_t len)
  __attribute__((nonnull));

/// array_pop
//
// SUMMARY
//  remove the last item of the array
//
// PARAMETERS
//  ar   - array
//
// REMARKS
//  destroy_item is called before this function returns
//
xapi array_pop(array * const restrict ar)
  __attribute__((nonnull(1)));

/// array_unshift
//
// SUMMARY
//  add an item to the beginning of an array
//
// PARAMETERS
//  ar - array
//  el - (returns) pointer to item
//
xapi array_unshift(array * const restrict ar, void * el)
  __attribute__((nonnull));

/// array_unshift_range
//
// SUMMARY
//  add items to the beginning of an array
//
// REMARKS
//  the order of the items is not reversed
//
// PARAMETERS
//  ar  - array
//  len - number of items
//  el  - (returns) pointers to items
//
xapi array_unshift_range(array * const restrict ar, void * items, size_t len)
  __attribute__((nonnull));

/// array_shift
//
// SUMMARY
//  remove the first item of the array
//
// PARAMETERS
//  ar   - array
//
// REMARKS
//  destroy_item is called before this function returns
//
xapi array_shift(array * const restrict ar)
  __attribute__((nonnull(1)));

/// array_insert
//
// SUMMARY
//  add an item to an array at a specific location, expanding the array to retain existing items
//
// PARAMETERS
//  ar    - array
//  index - 0 <= index <= array_size(s)
//  el    - (returns) pointer to item
//
xapi array_insert(array * const restrict ar, size_t index, void * el)
  __attribute__((nonnull));

/// array_insert_range
//
// SUMMARY
//  add items to an array at a specific location, expanding the array to retain existing items
//
// PARAMETERS
//  ar    - array
//  index - 0 <= index <= array_size(s)
//  len   - number of items to insert
//  el    - (returns) pointers to items
//
xapi array_insert_range(array * const restrict ar, size_t index, void * items, size_t len)
  __attribute__((nonnull));

/// array_update
//
// SUMMARY
//  add an item to an array at a specific location, overwriting existing items if any
//
// PARAMETERS
//  ar    - array
//  index - 0 <= index <= array_size(s)
//  el    - (returns) pointer to item
//
xapi array_update(array * const restrict ar, size_t index, void * el)
  __attribute__((nonnull));

/// array_update_range
//
// SUMMARY
//  add items to an array at a specific location, overwriting existing items if any
//
// PARAMETERS
//  ar    - array
//  index - 0 <= index <= array_size(s)
//  len   - number of items to update
//  el    - (returns) pointers to items
//
xapi array_update_range(array * const restrict ar, size_t index, void * items, size_t len)
  __attribute__((nonnull));

/// array_delete
//
// SUMMARY
//  remove an item from the array
//
// PARAMETERS
//  ar    - array to delete from
//  index - index of the item to delete
//
xapi array_delete(array * const restrict ar, size_t index)
  __attribute__((nonnull));

/// array_delete_range
//
// SUMMARY
//  remove contiguous items from the array
//
// PARAMETERS
//  ar    - array to delete from
//  index - index of the first item to delete
//  len   - number of items to delete
//
xapi array_delete_range(array * const restrict ar, size_t index, size_t len)
  __attribute__((nonnull));

/// array_subarray
//
// SUMMARY
//  create subset of an array
//
// PARAMETERS
//  array - array to copy from
//  index - index at which to begin copying
//  len   - number of elements to copy
//  lc    - (returns) pointer to array, or reuses an existing array
//
xapi array_subarray(array * const restrict lip, size_t index, size_t len, array ** const restrict lc)
  __attribute__((nonnull));

/// array_splice
//
// SUMMARY
//  remove items between from a source array and add them to a destination array
//
// REMARKS
//  the destination is expanded to make room as necessary
//
// PARAMETERS
//  dst       - array to copy to
//  dst_index - 0 <= dst_index <= dst->l
//  src       - array to copy from
//  src_index - 0 <= src_index <= src->l
//  len       - number >= 0 of items to copy
//
xapi array_splice(array * const restrict dst, size_t dst_index, array * const restrict src, size_t src_index, size_t len)
  __attribute__((nonnull));

/// array_replicate
//
// SUMMARY
//  copy items from a source array to a destination array
//
// REMARKS
//  the destination is expanded to make room as necessary
//
// PARAMETERS
//  dst       - array to copy to
//  dst_index - 0 <= dst_index <= dst->l
//  src       - array to copy from
//  src_index - 0 <= src_index <= src->l
//  len       - number >= 0 of items to copy
//
xapi array_replicate(array * const restrict dst, size_t dst_index, array * const restrict src, size_t src_index, size_t len)
  __attribute__((nonnull));

/// array_equal
//
// SUMARY
//  determine whether two arrays contain the same items
//
// PARAMETERS
//  A     - first array
//  B     - second array
//  [cmp] - comparison function
//
bool array_equal(array * const restrict A, array * const restrict B, int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz))
  __attribute__((nonnull(1, 2)));

/// array_sort
//
// SUMMARY
//  sort an array in place
//
// SEE
//  man 3 qsort
//
// PARAMETERS
//  ar - array
//
void array_sort(const array * const restrict ar, int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz))
  __attribute__((nonnull(1)));

/// array_search
//
// SUMMARY
//  search for an item in the array
//
// SEE
//  man 3 bsearch
//
// PARAMETERS
//  ar         - array
//  [index]    - first index in the search range
//  [len]      - number of items in the search range
//  [key]      - item to search for
//  [cmp]      - comparison function
//  [item]     - (returns) pointer to the item, if found
//  [lx]       - (returns) index of the last item considered
//  [lc]       - (returns) result of the final comparison
//
bool array_search(
    const array * const restrict ar
  , void * key
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * item
  , size_t * restrict lx
  , int * restrict lc
)
  __attribute__((nonnull(1, 2)));

bool array_search_range(
    const array * const restrict ar
  , size_t index
  , size_t len
  , void * key
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * item
  , size_t * restrict lx
  , int * restrict lc
)
  __attribute__((nonnull(1, 4)));

#endif
