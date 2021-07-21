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

#ifndef _VALYRIA_DICTIONARY_INTERNAL_H
#define _VALYRIA_DICTIONARY_INTERNAL_H

#include "dictionary.h"
#include "hashtable.internal.h"

#include "macros.h"

typedef struct entry {
  bool val;     // has-a-value
  void * k;     // key
  struct {
    uint16_t ka;  // key allocated size
    uint16_t kl;  // key length
  } __attribute__((aligned(8)));

  char v[];     // the value
} entry;

typedef struct dictionary_t {
  union {
    hashtable_t ht;
    hashtable htx;
    dictionary dx;
    struct {
      size_t size;
      size_t table_size;
    };
  };

  size_t vsz;     // value size
  size_t vssz;    // value storage size

  // user callbacks
  void (*destroy_fn)(void * entry);
} dictionary_t;

STATIC_ASSERT(offsetof(dictionary, size) == offsetof(dictionary_t, size));
STATIC_ASSERT(offsetof(dictionary_t, size) == offsetof(hashtable_t, size));
STATIC_ASSERT(offsetof(dictionary, table_size) == offsetof(hashtable_t, table_size));
STATIC_ASSERT(offsetof(dictionary_t, table_size) == offsetof(hashtable_t, table_size));

#endif
