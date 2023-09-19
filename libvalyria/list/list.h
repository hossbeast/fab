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

#ifndef _VALYRIA_LIST_H
#define _VALYRIA_LIST_H

/*

SUMMARY
 dynamically resizing ordered collection of items addressable by index

REMARKS
 list stores references to objects

*/

#include "types.h"

#include "array.h"

typedef struct list {
  size_t size;
} list;

/// list_create
//
// SUMMARY
//  create an empty list
//
// PARAMETERS
//  list         - created list goes here
//  [capacity]   - initial capacity
//  [free_item]  - invoked on an element before releasing its storage
//
// REMARKS
//  free_item follows the free idiom
//
void list_create(list ** const restrict li)
  __attribute__((nonnull(1)));

void list_createx(
    list ** const restrict li
  , size_t capacity
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * free_fn // void (*free_fn)(void * item)
)
  __attribute__((nonnull(1)));

/// list_xfree
//
// SUMMARY
//  free a list
//
void list_xfree(list * const restrict li);

/// list_ixfree
//
// SUMMARY
//  free a list, zero its reference
//
void list_ixfree(list ** const restrict li)
  __attribute__((nonnull));

void list_suppress_free_fn(list * const restrict li)
  __attribute__((nonnull));

void list_restore_free_fn(list * const restrict li)
  __attribute__((nonnull));

/// list_recycle
//
// SUMMARY
//  reset the number of elements in the list - the allocation remains intact
//
void list_recycle(list * const restrict li)
  __attribute__((nonnull));

/// list_truncate
//
// SUMMARY
//  set the size of the list
//
// PARAMETERS
//  len - new size of the list, 0 <= len <= li->l
//
void list_truncate(list * const restrict li, size_t len)
  __attribute__((nonnull));

/// list_get
//
// SUMMARY
//  get an element from a list by index
//
void * list_get(const list * const restrict li, int x)
  __attribute__((nonnull));

/// list_get_item
//
// SUMMARY
//  get an element from a list by index
//
// PARAMETERS
//  li         - list
//  x          - index
//  [item]     - (returns) pointer to the item, if any
//  [item_len] - (returns) item length
//
bool list_get_item(const list * const restrict li, int x, void * restrict item, size_t * restrict item_len)
  __attribute__((nonnull(1)));

/// list_push
//
// SUMMARY
//  add an element to the end of the list
//
// PARAMETERS
//  li     - list
//  [item] - pointer to item
//
void list_push(list * const li, void * item, size_t item_len)
  __attribute__((nonnull));

/// list_push_range
//
// SUMMARY
//  add items to the end of the list
//
// PARAMETERS
//  li           - list
//  items        - pointer to first item
//  [items_lens] - pointer to item lengths
//  len          - number of items
//
void list_push_range(list * const restrict li, void * restrict items, size_t * restrict items_lens, size_t len)
  __attribute__((nonnull(1, 2)));

/// list_pop
//
// SUMMARY
//  remove the last item in the list, if any
//
// PARAMETERS
//  li - list
//
// REMARKS
//  x/free_item is called on the element before this function returns
//
void list_pop(list * const li)
  __attribute__((nonnull(1)));

/// list_unshift
//
// SUMMARY
//  add an element to the beginning of the list
//
// PARAMETERS
//  li         - list
//  item       - pointer to item
//  [item_len] - item length
//
void list_unshift(list * const li, void * item, size_t item_len)
  __attribute__((nonnull));

/// list_unshift_range
//
// SUMMARY
//  add items to the beginning of the list
//
// REMARKS
//  the order of the elements is not reversed
//
// PARAMETERS
//  li         - list
//  items      - pointer to first item
//  items_lens - pointer to item lengths
//  len        - number of items
//
void list_unshift_range(list * const li, void * items, size_t * restrict items_lens, size_t len)
  __attribute__((nonnull(1, 2)));

/// list_shift
//
// SUMMARY
//  remove the first item of the list, if any
//
// PARAMETERS
//  li - list
//
// REMARKS
//  x/free_item is called on the element before this function returns
//
void list_shift(list * const li)
  __attribute__((nonnull(1)));

/// list_insert
//
// SUMMARY
//  insert an item into the list at the specified index
//
// PARAMETERS
//  li       - list
//  index    - 0 <= index <= li->l
//  item     - pointer to item
//  item_len - item length
//
void list_insert(list * const li, size_t index, void * item, size_t item_len)
  __attribute__((nonnull));

/// list_insert_range
//
// SUMMARY
//  insert items into the list at the specified index
//
// PARAMETERS
//  li           - list
//  index        - 0 <= index <= li->l
//  items        - pointer to first item
//  [items_lens] - pointer to item lengths
//  len          - number of items
//
void list_insert_range(list * const li, size_t index, void * items, size_t * restrict items_lens, size_t len)
  __attribute__((nonnull(1, 3)));

