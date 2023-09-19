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

#ifndef _VALYRIA_HASHTABLE_INTERNAL_H
#define _VALYRIA_HASHTABLE_INTERNAL_H

#include "types.h"
#include "hashtable.h"

#include "macros.h"

#define HT_OCCUPIED 0x01   /* this bucket contains an entry */
#define HT_DELETED  0x02   /* this bucket has been deleted */

struct hashtable_t;
typedef struct ht_operations {
  /// hash_entry
  //
  // SUMMARY
  //  hash an entry
  //
  // PARAMETRS
  //  ht    - hashtable
  //  entry - entry to be hashed
  //
  uint32_t (*hash_entry)(const struct hashtable_t * ht, void * entry);

  /// cmp_entries
  //
  // SUMMARY
  //  compare two entries
  //
  int (*compare_entries)(const struct hashtable_t * ht, void * A, void * B);

  /// delete_entry
  //
  // SUMMARY
  //  called when an entry is marked as deleted (delete, recycle)
  //
  // PARAMETRS
  //  ht    - hashtable
  //  entry - entry
  //
  void (*delete_entry)(const struct hashtable_t * ht, void * entry);

  /// destroy_entry
  //
  // SUMMARY
  //  called when an entry is destroyed (in hashtable_destroy)
  //
  // PARAMETRS
  //  ht    - hashtable
  //  entry - entry
  //
  void (*destroy_entry)(const struct hashtable_t * ht, void * entry);

  /// store_entry
  //
  // SUMMARY
  //   called when an entry is stored in a bucket
  //
  // PARAMETRS
  //  ht    - hashtable
  //  dst   - pointer to storage within a bucket of size ht->esz
  //  entry - pointer to the entry to store
  //  found - true if dst is a matching entry
  //
  void (*store_entry)(const struct hashtable_t * ht, void * dst, void * entry, bool empty);
} ht_operations;

typedef struct ht_bucket
{
  uint32_t attr;   // options and modifiers
  uint32_t h;      // hash
  char __attribute__((aligned(8))) p[];    // payload
} ht_bucket;

typedef struct hashtable_t {
  union {
    hashtable htx;
    struct {
      size_t size;
      size_t table_size;
      uint32_t hash;        // order invariant hash of entries
    };
  };

  size_t dirty_size;
  size_t lm;              // bitmask equal to table_size - 1
  size_t overflow_size;   // size at which to rehash
  size_t essz;            // entry storage size
  size_t esz;             // entry size
  ht_bucket * tab;        // bucket table

  // implementation
  ht_operations * ops;

  // user callbacks
  uint32_t (*hash_fn)(uint32_t h, const void * entry, size_t sz);
  int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz);
  void (*destroy_fn)(void * entry);
} hashtable_t;

STATIC_ASSERT(offsetof(hashtable, size) == offsetof(hashtable_t, size));
STATIC_ASSERT(offsetof(hashtable, table_size) == offsetof(hashtable_t, table_size));
STATIC_ASSERT(offsetof(hashtable, hash) == offsetof(hashtable_t, hash));

/// hashtable_init
//
// SUMMARY
//  initialize an already allocated hashtable
//
void hashtable_init(
    hashtable_t * restrict ht
  , size_t esz
  , size_t capacity
  , ht_operations * restrict ops
  , uint32_t (*hash_fn)(uint32_t h, const void * entry, size_t sz)
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
)
  __attribute__((nonnull(1)));

/// hashtable_destroy
//
// SUMMARY
//
//
void hashtable_xdestroy(hashtable_t * restrict ht)
  __attribute__((nonnull));

/// hashtable_grow
//
// SUMMARY
//  reallocate the hashtable at the next-larger power-of-two
//
void hashtable_grow(hashtable_t * restrict ht)
  __attribute__((nonnull));

/// hashtable_probe
//
// SUMMARY
//  search for an entry by linear probe
//
// PARAMETERS
//  ht    - hashtable
//  h     - entry hash
//  entry - pointer to the entry
//  [i]   - (returns) table index
//
// RETURNS
//   0 : found
//   1 : not found, and the first deleted slot in the probe sequence is returned in i
//  -1 : not found
//
int hashtable_probe(const hashtable_t * const restrict ht, uint32_t h, void * ent, size_t ent_sz, hashtable_cmp cmp, size_t * restrict i)
  __attribute__((nonnull(1, 3)));

/// hashtable_bucket_at
//
// SUMMARY
//  get the bucket at the specified table index
//
ht_bucket * hashtable_bucket_at(const hashtable_t * restrict ht, ht_bucket * restrict tab, size_t x)
  __attribute__((nonnull));

/// hashtable_table_bucket
//
// SUMMARY
//  get the occupied bucket at the specified table index, if any
//
ht_bucket * hashtable_table_bucket(const hashtable_t * restrict ht, size_t x)
  __attribute__((nonnull));

/// hashtable_store
//
// SUMMARY
//
//
void hashtable_store(hashtable_t * restrict ht, void * entry, ht_bucket ** bp)
  __attribute__((nonnull));

#endif
