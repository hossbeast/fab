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

#ifndef _MAP_INTERNAL_H
#define _MAP_INTERNAL_H

#include "map.h"
#include "hashtable.internal.h"

typedef struct mapping {
  void * k;     // key
  uint16_t ka;  // allocated size
  uint16_t kl;  // length

  void * v;     // reference to value
  size_t vl;    // value length
} mapping;

typedef struct map_t {
  union {
    hashtable_t ht;
    hashtable htx;
    map mx;
    struct {
      size_t size;
      size_t table_size;
    };
  };

  // user callbacks
  void (*destroy_fn)(void * entry);
} map_t;

STATIC_ASSERT(offsetof(map, size) == offsetof(map_t, size));
STATIC_ASSERT(offsetof(map_t, size) == offsetof(hashtable_t, size));
STATIC_ASSERT(offsetof(map, table_size) == offsetof(hashtable_t, table_size));
STATIC_ASSERT(offsetof(map_t, table_size) == offsetof(hashtable_t, table_size));

#endif
