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

#ifndef FABD_PATTERN_GROUP_H
#define FABD_PATTERN_GROUP_H

#include "xapi.h"
#include "types.h"

struct yyu_location;
union pattern_segment;
struct pattern_segments;

xapi pattern_group_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , struct pattern_segments * restrict chain
  , const char * restrict name
  , uint8_t name_len
  , uint16_t group_number
)
  __attribute__((nonnull(1, 2)));

#endif
