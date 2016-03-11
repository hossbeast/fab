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

#ifndef _MAP_H
#define _MAP_H

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

struct map;
typedef struct map map;

#ifndef MAP_VALUE_TYPE
# define MAP_VALUE_TYPE void
#endif

#define MAP_PRIMARY     0x01    /* primary storage of the values in the map */
#define MAP_SECONDARY   0x02    /* not the primary storage of the objects in the map */

#define restrict __restrict

/// map_create
//
// create an empty map
//
// PARAMETERS
//  map         - (returns) newly allocated map
//  vsz         - value size > 0
//  destructor  - invoked with key/value just before freeing their associated memory
//  attr        - bitwise combination of MAP_* options and modifiers
//  [capacity]  - initial capacity, the minimum number of entries which can be set without rehashing
//
// REMARKS
//  either attr & MAP_PRIMARY and vsz != 0, or attr & MAP_SECONDARY and vsz == 0
//
xapi map_create(map ** const restrict m, size_t vsz, void (*destructor)(const char *, MAP_VALUE_TYPE *), uint32_t attr)
  __attribute__((nonnull(1)));

xapi map_createx(map ** const restrict m, size_t vsz, void (*destructor)(const char *, MAP_VALUE_TYPE *), uint32_t attr, size_t capacity)
  __attribute__((nonnull(1)));

/// map_free
//
// SUMMARY
//  free a map with free semantics
//
void map_free(map * const restrict map);

/// map_xfree
//
// SUMMARY
//  free a map with xfree semantics
//
void map_xfree(map ** const restrict map)
  __attribute__((nonnull));

/// map_add
//
// SUMMARY
//  set a key/value pair on a MAP_PRIMARY map
//
// PARAMETERS
//  map   - map
//  key   - pointer to key
//  keyl  - key length
//  rv    - (returns) pointer to value
//
xapi map_add(map * const restrict m, const char * const restrict key, size_t keyl, MAP_VALUE_TYPE * const * const restrict rv)
  __attribute__((nonnull));

/// map_set
//
// SUMMARY
//  set a key/value pair on a MAP_SECONDARY map
//
// PARAMETERS
//  map    - map
//  key    - pointer to key
//  keyl   - key length
//  value  - pointer to value
//
xapi map_set(map * const restrict m, const char * const restrict key, size_t keyl, MAP_VALUE_TYPE * const restrict value)
  __attribute__((nonnull));

/// map_get
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
MAP_VALUE_TYPE * map_get(const map * const restrict m, const char * const restrict key, size_t keyl)
  __attribute__((nonnull));

/// map_clear
//
// disassociate all keys - internal structures in the map remain allocated
//
void map_clear(map * const restrict map)
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
// RETURNS
//  boolean value indicating whether an entry was found and removed
//
int map_delete(map * const restrict m, const char * const restrict key, size_t keyl)
  __attribute__((nonnull));

/// map_size
//
// SUMMARY
//  get the number of entries in the map
//
size_t map_size(const map * const restrict m)
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
// RETURNS
//  xapi semantics
//
xapi map_keys(const map * const restrict m, const char *** const restrict keys, size_t * const restrict keysl)
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
// RETURNS
//  xapi semantics
//
xapi map_values(const map * const restrict m, MAP_VALUE_TYPE *** restrict values, size_t * const restrict valuesl)
  __attribute__((nonnull));

/// map_table_size
//
// SUMMARY
//  get the size of the map tables for use with map_keyat / map_valueat
//
// PARAMETERS
//  m - map 
//
size_t map_table_size(const map * const restrict m)
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
const char * map_table_key(const map * const restrict m, size_t x)
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
MAP_VALUE_TYPE * map_table_value(const map * const restrict m, size_t x)
  __attribute__((nonnull));

#undef restrict
#endif
