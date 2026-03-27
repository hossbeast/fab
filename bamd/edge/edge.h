/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "graph.h"

struct moria_edge;
struct edges;

static inline enum edge_type edge_type_get(const moria_edge * restrict edge)
{
  uint32_t attrs;

  attrs = edge->attrs;
  attrs &= EDGE_TYPE_OPT;

  return attrs;
}

static inline enum edge_kind edge_kind_get(const moria_edge * restrict edge)
{
  uint32_t attrs;

  attrs = edge->attrs;
  attrs &= EDGE_KIND_OPT;

  return attrs;
}

void edge_describe(const struct moria_edge * restrict ne, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void edge_identity_describe(const struct moria_edge * restrict ne, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void edge_describe_down(const struct moria_edge * restrict ne, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void edge_describe_up(const struct moria_edge * restrict ne, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void edge_say(const struct moria_edge * restrict ne, struct narrator * restrict N)
  __attribute__((nonnull));

void edge_identity_say(const struct moria_edge * restrict ne, struct narrator * restrict N)
  __attribute__((nonnull));

void edge_identity_show(const struct moria_edge * restrict ne)
  __attribute__((nonnull));

void edge_show(const struct moria_edge * restrict ne)
  __attribute__((nonnull));

bool edges_gather_down(const struct moria_vertex * restrict v, edge_type type, struct edges * restrict list)
  __attribute__((nonnull));

bool edges_gather_up(const struct moria_vertex * restrict v, edge_type type, struct edges * restrict list)
  __attribute__((nonnull));

moria_edge *edges_check_up(const struct moria_vertex * restrict v, edge_type type)
  __attribute__((nonnull));

moria_edge *edges_check_down(const struct moria_vertex * restrict v, edge_type type)
  __attribute__((nonnull));

uint32_t edges_count_up(const struct moria_vertex * restrict v, edge_type type)
  __attribute__((nonnull));

uint32_t edges_count_down(const struct moria_vertex * restrict v, edge_type type)
  __attribute__((nonnull));

moria_vertex *edge_follow_up(const struct moria_vertex * restrict v, edge_type type)
  __attribute__((nonnull));

moria_vertex *edge_follow_down(const struct moria_vertex * restrict v, edge_type type)
  __attribute__((nonnull));
