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
 meant to be used as secondary storage for the elements in the collection

*/

#include "xapi.h"
#include "types.h"
#include "hashtable.h"

struct mapping;

typedef struct map
{
  hashtable;

  void (*free_value)(void *);
  xapi (*xfree_value)(void *);
  void (*free_mapping)(struct mapping *);
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
xapi map_create(map ** restrict m)
  __attribute__((nonnull));

xapi map_createx(
    map ** restrict m
  , size_t capacity
  , void * free_value
  , void * xfree_value
)
  __attribute__((nonnull(1)));

/// map_xfree
//
// SUMMARY
//  free a map
//
xapi map_xfree(map * restrict map);

/// map_ixfree
//
// SUMMARY
//  free a map, zero its reference
//
xapi map_ixfree(map ** restrict map)
  __attribute__((nonnull));

/// map_set
//
// SUMMARY
//  add or update an element
//
// PARAMETERS
//  map     - map
//  key     - pointer to key
//  keyl    - key length > 0
//  [value] - pointer to value
//
xapi map_set(map * restrict m, const void * key, size_t keyl, void * restrict value)
  __attribute__((nonnull(1, 2)));

/// map_get
//
// SUMMARY
//  given a key, returns pointer to associated value, or 0 if not found
//
// PARAMETERS
//  map  - map
//  key  - pointer to key
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
void * map_get(const map * restrict m, const void * restrict key, size_t keyl)
  __attribute__((nonnull));

/// map_recycle
//
// disassociate all keys - internal structures in the map remain allocated
//
xapi map_recycle(map * restrict map)
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
xapi map_delete(map * restrict m, const void * restrict key, size_t keyl)
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
xapi map_keys(const map * restrict m, const char *** restrict keys, size_t * restrict keysl)
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
xapi map_values(const map * restrict m, void * restrict values, size_t * restrict valuesl)
  __attribute__((nonnull));

/// map_keyat
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

/// map_valueat
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

#endif
