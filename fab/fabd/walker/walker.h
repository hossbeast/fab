/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fsent;
struct graph_invalidation_context;
struct configblob;

/*
 * visit directory entries in a filesystem tree, creating graph nodes for each node as necessary
 *
 * either base is specified (if traversing an already-instantiated tree), or parent is specified
 * (node to which the tree is to be attached)
 *
 * [basep]    - (returns) base of tree rooted at abspath
 * [base]     - base of the tree rooted at abspath, if it already exists
 * [parent]   - parent of base to which it is attached when created
 * abspath    - absolute path to a base directory
 */
void walker_descend(
    struct fsent ** restrict basep
  , struct fsent * restrict base
  , struct fsent * restrict parent
  , const char * restrict abspath
  , uint16_t walk_id
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(4, 6)));

/*
 * traverse upwards in a filesystem tree from a base node, visiting only nodes relevant for modules
 *
 *  base      - node to ascend from
 *  [walk_id] -
 */
void walker_ascend(
    struct fsent * restrict base
  , uint16_t walk_id
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 3)));

void walker_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * reapply configuration to the extern module
 *
 * ctx    - reconfiguration context
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
int walker_system_reconfigure(struct configblob * restrict cfg, char * restrict err, uint16_t err_sz);

void walker_setup(void);
void walker_cleanup(void);

uint16_t walker_begin(void);
