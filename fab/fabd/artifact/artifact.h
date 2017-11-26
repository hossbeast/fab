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

#ifndef FABD_ARTIFACT_H
#define FABD_ARTIFACT_H

/*

SUMMARY
 an artifact represents one of the final products of the build, the target of a module

*/

#include "types.h"
#include "xapi.h"

struct ff_node_artifact_pattern;
struct list;
struct node;

typedef struct artifact
{
  struct node * node;
  const char * variant;   // pointer into node->name->name
  size_t variant_len;
} artifact;

/// artifact_pattern_generate
//
// SUMMARY
//  create artifact nodes in a module as specified by a pattern
//
// PARAMETERS
//  pattern   - artifact pattern
//  base      - node to which new nodes are attached (directory node for the module)
//  artifacts - newly generated artifacts are appended to this list<artifact>
//
xapi artifact_pattern_generate(
    /* 1 */ const struct ff_node_artifact_pattern * restrict pattern
  , /* 2 */ struct node * restrict base
  , /* 3 */ struct list * restrict artifacts
)
  __attribute__((nonnull));

#endif
