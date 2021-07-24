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

#ifndef FABD_DEPENDENCY_H
#define FABD_DEPENDENCY_H

#include "types.h"

#include "valyria/llist.h"
#include "moria.h"

#include "graph.h"

struct narrator;
struct fsent;
struct rule_module_edge;
struct graph_invalidation_context;

// active list
extern llist dependency_list;

/*
 * EDGE_DEPENDS - dependency edges between fsent vertices in the graph
 *
 * The targets are upwards, sources are downwards.
 */
typedef struct dependency {
  moria_edge edge;

  const struct rule_module_edge * rme;      // not null - rme which created this edge
  llist dependencies_lln;                   // in rme->dependencies
  uint32_t reconciliation_id;

  /* dependency with an attached fml can be added to the buildplan */
  const struct fsent *fml;
  uint16_t id;              // build plan id
  uint16_t depth;           // max depth
} dependency;

/* release dependency edges */
void dependency_cleanup(void);

void dependency_alloc(dependency ** restrict rv, struct moria_graph * restrict g, uint16_t Alen, uint16_t Blen)
  __attribute__((nonnull));

/* get a list of the dependency sources */
void dependency_sources(const dependency * restrict bpe, struct llist * restrict list)
  __attribute__((nonnull));

/* get a list of the dependency targets */
void dependency_targets(const dependency * restrict bpe, struct llist * restrict list)
  __attribute__((nonnull));

void dependency_say_sources(const dependency * restrict bpe, struct narrator * restrict N)
  __attribute__((nonnull));

void dependency_say_targets(const dependency * restrict bpe, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * create a dependency edge between two nodes
 *
 * A - target
 * B - source
 */
void dependency_connect(
    struct fsent * restrict A
  , struct fsent * restrict B
  , enum edge_kind relation
  , struct graph_invalidation_context * restrict invalidation
  , struct dependency ** restrict e
)
  __attribute__((nonnull));

/*
 * create a dependency edge between zero or more nodes
 *
 * Alist - targets
 * Blist - sources
 */
void dependency_hyperconnect(
    struct moria_vertex ** restrict Alist
  , uint16_t Alen
  , struct moria_vertex ** restrict B
  , uint16_t Blen
  , enum edge_kind relation
  , struct graph_invalidation_context * restrict invalidation
  , struct dependency ** restrict e
)
  __attribute__((nonnull(6, 7)));

void dependency_disconnect(dependency * restrict ne, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

#endif
