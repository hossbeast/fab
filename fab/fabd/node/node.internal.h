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

#ifndef FABD_NODE_INTERNAL_H
#define FABD_NODE_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "node.h"

struct vertex;
struct graph;

/// graph_node_create
//
// SUMMARY
//  create a vertex in g_graph
//
// PARAMETERS
//  v         - (returns) newly created vertex
//  g         - ignored
//  attrs     - ignored
//  label     - filename
//  label_len -
//
// REMARKS
//  used by unit tests to drive node operations against g_graph from a sequence of operations
//  obtained from a libmoria/operations parser
//
xapi graph_node_create(
    /* 1 */ struct vertex ** const restrict v
  , /* 2 */ struct graph * const restrict g
  , /* 3 */ uint32_t attrs
  , /* 4 */ const char * const restrict label
  , /* 5 */ uint16_t label_len
)
  __attribute__((nonnull));

/*
 * write the path to the node, relative to another node, to a buffer
 */
size_t node_get_relative_path(const node * n, const node * base, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

/*
 * write the path to a node, relative to another node, to a narrator
 */
xapi node_relative_path_say(const node * n, const node * base, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
