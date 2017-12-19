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

/// walker_walk
//
// SUMMARY
//  visit directory entries in a filesystem tree, creating graph nodes for each node as necessary
//
// PARAMETERS
//  [root]     - (returns) base of the tree rooted at abspath
//  [base]     - base of the tree rooted at abspath, if any
//  [ancestor] - if base was null, and a new node is created for the tree rooted at abspath, and ancestor
//               is not null, then the new node is attached to the graph as a child of ancestor
//  abspath    - absolute path to a directory to traverse
//  walk_id    -
//
xapi walker_walk(
    struct node ** restrict root
  , struct node * restrict base
  , struct node * restrict ancestor
  , const char * restrict abspath
  , int walk_id
)
  __attribute__((nonnull(4)));

int walker_walk_begin(void);

#endif
