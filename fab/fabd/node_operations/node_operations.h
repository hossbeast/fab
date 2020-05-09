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

struct filesystem;
struct node;
struct node_edge;
struct vertex;
struct operations_dispatch;
struct graph;
struct graph_invalidation_context;

enum edge_type;
enum vertex_filetype;

extern uint32_t node_invalidation_counter;

/*
 * create a node in the graph
 *
 * valid - whether the node currently exists in the filesystem
 */
xapi node_vertex_create(struct node ** restrict n, uint32_t attrs, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

xapi node_index(struct node * restrict n)
  __attribute__((nonnull));

/// node_connect
//
// SUMMARY
//  create an edge in the graph
//
// PARAMETERS
//  above - parent
//  below - child
//
xapi node_connect(
    struct node * restrict above
  , struct node * restrict below
  , enum edge_type relation
  , struct graph_invalidation_context * restrict invalidation
  , struct node_edge ** restrict e
  , struct node ** restrict old
)
  __attribute__((nonnull(1, 2, 4)));

xapi node_hyperconnect(
    struct node ** restrict Alist
  , uint16_t Alen
  , struct node ** restrict B
  , uint16_t Blen
  , enum edge_type relation
  , struct graph_invalidation_context * restrict invalidation
  , struct node_edge ** restrict e
)
  __attribute__((nonnull(1, 3, 6)));

/// node_disconnect
//
// SUMMARY
//  remove an edge between two nodes, if any
//
xapi node_disconnect(struct node * restrict A, struct node * restrict B, struct graph_invalidation_context * restrict context)
  __attribute__((nonnull(1, 2)));

/// node_edge_disconnect
//
// remove an edge
//
xapi node_edge_disconnect(struct node_edge * restrict ne, struct graph_invalidation_context * restrict context)
  __attribute__((nonnull(1)));

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
xapi node_disintegrate_fs(struct node_edge * restrict ne, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull(1)));

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
xapi node_invalidate(struct node * restrict n, struct graph_invalidation_context * restrict context)
  __attribute__((nonnull));

xapi node_ok(struct node * restrict n)
  __attribute__((nonnull));

xapi node_delete_node(struct node * restrict n)
  __attribute__((nonnull));

xapi node_operations_create_vertex(
    struct vertex ** const restrict rv
  , struct graph * const restrict g
  , uint32_t attrs
  , const char * const restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

#endif
