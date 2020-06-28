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
 hashtable - dynamically resizing unordered collection of unique entries

REMARKS
 hashtable stores objects - the put operation makes a copy

*/

#include "xapi.h"
#include "types.h"

typedef struct hashtable {
  size_t size;        // number of entries
  size_t table_size;  // allocated table size
  uint32_t hash;      // order invariant hash of entries
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
//  [destroy_entry]  - invoked with the entry just before freeing it
//  [xdestroy_entry] - invoked with the entry just before freeing it
//
xapi hashtable_create(hashtable ** restrict ht, size_t esz)
  __attribute__((nonnull(1)));

xapi hashtable_createx(
    hashtable ** restrict ht
  , size_t esz
  , size_t capacity
  , uint32_t (*hash_entry)(uint32_t h, const void * entry, size_t sz)
  , int (*cmp_entry)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void (*destroy_entry)(void * entry)
  , xapi (*xdestroy_entry)(void * entry)
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
// REMARKS
//  destroys an existing entry, if any
//
// PARAMETERS
//  hashtable - hashtable
//  entry     - pointer to entry
//
xapi hashtable_put(hashtable * const restrict ht, void * restrict entry)
  __attribute__((nonnull));

/// hashtable_get
//
// SUMMARY
//  get an entry in the hashtable
//
// PARAMETERS
//  ht    - hashtable
//  entry - pointer to entry
//
// RETURNS
//  pointer to entry, if any
//
void * hashtable_get(const hashtable * const restrict ht, void * restrict entry)
  __attribute__((nonnull));

static inline bool __attribute__((nonnull)) hashtable_contains(const hashtable * const restrict ht, void * restrict entry)
{
  return hashtable_get(ht, entry) ? true : false;
}

typedef uint32_t (*hashtable_hash)(uint32_t h, const void * key, size_t sz);
typedef int (*hashtable_cmp)(const void * A, const void * key, size_t sz);

void * hashtable_search(const hashtable * restrict ht, void * key, size_t key_len, hashtable_hash, hashtable_cmp cmp)
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

/// hashtable_splice
//
// SUMMARY
//  move entries from one hashtable to another
//
xapi hashtable_splice(hashtable * dst, hashtable * src)
  __attribute__((nonnull));

/// hashtable_replicate
//
// SUMMARY
//  copy entries from one hashtable to another
//
xapi hashtable_replicate(hashtable * dst, hashtable * src)
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

xapi hashtable_table_delete(hashtable * restrict ht, size_t x)
  __attribute__((nonnull));

#endif
