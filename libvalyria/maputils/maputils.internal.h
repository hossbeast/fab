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

#ifndef _VALYRIA_MAPUTILS_INTERNAL_H
#define _VALYRIA_MAPUTILS_INTERNAL_H

/*

MODULE
 maputils

SUMMARY
 implementation utilities for map, dictionary, and multimap

*/

#include <sys/types.h>

#include "maputils.h"

/*
 * Default capacity, the minimum number of entries which can be set without rehashing
 */
#define DEFAULT_CAPACITY  100

/*
 * Maximum proportion of used entries to allocated table size
 *
 * valid values : 0 < saturation <= 1
 *
 * smaller values trade space for time because sparser tables mean shorter probe sequences
 */
#define MAX_SATURATION  0.45f      /* for 100-sized table, reallocate at 45 keys */

#define MAP_PRIMARY     0x01    /* primary storage of the values in the map */
#define MAP_SECONDARY   0x02    /* not the primary storage of the values in the map */
#define MAP_KEY_DELETED 0x01    /* this key has been deleted */

#define VALUE(m, x) ({                                \
  void * val;                                         \
  if(MAPATTR_PRIMARY(m))                              \
  {                                                   \
    val = (m)->tv + ((x) * VALUE_SIZE(m));            \
  }                                                   \
  else                                                \
  {                                                   \
    val = *(char**)((m)->tv + ((x) * VALUE_SIZE(m))); \
  }                                                   \
  val;                                                \
})

/// hashkey
//
// SUMMARY
//  compute a hash for the key
//
// REMARKS
//  the jenkins hash function : http://en.wikipedia.org/wiki/Jenkins_hash_function
//
size_t maputils_hashkey(const char * restrict k, size_t kl, size_t lm)
  __attribute__((nonnull));
  
// round up to the next highest power of 2
static inline size_t roundup2(size_t x)
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;

  return x;
}

#endif
