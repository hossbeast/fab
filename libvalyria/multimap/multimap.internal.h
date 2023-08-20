/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
