/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
