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

#ifndef PATTERN_LOOKUP_INTERNAL_H
#define PATTERN_LOOKUP_INTERNAL_H

#include "lookup.h"

struct node;
struct yyu_location;
struct module;
struct selection;

/*
 * lookup nodes by path fragment - only used in test code
 */
xapi pattern_lookup_fragment(
    const char * restrict frag
  , uint16_t fragl
  , const char * restrict fname
  , const struct yyu_location * restrict loc
  , const struct module * restrict mod
  , const struct node * restrict base
  , uint32_t attrs
  , struct selection * restrict nodes
  , struct node ** restrict np
)
  __attribute__((nonnull(1)));

#endif
