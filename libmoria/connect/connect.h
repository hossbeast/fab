/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/rbtree.h"

struct moria_vertex;
struct moria_graph;
struct moria_edge;

typedef struct moria_connect_context {
  rbtree_search_context identity_search_context;
  rbtree_search_context vertex_search_context;
} moria_connect_context;

/* return values from moria_preconnect */
#define MORIA_NOEDGE       0 /* edge not found */
#define MORIA_HASEDGE      1 /* edge found having the same vertices and attrs */
#define MORIA_EDGEATTRS    2 /* edge found but has different attrs */
#define MORIA_EDGEDOWN     3 /* identity edge exists in A->down with this label but different B vertex */
#define MORIA_EDGEUP       4 /* identity edge exists in B->up with a different A vertex */

/*
 * prepare to call moria_connect
 *
 * ctx   - context
 * g     - graph
 * B     - downwards-facing vertex
 * A     - upwards-facing vertex
 * attrs - properties for the edge
 * [e]   - (returns) the edge found, if any
 *
 * returns one of the MORIA_* response codes
 */
int moria_preconnect(
    moria_connect_context * restrict ctx
  , struct moria_graph * const restrict g
  , struct moria_vertex * B
  , struct moria_vertex * A
  , uint32_t attrs
  , struct moria_edge ** restrict e
)
  __attribute__((nonnull(1, 2, 3, 4)));

/*
 * create the edge B : A with the specified attributes (idempotent)
 *
 * ctx   - context
 * g     - graph
 * B     - downwards-facing vertex
 * A     - upwards-facing vertex
 * edge  - edge to use
 * attrs - properties for the edge
 * [e]   - (returns) the edge found, if any
 */
void moria_connect(
    moria_connect_context * restrict ctx
  , struct moria_graph * const restrict g
  , struct moria_edge * const restrict e
  , struct moria_vertex * const restrict B
  , struct moria_vertex * const restrict A
  , uint32_t attrs
)
  __attribute__((nonnull));

/* prepare to call moria_hyperconnect */
int moria_preconnect_hyper(
    moria_connect_context * restrict ctx
  , struct moria_graph * const restrict g
  , struct moria_vertex ** const restrict Blist
  , uint16_t Blen
  , struct moria_vertex ** const restrict Alist
  , uint16_t Alen
  , uint32_t attrs
  , struct moria_edge ** const restrict ep
)
  __attribute__((nonnull(1, 2)));

/* create the edge { Bset } : { Aset } (idempotent) */
void moria_connect_hyper(
    moria_connect_context * restrict ctx
  , struct moria_graph * const restrict g
  , struct moria_edge * const restrict e
  , struct moria_vertex ** const restrict Blist
  , uint16_t Blen
  , struct moria_vertex ** const restrict Alist
  , uint16_t Alen
  , uint32_t attrs
)
  __attribute__((nonnull(1, 2, 3)));

/* remove the edge B : A if any */
void moria_disconnect(
    struct moria_graph * const restrict g
  , struct moria_vertex * B
  , struct moria_vertex * A
)
  __attribute__((nonnull));

/* remove the edge { Bset } : { Aset } if any */
void moria_hyperdisconnect(
    struct moria_graph * restrict g
  , struct moria_vertex ** Blist
  , uint16_t Blen
  , struct moria_vertex ** Alist
  , uint16_t Alen
)
  __attribute__((nonnull));

/* remove the specified edge */
void moria_edge_disconnect(
    struct moria_graph * const restrict g
  , struct moria_edge * restrict e
)
  __attribute__((nonnull));
