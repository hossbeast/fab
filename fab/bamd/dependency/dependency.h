/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "moria.h"

#include "graph.h"

struct narrator;
struct fsent;
struct module_rule_edge;
struct graph_invalidation_context;
struct entlist;
struct buildplan_entity;

// active list
extern llist dependency_list;

/*
 * EDGE_DEPENDS - dependency edges between fsent vertices in the graph
 *
 * The targets are upwards, sources are downwards.
 */
typedef struct dependency {
  moria_edge edge;

  /* cached */
  struct buildplan_entity *bpe;
} dependency;

/* release dependency edges */
void dependency_cleanup(void);

/*
 * create a dependency edge between two nodes
 *
 * B - source (down)
 * A - target (up)
 */
void dependency_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , enum edge_type relation
)
  __attribute__((nonnull));

/*
 * create a dependency edge between zero or more nodes
 *
 * Blist - sources
 * Alist - targets
 */
void dependency_hyperconnect(
    struct moria_vertex ** restrict B
  , uint16_t Blen
  , struct moria_vertex ** restrict Alist
  , uint16_t Alen
  , enum edge_type relation
);

void dependency_disconnect(dependency * restrict ne, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void dependency_disconnecting(
    dependency * restrict dep
  , struct entlist * restrict edges
  , struct entlist * restrict vertices
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void dependency_release(dependency * restrict dep)
  __attribute__((nonnull));
