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

#ifndef FABD_PATTERN_MATCH_INTERNAL_H
#define FABD_PATTERN_MATCH_INTERNAL_H

#include "pattern/match.h"

struct ffn_bydir_walk;

/// pattern_match_context
//
// SUMMARY
//  dynamic matching context
//
// MEMBERS
//  node        - node being matched
//  name_offset - offset within the name of the node up to which the match has succeeded
//  variant     - string to use when evaluating whether a variant node matches
//  variant_len - variant string length
//  stem        - (returns) the stem, if any
//  stem_len    - (returns) stem length
//
typedef struct pattern_match_context
{
  const struct node * restrict node;
  uint16_t name_offset;
  const char * restrict variant;
  uint16_t variant_len;
  const char * restrict stem;
  uint16_t stem_len;
} pattern_match_context;

/// pattern_match_node
//
// SUMMARY
//  match a pattern against the name of a node
//
// PARAMETERS
//  context - dynamic pattern matching context
//  walk    - node to match on
//
xapi pattern_part_match(pattern_match_context * restrict context, struct ffn_bydir_walk * restrict walk)
  __attribute__((nonnull));

#endif
