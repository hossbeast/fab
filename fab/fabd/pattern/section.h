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

#ifndef FABD_PATTERN_SECTION_H
#define FABD_PATTERN_SECTION_H

#include "xapi.h"
#include "types.h"

#include "section.h"
#include "pattern.h"

struct narrator;
struct pattern_section;
struct pattern_segments;
struct yyu_location;

xapi pattern_section_mk(
    struct pattern_section ** restrict rv
  , const struct yyu_location * restrict loc
  , enum pattern_nodeset nodeset
  , enum pattern_graph graph
  , enum pattern_axis axis
  , struct pattern_segments * restrict qualifiers
)
  __attribute__((nonnull(1, 2)));

xapi pattern_section_say(const struct pattern_section * section, bool first, struct narrator * restrict N)
  __attribute__((nonnull));

int pattern_sections_cmp(const struct pattern_section * A, const struct pattern_section * B);
int pattern_section_cmp(const struct pattern_section * A, const struct pattern_section * B);

#endif
