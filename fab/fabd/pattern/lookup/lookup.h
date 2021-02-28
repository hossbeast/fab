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

struct fsent;
struct pattern;
struct selection;
struct module;

#define PATTERN_LOOKUP_DIR       0x01 /* only match directories */
#define PATTERN_LOOKUP_MODULE    0x03 /* only match directories with a module file */
#define PATTERN_LOOKUP_MODEL     0x05 /* only match directories with a model file */

/*
 * Render a lookup or reference pattern, and lookup a node for up each fragment. For each fragment,
 * if graph lookup results in a number of matches != 1 then an error is raised. Note that a
 * reference pattern can only have a single fragment.
 *
 * ref    - reference pattern
 * attrs  - bitwise combo of PATTERN_LOOKUP_*
 * nodes  - found nodes added here
 */
xapi pattern_lookup(
    const struct pattern * restrict ref
  , uint32_t attrs
  , struct selection * restrict nodes
  , char * restrict err
  , size_t errsz
  , uint16_t * restrict errlen
)
  __attribute__((nonnull(1, 3, 4, 6)));

#endif
