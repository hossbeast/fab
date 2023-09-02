/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "narrator.h"

typedef struct bacon_narrator {
  narrator base;
  char *  s;      // buffer
  size_t  l;      // position
  size_t  a;      // buffer size
  narrator *next; // underlying narrator, (not owned)
} bacon_narrator;

extern struct narrator_vtable bacon_narrator_vtable;

narrator * bacon_narrator_init(bacon_narrator * restrict n)
  __attribute__((nonnull));

void bacon_narrator_destroy(bacon_narrator * restrict n)
  __attribute__((nonnull));

void bacon_narrator_configure(bacon_narrator * restrict n, narrator * restrict next)
  __attribute__((nonnull));

void bacon_narrator_reset(bacon_narrator * restrict n)
  __attribute__((nonnull));

/* propagate to the underlying narrator with a single write */
void bacon_narrator_flush(bacon_narrator * restrict n)
  __attribute__((nonnull));
