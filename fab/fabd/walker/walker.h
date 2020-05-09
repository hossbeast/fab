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

#ifndef _WALKER_H
#define _WALKER_H

#include <fcntl.h>

#include "xapi.h"
#include "types.h"

struct node;
struct graph_invalidation_context;

/*
 * visit directory entries in a filesystem tree, creating graph nodes for each node as necessary
 *
 * either base is specified (if traversing an already-instantiated tree), or parent is specified
 * (node to which the tree is to be attached)
 *
 * [basep]      - (returns) base of tree rooted at abspath
 * [base]       - base of the tree rooted at abspath, if it already exists
 * [parent]     - parent of base to which it is attached when created
 * abspath      - absolute path to a base directory
 * [walk_id]    -
 * invalidation -
 */
xapi walker_descend(
    struct node ** restrict basep
  , struct node * restrict base
  , struct node * restrict parent
  , const char * restrict abspath
  , int walk_id
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(4, 6)));

/// walker_ascend
//
// SUMMARY
//  traverse upwards in a filesystem tree from a base node, visiting only nodes relevant for modules
//
// PARAMETERS
//  base      - node to ascend from
//  [walk_id] -
//
xapi walker_ascend(struct node * restrict base, int walk_id, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/// walker_descend_begin
//
// SUMMARY
//  allocate a walk scope, in which each node is visited at most once
//
int walker_descend_begin(void);

#endif
