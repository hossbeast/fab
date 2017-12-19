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

#ifndef FABD_PATTERN_GENERATE_H
#define FABD_PATTERN_GENERATE_H

#include "types.h"
#include "xapi.h"

struct artifact;
struct ff_node_pattern;
struct list;
struct node;
struct map;

/// pattern_generate
//
// SUMMARY
//  create nodes as specified by a pattern
//
// PARAMETERS
//  pattern             - pattern
//  base                - node to which new nodes are attached (directory node for the module)
//  [scope]             - 
//  af                  - artifact to generate in the context of
//  [stem]              - stem from a previous match
//  [stem_len]          - stem length
//  results             - newly created nodes are appended to this list
//  generating_artifact - true if generating from an artifact pattern
//
xapi pattern_generate(
    /* 1 */ const struct ff_node_pattern * restrict pattern
  , /* 2 */ struct node * restrict base
  , /* 3 */ struct map * restrict scope
  , /* 4 */ const struct artifact * restrict af
  , /* 5 */ const char * restrict stem
  , /* 6 */ uint16_t stem_len
  , /* 7 */ struct list * restrict nodes
  , /* 8 */ bool generating_artifact
)
  __attribute__((nonnull(1, 2, 7)));

#endif
