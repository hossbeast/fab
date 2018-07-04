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

#ifndef _VALYRIA_HASHTABLE_H
#define _VALYRIA_HASHTABLE_H

/*

MODULE
  hashtable

dynamically resizing unordered collection of unique entries

*/

#include "xapi.h"
#include "types.h"

struct hashtable;
typedef uint32_t (*ht_hash_fn)(struct hashtable * ht, uint32_t, void *, size_t);
typedef int (*ht_cmp_fn)(struct hashtable * ht, void *, size_t, void *, size_t);
typedef void (*ht_free_fn)(struct hashtable * ht, void *);
typedef xapi (*ht_xfree_fn)(struct hashtable * ht, void *);

struct ht_bucket;
typedef struct hashtable
{
  size_t   count;           // number of active buckets
  size_t   table_size;      // allocated table size, a power of two
  uint32_t hash;
  size_t   lm;              // bitmask equal to table_size - 1
  size_t   overflow_count;  // size at which to rehash
  size_t   esz;             // entry size
  struct ht_bucket * tab;          // bucket table

  ht_hash_fn hash_fn;
  ht_cmp_fn cmp_fn;
  ht_free_fn free_fn;
  ht_xfree_fn xfree_fn;
} hashtable;

/// hashtable_create
//
// SUMMARY
//  allocate a hashtable
//
// PARAMETERS
//  ht            - (returns) newly allocated hashtable
//  esz           - entry size > 0
//  [capacity]    - initial capacity, the minimum number of entries which can be set without rehashing
//  [free_value]  - invoked with the entry just before freeing it
//  [xfree_value] - invoked with the entry just before freeing it
//
xapi hashtable_create(hashtable ** restrict ht, size_t esz)
  __attribute__((nonnull(1)));

xapi hashtable_createx(
    hashtable ** restrict ht
  , size_t esz
  , size_t capacity
  , ht_hash_fn hash_fn
  , ht_cmp_fn cmp_fn
  , ht_free_fn free_fn
  , ht_xfree_fn xfree_fn
)
  __attribute__((nonnull(1)));

/// hashtable_xfree
//
// SUMMARY
//  free a hashtable
//
xapi hashtable_xfree(hashtable * restrict ht);

xapi hashtable_ixfree(hashtable ** restrict ht)
  __attribute__((nonnull));

/// hashtable_put
//
// SUMMARY
//  add an entry to the hashtable
//
// PARAMETERS
//  hashtable - hashtable
//  entry     - pointer to entry
//
xapi hashtable_put(hashtable * const restrict ht, void * restrict entry)
  __attribute__((nonnull));

/// hashtable_contains
//
// SUMMARY
//  determine whether an entry is in the hashtable
//
bool hashtable_contains(hashtable * const restrict ht, void * restrict entry)
  __attribute__((nonnull));

/// hashtable_equal
//
// SUMMARY
//  determine whether two hashtables contain the same set of active entries
//
bool hashtable_equal(hashtable * const restrict A, hashtable * const restrict B)
  __attribute__((nonnull));

/// hashtable_recycle
//
// disassociate all entries - internal structures remain allocated
//
xapi hashtable_recycle(hashtable * const restrict ht)
  __attribute__((nonnull));

/// hashtable_delete
//
// SUMMARY
//  remove an entry from the hashtable, if any
//
// PARAMETERS
//  m    - hashtable
//  key  - pointer to key
//
xapi hashtable_delete(hashtable * restrict m, void * restrict entry)
  __attribute__((nonnull));

/// hashtable_entries
//
// SUMMARY
//  returns a list of entries in the hashtable
//
// PARAMETERS
//  m      - hashtable
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
xapi hashtable_entries(const hashtable * restrict m, void * restrict entries, size_t * entriesl)
  __attribute__((nonnull));

/// hashtable_table_entry
//
// SUMMARY
//  get a pointer to the entry at the specified table index, if any
//
void * hashtable_table_entry(const hashtable * restrict ht, size_t x)
  __attribute__((nonnull));

#endif
