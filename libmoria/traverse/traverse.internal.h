/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

#include "traverse.h"

/*

Vertex and edge entities are in separate traversal state spaces - that is, some vertex and edge may
have the same index and mask. In any particular traversal, either vertices or edges are being
visited (and tracked in the traversal state), never both simultaneously.

*/

typedef struct traversal_state {
  llist lln;        // in graph->states
  struct traversal_state *next;
  size_t size;      // elements in bits
  uint64_t bits[];
} traversal_state;

struct moria_vertex_traversal_state {
  traversal_state st;
};
struct moria_edge_traversal_state {
  traversal_state st;
};
