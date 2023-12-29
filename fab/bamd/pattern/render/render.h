/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct pattern;
struct bacon_value;

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

#define PATTERN_RENDER_LAST_SECTION_SKIP 1
#define PATTERN_RENDER_LAST_SECTION_ONLY 2

/*
 * internal to the pattern subsystem, used to convert lookup patterns into a list of fragments
 */
void pattern_render(
    const struct pattern * restrict pat
  , const struct bacon_value * restrict vars
  , pattern_render_result ** result
  , uint32_t flags
)
  __attribute__((nonnull(1, 3)));

static inline struct pattern_render_fragment * pattern_render_fragment_next(const struct pattern_render_fragment * fragment)
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
