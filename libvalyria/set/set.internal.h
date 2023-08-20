/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
