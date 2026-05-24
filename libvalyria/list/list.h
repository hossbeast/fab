/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

SUMMARY
 dynamically resizing ordered collection of items addressable by index

REMARKS
 list stores references to objects

*/

#include "types.h"


typedef struct list {
  size_t size;  // items in use
  void **v;     // storage;
  size_t a;     // allocated size in items
  void (*free_fn)(void * item);
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
void list_create(list ** restrict li)
  __attribute__((nonnull));

void list_createx(
    list ** li
  , size_t capacity
  , void (*free_fn)(void *item)
)
  __attribute__((nonnull(1)));

/// list_free
//
// SUMMARY
//  free a list
//
void list_free(list * restrict li);

/// list_ifree
//
// SUMMARY
//  free a list, zero its reference
//
void list_ifree(list ** li)
  __attribute__((nonnull));

void list_init(list * restrict li)
  __attribute__((nonnull));

void list_initx(list * restrict li, size_t capacity, void (*free_fn)(void *))
  __attribute__((nonnull(1)));

void list_destroy(list * restrict li)
  __attribute__((nonnull));

/// list_recycle
//
// SUMMARY
//  reset the number of elements in the list - the allocation remains intact
//
void list_recycle(list * restrict li)
  __attribute__((nonnull));

/// list_truncate
//
// SUMMARY
//  set the size of the list
//
// PARAMETERS
//  len - new size of the list, 0 <= len <= li->l
//
void list_truncate(list * restrict li, size_t len)
  __attribute__((nonnull));

/// list_get
//
// SUMMARY
//  get an element from a list by index
//
void * list_get(const list * restrict li, int x)
  __attribute__((nonnull));

/// list_push
//
// SUMMARY
//  add an element to the end of the list
//
// PARAMETERS
//  li     - list
//  [item] - pointer to item
//
void list_push(list * restrict li, void * item)
  __attribute__((nonnull));

/// list_push_range
//
// SUMMARY
//  add items to the end of the list
//
// PARAMETERS
//  li           - list
//  items        - pointer to first item
//  len          - number of items
//
void list_push_range(list * restrict li, void * restrict items, size_t len)
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
void list_pop(list * restrict li)
  __attribute__((nonnull(1)));

/// list_unshift
//
// SUMMARY
//  add an element to the beginning of the list
//
// PARAMETERS
//  li         - list
//  item       - pointer to item
//
void list_unshift(list * restrict li, void * item)
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
//  len        - number of items
//
void list_unshift_range(list * restrict li, void * items, size_t len)
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
void list_shift(list * restrict li)
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
//
void list_insert(list * restrict li, size_t index, void * item)
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
//  len          - number of items
//
void list_insert_range(list * restrict li, size_t index, void * items, size_t len)
  __attribute__((nonnull(1, 3)));

/// list_put
//
// SUMMARY
//  update the element in the list at the specified index
//
// REMARKS
//  the existing element at the index is destroyed
//
// PARAMETERS
//  li         - list
//  index      - 0 <= index < li->l
//  item       - pointer to the element to place
//
void list_put(list * restrict li, size_t index, void * item)
  __attribute__((nonnull));

/// list_put_range
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
//  len          - number of items
//
void list_put_range(list * restrict li, size_t index, void * items, size_t len)
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
//
void list_delete(list * restrict li, size_t index)
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
void list_delete_range(list * restrict li, size_t index, size_t len)
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
void list_splice(list * dst, size_t dst_index, list * src, size_t src_index, int len)
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
void list_replicate(list * dst, size_t dst_index, list * src, size_t src_index, int len)
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
bool list_equal(list * A, list * B, int (*cmp_fn)(const void * A, const void * B, void *))
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
void list_sort(list * restrict li, int (*cmp_fn)(const void *A, const void *B, void *))
  __attribute__((nonnull));
