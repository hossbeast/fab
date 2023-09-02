/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct moria_edge;
struct moria_graph;
struct moria_vertex;

/*
 * Get the non-hyper edge between the specified vertices
 */
struct moria_edge * moria_edge_between(const struct moria_vertex * restrict B, const struct moria_vertex * restrict A)
  __attribute__((nonnull));

/*
 * Get the edge connecting the specified sets of edges
 */
struct moria_edge * moria_edge_of(
    struct moria_vertex ** restrict Alist
  , uint16_t Alen
  , struct moria_vertex ** restrict Blist
  , uint16_t Blen
)
  __attribute__((nonnull));

void moria_edge_init(struct moria_edge * const restrict e, struct moria_graph * const restrict g)
  __attribute__((nonnull));
