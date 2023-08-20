/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 list.internal

*/

#include "list.h"
#include "array.internal.h"

#include "macros.h"

typedef struct value {
  void * p;
  size_t l;
} value;

typedef struct list_t {
  union {
    array_t ar;
    array arx;
    list lix;
    struct {
      size_t size;
    };
  };

  int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz);
  void (*free_fn)(void * item);
} list_t;

STATIC_ASSERT(offsetof(list, size) == offsetof(list_t, size));
STATIC_ASSERT(offsetof(list_t, size) == offsetof(array_t, size));
