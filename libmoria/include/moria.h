/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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

typedef struct moria_entity  {
  size_t index;           // (entity id * 3) / 64
  uint64_t mask;          // (entity id * 3) % 64
} moria_entity;

typedef struct moria_vertex {
  moria_entity ent;           // traversable entity
  llist owner;                // list/freelist

  const char *    label;      // (not owned)
  uint16_t        label_len;

  /* the bottom 6 bits are ignored for the purposes of ordering vertices in graph_say */
  uint32_t        attrs;      // properties of the vertex

  /* edges where this == B, i.e. this -> { up } */
  rbtree up;

  /* upwards identity edge - NOT stored in up */
  struct moria_edge *up_identity;

  /* edges where this == A, i.e. { down } -> this */
  rbtree down;

  /* referent for VERTEX_LINK */
  struct moria_vertex *ref;
} moria_vertex;

typedef struct moria_edge {
  moria_entity ent;         // traversable entity
  llist owner;              // list/freelist

  uint32_t attrs;           // properties of the edge

/*
 * A is up
 * A depends on B
 * B is down
 *
 * By convention, written left-to-right in bottom-up order
 *  B : A       or, equivalently,
 *  B -> A
 */

  union {
    struct {
      rbnode rbn_up;
      struct moria_vertex *A;
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
      struct moria_vertex *B;
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
} moria_edge;
