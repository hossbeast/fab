/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "graph.h"

struct moria_vertex;
struct moria_edge;
struct moria_graph;

void graph_vertex_init(struct moria_graph * const restrict g, struct moria_vertex * const restrict v)
  __attribute__((nonnull));

void graph_edge_init(struct moria_graph * const restrict g, struct moria_edge * const restrict v)
  __attribute__((nonnull));
