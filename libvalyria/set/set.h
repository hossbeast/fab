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
 set - dynamically resizing unordered collection of unique elements

REMARKS
 set stores references to objects - the put operation stores a reference

*/

#include "xapi.h"
#include "types.h"

#include "hashtable.h"

typedef struct set {
  size_t size;        // number of entries
  size_t table_size;  // allocated table size
  uint32_t hash;      // order invariant hash of elements
} set;

typedef uint32_t (*set_key_hash)(uint32_t h, const void * key, size_t sz);
typedef int (*set_key_cmp)(const void * A, size_t Asz, const void * key, size_t sz);

/// set_create
//
// SUMMARY
//  create an empty set
//
// PARAMETERS
//  s               - (returns) newly allocated set
//  [capacity]      - initial capacity, the minimum number of elements which can
//                    be set without rehashing
//  [hash_element]  - element hash function
//  [cmp_elements]  - element comparison function
//  [free_element]  - invoked with the element just before freeing it
//  [xfree_element] - invoked with the element just before freeing it
//
// REMARKS
//  the default hash/cmp functions consider two elements to be equal if they
//  refer to the same object - that is, if the element pointer and lengths are
//  equal
//
xapi set_create(set ** restrict s)
  __attribute__((nonnull));

xapi set_createx(
    set ** restrict s
  , size_t capacity
  , uint32_t (*hash_element)(uint32_t h, const void * element, size_t sz)
  , int (*cmp_elements)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void (*destroy_element)(void * element)
  , xapi (*xdestroy_element)(void * element)
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
// REMARKS
//  if an element with the same identity already exists, it is destroyed
//
// PARAMETERS
//  set      - set
//  element  - pointer to element
//  elementl - element length
//
xapi set_put(set * s, void * element, size_t element_len)
  __attribute__((nonnull));

/// set_store
//
// (idempotent) put an element in the set, get or create
//
xapi set_store(set * s, void * element, size_t element_len, void ** restrict elementp)
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
bool set_contains(const set * s, const void * key, size_t key_len)
  __attribute__((nonnull));

void * set_get(const set * s, const void * key, size_t key_len)
  __attribute__((nonnull));

bool set_get_element(const set * s, const void * key, size_t key_len, void * elp, size_t * restrict ell)
  __attribute__((nonnull(1, 2)));

void * set_search(const set * s, void *key, size_t key_len, set_key_hash hash, set_key_cmp cmp)
  __attribute__((nonnull));

/// set_equal
//
// SUMMARY
//  determine whether two sets contain the same elements
//
bool set_equal(set * const A, set * const B)
  __attribute__((nonnull));

/// set_recycle
//
// disassociate all elements - internal structures in the set remain allocated
//
xapi set_recycle(set * restrict s)
  __attribute__((nonnull));

/// set_splice
//
// SUMMARY
//  move elements from one set to another
//
xapi set_splice(set * restrict dst, set * restrict src)
  __attribute__((nonnull));

/// set_replicate
//
// SUMMARY
//  copy elements from one set to another
//
xapi set_replicate(set * restrict dst, set * restrict src)
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
xapi set_delete(set * restrict s, const void * key, size_t key_len)
  __attribute__((nonnull));

/// set_elements
//
// SUMMARY
//  returns an array of elements in the set
//
// PARAMETERS
//  s     - set
//  els   - (returns) list of elements to be freed by the caller
//  elsls - (returns) element sizes
//  elsl  - (returns) size of returned lists
//
xapi set_elements(const set * restrict s, void * restrict els, size_t ** restrict elsls, size_t * restrict elsl)
  __attribute__((nonnull));

/// set_table_element
//
// SUMMARY
//  get the element at the specified slot, if any
//
// PARAMETERS
//  s           - set
//  x           - slot index
//  element     - (returns) the element
//  element_len - (returns) length of the element
//
// RETURNS
//  true if an element was found at the specified slot
//
bool set_table_element(const set * restrict s, size_t x, void * element, size_t * element_len)
  __attribute__((nonnull(1)));

void * set_table_get(const set * restrict s, size_t x)
  __attribute__((nonnull));

xapi set_table_delete(set * restrict s, size_t x)
  __attribute__((nonnull));

#endif
