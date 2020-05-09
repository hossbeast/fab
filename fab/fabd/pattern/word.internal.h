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

#ifndef FABD_PATTERN_WORD_INTERNAL_H
#define FABD_PATTERN_WORD_INTERNAL_H

/*

SUMMARY
 fab-file node

*/

#include "xapi.h"
#include "types.h"

#include "word.h"

struct yyu_location;
union pattern_segment;

xapi pattern_word_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , const char * restrict name
  , uint16_t name_len
)
  __attribute__((nonnull(1, 2, 3)));

#endif
