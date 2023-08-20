/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct moria_graph;
struct moria_vertex;
struct moria_edge;

/// vertex_init
//
// SUMMARY
//  allocate a vertex in the graph
//
// PARAMETERS
//  v      - (returns) vertex
//  g      - graph
//  attrs  - bitmask
//  label  - label for the vertex
//
// VARIANT
//  w - provide label as pointer/length pair
//
// REMARKS
//  the label is not copied into or owned by the vertex
//
void moria_vertex_init(struct moria_vertex * const restrict v, struct moria_graph * const restrict g, uint32_t attrs)
  __attribute__((nonnull));

void moria_vertex_inits(struct moria_vertex * const restrict v, struct moria_graph * const restrict g, uint32_t attrs, const char * const restrict label)
  __attribute__((nonnull));

void moria_vertex_initw(struct moria_vertex * const restrict v, struct moria_graph * const restrict g, uint32_t attrs, const char * const restrict label, uint16_t label_len)
  __attribute__((nonnull));

/*
 * Get the vertex at distance 1 from a starting vertex on the identity tree having the
 * specified label. These apis do not follow link nodes.
 *
 * v     - starting vertex
 * label - select the vertex with this label
 * attrs - one of MORIA_TRAVERSE_{UP,DOWN}
 */
struct moria_vertex * moria_vertex_downs(const struct moria_vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_downw(const struct moria_vertex * restrict v, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

struct moria_edge * moria_vertex_edge_downs(const struct moria_vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct moria_edge * moria_vertex_edge_downw(const struct moria_vertex * restrict v, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_up(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

struct moria_edge * moria_vertex_edge_up(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_descends(const struct moria_vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_descendw(const struct moria_vertex * restrict v, const char * restrict path, uint16_t path_len)
  __attribute__((nonnull));
