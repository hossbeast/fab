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

#ifndef _FABD_NODE_OPERATIONS_H
#define _FABD_NODE_OPERATIONS_H

#include "xapi.h"

struct edge;
struct filesystem;
struct node;
struct vertex;
struct operations_dispatch;

enum relation_type;

/// node_connect
//
// SUMMARY
//  create an edge in the graph
//
// PARAMETERS
//  g      - graph
//  parent - parent
//  n      - node
//
xapi node_connect(struct node * restrict above, struct node * restrict below, enum relation_type relation)
  __attribute__((nonnull));

/// node_disconnect
//
// SUMMARY
//  remove an edge between two nodes, if any
//
xapi node_disconnect(struct node * restrict A, struct node * restrict B)
  __attribute__((nonnull));

/// node_disintegrate_fs
//
// SUMMARY
//  unlink a subgraph from the filesystem subgraph
//
// PARAMETERS
//  g         - supergraph
//  e         - edge to begin with
//  traversal - id of an open traversal
//
xapi node_disintegrate_fs(struct edge * restrict e, int traversal)
  __attribute__((nonnull));

/// node_invalidate
//
// SUMMARY
//  mark a node and all of its consumers (upward) as invalid
//
// PARAMETERS
//  g         - graph
//  n         - node
//  traversal - id of an open traversal
//
xapi node_invalidate(struct node * restrict n, int traversal)
  __attribute__((nonnull));

/// node_refresh
//
// SUMMARY
//  determine whether a node is invalid, according to the options on the fs it belongs to
//
// PARAMETERS
//  g         - graph
//  n         - node
//  traversal - id of an open traversal
//
xapi node_refresh(struct node * restrict n, int traversal)
  __attribute__((nonnull));

/// node_operations_dispatch
//
// function pointers for moria/operations which operate on the node graph
//
struct operations_dispatch * node_operations_dispatch;

#endif
