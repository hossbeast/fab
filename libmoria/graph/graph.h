/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

An identity graph is a tree where each vertex is uniquely identified by its path in the upward
direction to another vertex called the root. For example, a filesystem tree is an identity graph
with a root vertex at '/'.

hyperedges cannot be part of the identity graph

*/

#include "types.h"

#include "valyria/rbtree.h"

struct attrs32;
struct narrator;
struct moria_graph;
struct llist;

/*
 * initialize a graph
 */
void moria_graph_init(struct moria_graph * const restrict g)
  __attribute__((nonnull));

/*
 * destroy a graph, free its resources
 */
void moria_graph_destroy(struct moria_graph * const restrict g)
  __attribute__((nonnull));

/* write the normal representation of a graph to a narrator
 *
 * g   - graph
 * [d] - attrs name lookup
 * N   - narrator
 */
void moria_graph_say(
  /*  1 */   struct moria_graph * const restrict g
  /*  2 */ , struct llist ** restrict vertex_lists
  /*  3 */ , uint16_t vertex_lists_len
  /*  4 */ , struct llist ** restrict edge_lists
  /*  5 */ , uint16_t edge_lists_len
  /*  6 */ , const struct attrs32 * vertex_defs
  /*  7 */ , const struct attrs32 * edge_defs
  /*  8 */ , const uint32_t *mask_list
  /*  9 */ , const uint32_t *bits_list
  /* 10 */ , uint16_t mask_list_len
  /* 11 */ , struct narrator * const restrict N
)
  __attribute__((nonnull(1, 2, 4, 11)));
