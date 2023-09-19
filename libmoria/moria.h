/* Copyright (c) 2012-2021 Todd Freed <todd.freed@gmail.com>

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

#ifndef _MORIA_H
#define _MORIA_H

#include "types.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"

struct attrs32;
struct llist;
struct narrator;
struct traversal_criteria;
struct moria_vertex;

#define MORIA_VERTEX_LINK   0x80000000 /* in vertex attrs, vertex refers to another vertex */
#define MORIA_EDGE_HYPER    0x80000000 /* in edge attrs, many-to-many hyperedge */
#define MORIA_EDGE_IDENTITY 0x40000000 /* in edge attrs, one-to-one identity edge */

typedef struct moria_graph {
  size_t vertex_index;    // traversal
  uint64_t vertex_mask;

  size_t edge_index;      // traversal
  uint64_t edge_mask;

  llist states;           // struct traversal_state
} moria_graph;

#define MORIA_GRAPH_INITIALIZER(x) { vertex_mask : 1, edge_mask : 1, states : LLIST_INITIALIZER(x.states) }

typedef struct moria_traversible  {
  size_t index;           // (entity id * 3) / 64
  uint64_t mask;          // (entity id * 3) % 64
} moria_traversible;

typedef struct moria_vertex {
  moria_traversible ent;        // traversable entity
  llist owner;                  // list/freelist

  const char *    label;        // (not owned)
  uint16_t        label_len;

  /* the bottom 6 bits are ignored for the purposes of ordering vertices in graph_say */
  uint32_t        attrs;        // properties of the vertex

  /* edges where this == B, i.e. { up } -> this */
  rbtree up;

  /* upwards identity edge - NOT stored in up */
  struct moria_edge * up_identity;

  /* edges where this == A, i.e. this -> { down } */
  rbtree down;

  /* referent for VERTEX_LINK */
  struct moria_vertex *ref;

  llist lln;                    // adhoc
  rbnode rbn_lookup;            // graph_lookup
} moria_vertex;

typedef struct moria_edge {
  moria_traversible ent;     // traversable entity
  llist owner;               // list/freelist
  uint32_t attrs;            // properties of the edge

  union {
    struct {
      rbnode rbn_up;
      struct moria_vertex *A;      // A depends on B, A -> B
    };

    /* hyperedge */
    struct {
      struct {
        struct moria_vertex *v;
        rbnode rbn;
      } *Alist;
      uint16_t Alen;
    };
  };

  union {
    struct {
      rbnode rbn_down;
      struct moria_vertex *B;      // A depends on B, A -> B, A is up, B is down
    };

    /* hyperedge */
    struct {
      struct {
        struct moria_vertex *v;
        rbnode rbn;
      } *Blist;
      uint16_t Blen;
    };
  };

  llist lln;                // adhoc
} moria_edge;

typedef struct moria_connect_context {
  rbtree_search_context identity_search_context;
  rbtree_search_context vertex_search_context;
} moria_connect_context;

#define MORIA_NOEDGE       0 /* edge not found */
#define MORIA_HASEDGE      1 /* edge found having the same vertices and attrs */
#define MORIA_EDGEATTRS    2 /* edge found but has different attrs */
#define MORIA_EDGEDOWN     3 /* identity edge exists in A->down with this label but different B vertex */
#define MORIA_EDGEUP       4 /* identity edge exists in B->up with a different A vertex */

/*
 * create the edge A : B with the specified attributes (idempotent)
 *
 * REMARKS
 *  A : B, e.g. A needs B, e.g. A depends on B, e.g. A is up from B, B is down from A
 *
 * PARAMETERS
 *  ctx   - context
 *  g     - graph
 *  A     - vertex
 *  B     - vertex
 *  attrs - properties of the edge
 *  [e]   - (returns) the edge found, if any
 */
int moria_preconnect(
    moria_connect_context * restrict ctx
  , struct moria_graph * const restrict g
  , struct moria_vertex * A
  , struct moria_vertex * B
  , uint32_t attrs
  , struct moria_edge ** restrict e
)
  __attribute__((nonnull(1, 2, 3, 4)));

void moria_connect(
    moria_connect_context * restrict ctx
  , struct moria_graph * const restrict g
  , struct moria_edge * const restrict e
  , struct moria_vertex * const restrict A
  , struct moria_vertex * const restrict B
  , uint32_t attrs
)
  __attribute__((nonnull));

/*
 * create the edge { Aset } : { Bset } (idempotent)
 */
int moria_preconnect_hyper(
    moria_connect_context * restrict ctx
  , moria_graph * const restrict g
  , moria_vertex ** const restrict Alist
  , uint16_t Alen
  , moria_vertex ** const restrict Blist
  , uint16_t Blen
  , uint32_t attrs
  , moria_edge ** const restrict ep
)
  __attribute__((nonnull(1, 2)));

void moria_connect_hyper(
    moria_connect_context * restrict ctx
  , moria_graph * const restrict g
  , moria_edge * const restrict e
  , moria_vertex ** const restrict Alist
  , uint16_t Alen
  , moria_vertex ** const restrict Blist
  , uint16_t Blen
  , uint32_t attrs
)
  __attribute__((nonnull(1, 2, 3)));

void moria_disconnect(
    struct moria_graph * const restrict g
  , struct moria_vertex * A
  , struct moria_vertex * B
)
  __attribute__((nonnull));

void moria_hyperdisconnect(
    struct moria_graph * restrict g
  , struct moria_vertex ** Alist
  , uint16_t Alen
  , struct moria_vertex ** Blist
  , uint16_t Blen
)
  __attribute__((nonnull));

void moria_edge_disconnect(
    struct moria_graph * const restrict g
  , struct moria_edge * restrict e
)
  __attribute__((nonnull));

#endif
