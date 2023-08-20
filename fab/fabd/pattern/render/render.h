/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct pattern;

typedef struct pattern_render_fragment {
  uint16_t len;     /* length of this fragment */
  char text[];      /* fragment text */
} pattern_render_fragment;
STATIC_ASSERT(sizeof(pattern_render_fragment) == 2);

// sequence of path fragments rendered from a lookup pattern
typedef struct pattern_render_result {
  uint16_t size;      /* number of path fragments */
  pattern_render_fragment fragments[];
} pattern_render_result;
STATIC_ASSERT(sizeof(pattern_render_result) == 2);

void pattern_render(const struct pattern * restrict pat, pattern_render_result ** result)
  __attribute__((nonnull));

static inline struct pattern_render_fragment * pattern_render_fragment_next(struct pattern_render_fragment * fragment)
{
  char * next;

  next = (char*)(fragment + 1);       // fragment struct
  next += fragment->len;              // fragment text
  next += 1;                          // trailing null
  if ((fragment->len & 1) == 0) {
    next += 1;                        // padding
  }

  return (pattern_render_fragment*)next;
}
