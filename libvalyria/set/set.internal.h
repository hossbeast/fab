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

#ifndef _SET_INTERNAL_H
#define _SET_INTERNAL_H

#include "set.h"
#include "hashtable.internal.h"

#include "macros.h"

typedef struct element {
  void * p;
  size_t l;
} element;

typedef struct set_t {
  union {
    hashtable_t ht;
    hashtable htx;
    set sx;
    struct {
      size_t size;
      size_t table_size;
      uint32_t hash;
    };
  };

  // user callbacks
  void (*destroy_fn)(void * entry);
} set_t;

STATIC_ASSERT(offsetof(set, size) == offsetof(set_t, size));
STATIC_ASSERT(offsetof(set, table_size) == offsetof(set_t, table_size));
STATIC_ASSERT(offsetof(set, hash) == offsetof(set_t, hash));
STATIC_ASSERT(offsetof(set_t, size) == offsetof(hashtable_t, size));
STATIC_ASSERT(offsetof(set_t, table_size) == offsetof(hashtable_t, table_size));
STATIC_ASSERT(offsetof(set_t, hash) == offsetof(hashtable_t, hash));

#endif
