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

#ifndef FABD_PATTERN_MATCH_H
#define FABD_PATTERN_MATCH_H

#include "xapi.h"
#include "types.h"

struct artifact;
struct ff_node_pattern;
struct node;

/// pattern_match
//
// SUMMARY
//  determine whether a node matches a pattern
//
// PARAMETERS
//  pattern  - pattern to match
//  n        - node to match
//  af       - artifact to match in the context of
//  matched  - (returns) true if the pattern matches
//  stem     - (returns) returns the stem of the match, if any
//  stem_len - (returns) returns the length of the stem of the match, if any
//
xapi pattern_match(
    /* 1 */ const struct ff_node_pattern * restrict pattern
  , /* 2 */ const struct node * restrict n
  , /* 3 */ const struct artifact * restrict af
  , /* 4 */ bool * restrict matched
  , /* 5 */ const char ** restrict stem
  , /* 6 */ uint16_t * restrict stem_len
)
  __attribute__((nonnull));

#endif
