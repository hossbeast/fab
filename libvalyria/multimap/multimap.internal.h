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

#ifndef _VALYRIA_MULTIMAP_INTERNAL_H
#define _VALYRIA_MULTIMAP_INTERNAL_H

#include "types.h"

#include "multimap.h"

#include "macros.h"

typedef struct
{
  uint8_t attr;
  uint32_t h;

  size_t first;   // index of the first entry in the set
  size_t next;    // index of the next entry in the set
  size_t last;    // index of the last entry in the set

  struct {      // first entry in the set
    size_t n;     // number of values in the set
    size_t a;     // allocated size of p
    size_t l;     // length of the key
    char   p[];
  };
} multimap_key;

typedef struct multimap_t {
  union {
    multimap mx;
    struct {
      size_t size;
      size_t table_size;
    };
  };

  size_t overflow_size;       // size at which to rehash
  size_t lm;                  // bitmask equal to table_size - 1

  void (*free_value)(void *);

  multimap_key ** tk;  // key table
  char * tv;           // value table
} multimap_t;

STATIC_ASSERT(offsetof(multimap, size) == offsetof(multimap_t, size));
STATIC_ASSERT(offsetof(multimap, table_size) == offsetof(multimap_t, table_size));

void multimap_rehash(multimap_t * restrict m)
  __attribute__((nonnull));

#endif
