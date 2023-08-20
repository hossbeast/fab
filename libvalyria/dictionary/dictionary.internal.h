/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
