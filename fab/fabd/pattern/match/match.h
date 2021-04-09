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

#ifndef _PATTERN_MATCH_H
#define _PATTERN_MATCH_H

#include "xapi.h"
#include "types.h"

struct pattern;
struct fsent;

/*
 * Determine whether an fsent matches a match pattern
 */
xapi pattern_match(
    const struct pattern * restrict pat
  , const struct fsent * restrict dirnode
  , const char * restrict label
  , uint16_t label_len
  , bool * restrict matched
)
  __attribute__((nonnull));

#endif
