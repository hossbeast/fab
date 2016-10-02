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

#ifndef _MAP_DEF_H
#define _MAP_DEF_H

/*

MODULE
 map.def

SUMMARY
 low-level map create / mutate apis

*/

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

#define MAP_PRIMARY     0x01    /* primary storage of the values in the map */
#define MAP_SECONDARY   0x02    /* not the primary storage of the values in the map */

typedef struct
{
  int     d;      // deleted (boolean)
  size_t  a;      // allocated
  size_t  l;      // length
  char    p[];    // payload
} __attribute__((packed)) key;

struct map
{
  size_t      size;           // number of active entries
  uint32_t    attr;           // options and modifiers
  size_t      vsz;            // for MAP_PRIMARY, size of values
  void (*free_value)(void *);
  xapi (*xfree_value)(void *);

  size_t      table_size;     // table length, in elements (always a power of two)
  size_t      overflow_size;  // size at which to rehash
  size_t      lm;             // bitmask equal to table_size - 1
  key **      tk;             // key table
  char *      tv;             // value table
};

#define restrict __restrict

/// map_allocate
//
// SUMMARY
//  create an empty map
//
// PARAMETERS
//  map           - (returns) newly allocated map
//  attr          - bitwise combination of MAP_* options and modifiers
//  [vsz]         - value size > 0
//  [free_value]  - invoked with the value just before freeing it (keys may not be managed in this way)
//  [xfree_value] - invoked with the value just before freeing it (keys may not be managed in this way)
//  [capacity]    - initial capacity, the minimum number of entries which can be set without rehashing
//
// REMARKS
//  either attr & MAP_PRIMARY and vsz != 0, or attr & MAP_SECONDARY and vsz == 0
//
xapi map_allocate(
    struct map ** const restrict m
  , uint32_t attr
  , size_t vsz
  , void * free_value
  , void * xfree_value
  , size_t capacity
)
  __attribute__((nonnull(1)));

/// map_put
//
// SUMMARY
//  create or update a key/value pair mapping
//
// PARAMETERS
//  map   - map
//  key   - pointer to key
//  keyl  - key length
//  [r]   - pointer to value
//  [rv]  - (returns) pointer to map-owned value
//
xapi map_put(struct map * const restrict m, const char * const restrict key, size_t keyl, void * r, void * const * const restrict rv)
  __attribute__((nonnull(1,2)));

#undef restrict
#endif