/// list_update
//
// SUMMARY
//  update the element in the list at the specified index
//
// REMARKS
//  the existing element at the index is destroyed
//
// PARAMETERS
//  li    - list
//  index - 0 <= index < li->l
//  el    - pointer to the element to place
//
void list_update(list * const restrict li, size_t index, void * item, size_t item_len)
  __attribute__((nonnull));

/// list_update_range
//
// SUMMARY
//  update elements in the list at the specified range of indexes
//
// REMARKS
//  the existing elements at the index are destroyed
//
// PARAMETERS
//  li           - list
//  index        - 0 <= index < li->l
//  items        - pointer to first item
//  [items_lens] - pointer to item lengths
//  len          - number of items
//
void list_update_range(list * const restrict li, size_t index, void * items, size_t * restrict items_lens, size_t len)
  __attribute__((nonnull(1, 3)));

/// list_delete
//
// SUMMARY
//  remove an item from the list
//
// PARAMETERS
//  li    - list
//  index - index of the item to delete
//  [item]     - (returns) removed item
//  [item_len] - (returns) removed item length
//
void list_delete(list * const restrict li, size_t index)
  __attribute__((nonnull));

/// list_delete_range
//
// SUMMARY
//  remove contiguous items from the list
//
// PARAMETERS
//  li         - list
//  index      - index of the first item to delete
//
void list_delete_range(list * const restrict li, size_t index, size_t len)
  __attribute__((nonnull));

/// list_sublist
//
// SUMMARY
//  create subset of a list - the two lists contain pointers to the same underlying objects
//
// PARAMETERS
//  list  - list to copy from
//  index - index at which to begin copying
//  len   - number of elements to copy
//  lc    - (returns) pointer to list, or reuses an existing list
//
void list_sublist(list * const restrict lip, size_t index, size_t len, list ** const restrict lc)
  __attribute__((nonnull));

/// list_splice
//
// SUMMARY
//  remove elements from a source list and add them to a destination list
//
// REMARKS
//  the destination list is expanded to make room as necessary
//
// PARAMETERS
//  dst       - list to copy to
//  dst_index - 0 <= dst_index <= dst->size
//  src       - list to copy from
//  src_index - 0 <= src_index <= src->size
//  len       - number >= 0 of elements to copy
//
void list_splice(list * const restrict dst, size_t dst_index, list * const restrict src, size_t src_index, int len)
  __attribute__((nonnull));

/// list_replicate
//
// SUMMARY
//  copy elements from a source list to a destination list
//
// REMARKS
//  the destination is expanded to make room as necessary
//
// PARAMETERS
//  dst       - list to copy to
//  dst_index - 0 <= dst_index <= dst->l
//  src       - list to copy from
//  src_index - 0 <= src_index <= src->l
//  len       - number >= 0 of elements to copy
//
void list_replicate(list * const restrict dst, size_t dst_index, list * const restrict src, size_t src_index, int len)
  __attribute__((nonnull));

/// list_equal
//
// SUMARY
//  determine whether two lists contain the same items
//
// PARAMETERS
//  A     - first list
//  B     - second list
//  [cmp] - comparison function
//
bool list_equal(list * const restrict A, list * const restrict B, int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz))
  __attribute__((nonnull(1, 2)));

/// list_sort
//
// SUMMARY
//  sort a list in place
//
// SEE
//  man 3 qsort
//
// PARAMETERS
//  li - list
//
void list_sort(list * const restrict li, int (*cmp_fn)(const void *A, size_t Asz, const void *B, size_t Bsz))
  __attribute__((nonnull(1)));

/// list_search
//
// SUMMARY
//  search for an item in the list
//
// SEE
//  man 3 bsearch
//
// PARAMETERS
//  li         - list
//  [index]    - first index in the search range
//  [len]      - number of items in the search range
//  key        - item to search for
//  [key_len]  - item length
//  [item]     - (returns) pointer to the item, if found
//  [item_len] - (returns) item length, if found
//  [lx]       - (returns) index of the last item considered
//  [lc]       - (returns) result of the final comparison
//
bool list_search(
    const list * const restrict li
  , void * key_data
  , size_t key_len
  , int (*cmp_fn)(const void *A, size_t Asz, const void *B, size_t Bsz)
  , void * item
  , size_t * restrict item_len
  , size_t * restrict lx
  , int * restrict lc
)
  __attribute__((nonnull(1, 2)));

bool list_search_range(
    const list * const restrict li
  , size_t index
  , size_t len
  , void * key_data
  , size_t key_len
  , int (*cmp_fn)(const void *A, size_t Asz, const void *B, size_t Bsz)
  , void * item
  , size_t * restrict item_len
  , size_t * restrict lx
  , int * restrict lc
)
  __attribute__((nonnull(1, 4)));

#endif
