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

#ifndef FABD_PATTERN_BYTE_H
#define FABD_PATTERN_BYTE_H

#include "types.h"

struct yyu_location;
union pattern_segment;
struct narrator;

void pattern_byte_mk(
    union pattern_segment ** restrict pat
  , const struct yyu_location * restrict loc
  , uint8_t code
)
  __attribute__((nonnull));

void byte_say(uint8_t code, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
