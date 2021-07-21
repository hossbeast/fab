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

#ifndef _VALYRIA_MAP_H
#define _VALYRIA_MAP_H

/*

MODULE
 map

SUMMARY
 dynamically resizing unordered collection of key/value pairs

REMARKS
 map stores values as references to objects - the set operation stores a reference

*/

#include "types.h"

typedef struct map {
  size_t size;
  size_t table_size;
} map;

/// map_create
//
// SUMMARY
//  create an empty map
//
// PARAMETERS
//  map           - (returns) newly allocated map
//  [free_value]  - invoked with the value just before freeing it (keys may not be managed in this way)
//  [xfree_value] - invoked with the value just before freeing it (keys may not be managed in this way)
//  [capacity]    - initial capacity, the minimum number of entries which can be set without rehashing
//
void map_create(map ** restrict m)
  __attribute__((nonnull));

void map_createx(
    map ** restrict m
  , size_t capacity
  , void (*free_value)(void * value)
)
  __attribute__((nonnull(1)));

/// map_xfree
//
// SUMMARY
//  free a map
//
void map_xfree(map * restrict map);

/// map_ixfree
//
// SUMMARY
//  free a map, zero its reference
//
void map_ixfree(map ** restrict map)
  __attribute__((nonnull));

/// map_put
//
// SUMMARY
//  create or update a mapping
//
// PARAMETERS
//  map         - map
//  key         - pointer to key
//  keyl        - key length
//  [value]     - pointer to value
//  [value_len] - value length
//
void map_put(map * restrict m, const void * restrict key, uint16_t keyl, void * value, size_t value_len)
  __attribute__((nonnull(1, 2)));

/// map_get
//
// SUMMARY
//  given a key, get a pointer to the associated value, or 0 if not found
//
// PARAMETERS
//  map  - map
//  key  - pointer to key
//  keyl - length of key
//
// SUMMARY
//  returns pointer to the stored value, or 0 if not found
//
void * map_get(const map * restrict m, const void * restrict key, uint16_t keyl)
  __attribute__((nonnull));

/// map_get_mapping
//
// SUMMARY
//  given a key, returns
//
// PARAMETERS
//  m           - map
//  key         - pointer to key
//  keyl        - length of key
//  [value]     - (returns) pointer to the value
//  [value_len] - (returns) value length
//
// RETURNS
//  true if a mapping exists with the specified key
//
bool map_get_mapping(const map * restrict m, const void * restrict key, uint16_t keyl, void * value, size_t * restrict value_len)
  __attribute__((nonnull(1, 2)));

bool map_equal(map * const restrict A, map * const restrict B)
  __attribute__((nonnull));

/// map_recycle
//
// disassociate all keys - internal structures in the map remain allocated
//
void map_recycle(map * restrict map)
  __attribute__((nonnull));

/// map_delete
//
// SUMMARY
//  remove an entry from the map, if any
//
// PARAMETERS
//  m    - map
//  key  - pointer to key
//  keyl - length of key
//
void map_delete(map * restrict m, const void * restrict key, uint16_t keyl)
  __attribute__((nonnull));

/// map_keys
//
// SUMMARY
//  returns a list of keys in the map
//
// PARAMETERS
//  m      - map
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
void map_keys(const map * restrict m, void * restrict keys, uint16_t * restrict keysl)
  __attribute__((nonnull));

/// map_values
//
// SUMMARY
//  returns a list of values in the map
//
// PARAMETERS
//  m      - map
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
void map_values(const map * restrict m, void * restrict values, size_t * restrict valuesl)
  __attribute__((nonnull));

/// map_table_key
//
// SUMMARY
//  get the key at the specified slot, if any
//
// PARAMETERS
//  m - map
//  x - slot index
//
const void * map_table_key(const map * restrict m, size_t x)
  __attribute__((nonnull));

/// map_table_value
//
// SUMMARY
//  get the value at the specified slot, if any
//
// PARAMETERS
//  m - map
//  x - slot index
//
void * map_table_value(const map * restrict m, size_t x)
  __attribute__((nonnull));

/// map_table_mapping
//
// SUMMARY
//  get the mapping at the specified slot, if any
//
// PARAMETERS
//  m    - map
//  x    - slot index
//  [k]  - (returns) key
//  [kl] - (returns) key length
//  [v]  - (returns) value
//  [vl] - (returns) value length
//
bool map_table_mapping(const map * restrict m, size_t x, const void ** restrict k, uint16_t * restrict kl, void * v, size_t * restrict vl)
  __attribute__((nonnull(1)));

/// map_splice
//
// SUMMARY
//  copy elements from one map to another
//
void map_splice(map * restrict dst, map * restrict src)
  __attribute__((nonnull));

/// map_replicate
//
// SUMMARY
//  copy elements from one map to another
//
void map_replicate(map * restrict dst, map * restrict src)
  __attribute__((nonnull));

#endif
