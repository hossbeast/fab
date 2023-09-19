/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#ifndef _PATTERN_RENDER_H
#define _PATTERN_RENDER_H

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

#endif
