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

#ifndef _VALYRIA_MULTIMAP_H
#define _VALYRIA_MULTIMAP_H

/*

MODULE
 multimap

SUMMARY
 dynamically resizing unordered collection of key/value mappings, where duplicate values are
 retained

REMARKS
 meant to be used as secondary storage for the elements in the collection

*/

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"
#include "types.h"

struct multimap;
typedef struct multimap multimap;

/// multimap_create
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
xapi multimap_create(multimap ** restrict m)
  __attribute__((nonnull));

xapi multimap_createx(
    multimap ** restrict m
  , void * free_value
  , void * xfree_value
  , size_t capacity
)
  __attribute__((nonnull(1)));

/// multimap_xfree
//
// SUMMARY
//  free a multimap
//
xapi multimap_xfree(multimap * restrict map);

/// multimap_ixfree
//
// SUMMARY
//  free a map, zero its reference
//
xapi multimap_ixfree(multimap ** restrict map)
  __attribute__((nonnull));

/// multimap_set
//
// SUMMARY
//  add an element
//
// PARAMETERS
//  map     - map
//  key     - pointer to key
//  keyl    - key length
//  [value] - pointer to value
//
xapi multimap_set(multimap * restrict m, const void * restrict key, size_t keyl, void * restrict value)
  __attribute__((nonnull(1, 2)));

/// multimap_get
//
// SUMMARY
//  given a key, returns pointer to associated value, or 0 if not found
//
// PARAMETERS
//  map     - map
//  key     - pointer to key
//  keyl - length of key
//
// EXAMPLE
//  int k = 15;
//  int* v = 0;
//  if((v = map_get(map, MM(k))) == 0)
//    /* key is not set */
//
// SUMMARY
//  returns pointer to the stored value, or 0 if not found
//
xapi multimap_get(
    /* 1 */ const multimap * restrict m
  , /* 2 */ const void * restrict key
  , /* 3 */ size_t keyl
  , /* 4 */ void * restrict tmp
  , /* 5 */ size_t * restrict tmpsz
  , /* 6 */ void * restrict vals
  , /* 7 */ size_t * restrict valsl
)
  __attribute__((nonnull(1, 2, 7)));

/// multimap_recycle
//
// SUMMARY
//  disassociate all keys - internal structures in the multimap remain allocated
//
xapi multimap_recycle(multimap * restrict map)
  __attribute__((nonnull));

/// multimap_delete
//
// SUMMARY
//  remove an entry from the map, if any
//
// PARAMETERS
//  m    - map
//  key  - pointer to key
//  keyl - length of key
//
xapi multimap_delete(multimap * restrict m, const void * restrict key, size_t keyl)
  __attribute__((nonnull));

/// multimap_size
//
// SUMMARY
//  get the number of keys in the multimap
//
size_t multimap_size(const multimap * restrict m)
  __attribute__((nonnull));

/// multimap_keys
//
// SUMMARY
//  returns a list of keys in the multimap
//
// PARAMETERS
//  m      - map
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
xapi multimap_keys(const multimap * restrict m, const char *** restrict keys, size_t * restrict keysl)
  __attribute__((nonnull));

/// multimap_values
//
// SUMMARY
//  returns a list of values in the multimap
//
// PARAMETERS
//  m      - map
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
xapi multimap_values(const multimap * restrict m, void * restrict values, size_t * restrict valuesl)
  __attribute__((nonnull));

/// multimap_table_size
//
// SUMMARY
//  get the size of the map tables for use with map_keyat / map_valueat
//
// PARAMETERS
//  m - map
//
size_t multimap_table_size(const multimap * restrict m)
  __attribute__((nonnull));

/// multimap_table_key
//
// SUMMARY
//  get the key at the specified slot, if any
//
// PARAMETERS
//  m - map
//  x - slot index
//
const void * multimap_table_key(const multimap * restrict m, size_t x)
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
void * multimap_table_value(const multimap * restrict m, size_t x)
  __attribute__((nonnull));

#undef restrict
#endif
