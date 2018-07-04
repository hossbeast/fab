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

#define HT_OCCUPIED 0x01   /* this bucket contains an entry */
#define HT_DELETED  0x02   /* this bucket has been deleted */

typedef struct ht_bucket
{
  uint8_t  attr;   // options and modifiers
  uint32_t h;      // hash
  char     p[];    // payload
} ht_bucket;

/// hashtable_init
//
// SUMMARY
//  initialize an already allocated hashtable
//
xapi hashtable_init(
    hashtable * restrict ht
  , size_t esz
  , size_t capacity
  , ht_hash_fn hash_fn
  , ht_cmp_fn cmp_fn
  , ht_free_fn free_fn
  , ht_xfree_fn xfree_fn
)
  __attribute__((nonnull(1)));

/// hashtable_destroy
//
// SUMMARY
//  
//
xapi hashtable_xdestroy(hashtable * restrict ht)
  __attribute__((nonnull));

/// hashtable_grow
//
// SUMMARY
//  reallocate the hashtable at the next-larger power-of-two
//
xapi hashtable_grow(hashtable * restrict ht)
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
int hashtable_probe(hashtable * const restrict ht, uint32_t h, void * restrict entry, size_t * restrict i)
  __attribute__((nonnull(1, 3)));

/// hashtable_bucket_at
//
// SUMMARY
//  get the bucket at the specified table index
//
ht_bucket * hashtable_bucket_at(const hashtable * restrict ht, ht_bucket * restrict tab, size_t x)
  __attribute__((nonnull));

void hashtable_set_entry(hashtable * ht, ht_bucket * restrict b, uint32_t h, void * entry)
  __attribute__((nonnull));

#endif
