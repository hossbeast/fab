/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef PATTERN_INTERNAL_H
#define PATTERN_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "pattern.h"

#include "macros.h"

struct narrator;
struct yyu_location;
struct narrator;
struct pattern_section;
union pattern_segment;

xapi pattern_segment_init(union pattern_segment * restrict n, const pattern_segment_vtable * restrict vtab, const yyu_location * restrict loc)
  __attribute__((nonnull));

xapi pattern_mk(
    struct pattern ** restrict rv
  , const struct yyu_location * restrict loc
  , struct pattern_section * restrict sections
)
  __attribute__((nonnull));

xapi pattern_segments_mk(
    struct pattern_segments ** restrict rv
  , const struct yyu_location * restrict loc
  , pattern_qualifier_type qualifier_type
  , union pattern_segment * restrict segments
)
  __attribute__((nonnull));

xapi pattern_attrs_say(uint16_t attrs, struct narrator * restrict N)
  __attribute__((nonnull));

xapi pattern_segments_say(const pattern_segments * segment_list, bool only, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
