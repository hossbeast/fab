/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef PATTERN_LOOKUP_H
#define PATTERN_LOOKUP_H

#include "xapi.h"
#include "types.h"

struct pattern;
struct selection;
struct channel;

#define PATTERN_LOOKUP_DIR       0x01 /* only match directories */
#define PATTERN_LOOKUP_MODULE    0x03 /* only match directories which have a module file */
#define PATTERN_LOOKUP_MODEL     0x05 /* only match directories which have a model file */

/*
 * Render a lookup pattern, and lookup a node for up each fragment. The lookup must yield exactly
 * one node for each fragment.
 *
 * ref    - reference pattern
 * attrs  - bitwise combo of PATTERN_LOOKUP_*
 * nodes  - found nodes added here
 */
xapi pattern_lookup(
    const struct pattern * restrict ref
  , uint32_t attrs
  , struct selection * restrict nodes
  , struct channel * restrict chan
)
  __attribute__((nonnull(1, 3)));

#endif
