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

#ifndef FABD_RULE_H
#define FABD_RULE_H

/*

SUMMARY
 

*/

#include "xapi.h"
#include "types.h"

struct node;
struct list;
struct artifact;

/// rules_apply
//
// SUMMARY
//  Apply rules to add nodes and edges to the graph. Rules are applied iteratively, starting with
//  the artifact, and proceeding to its consequents, to their consequents, and so on.
//
// PARAMETERS
//  rules        - generally applicable rules for the module
//  base         - directory node for the base of the module
//  af           - artifact to begin from
//  traversal_id - restrict the traversal to nodes not yet visited with this id
//
xapi rules_apply(
    const struct list * restrict rules
  , struct node * restrict module_node
  , struct artifact * restrict af
  , int traversal_id
)
  __attribute__((nonnull));

#endif
