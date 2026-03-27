/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/llist.h"

struct fsent;
struct graph_invalidation_context;
struct config;

/*
 * visit directory entries in a filesystem tree, creating graph nodes for each node as necessary
 *
 * either base is specified (if traversing an already-instantiated tree), or parent is specified
 * (node to which the tree is to be attached)
 *
 * [base]     - base of the tree rooted at abspath, if it already exists
 * [parent]   - parent of base to which it is attached when created
 * abspath    - absolute path to a base directory
 */
void walker_descend(
    struct fsent * restrict base
  , const char * restrict abspath
  , uint16_t walk_id
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

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
 * reapply config to the extern module
 *
 * ctx    - reconfiguration context
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
void walker_system_reconfigure(struct config * restrict cfg, bool dryrun)
  __attribute__((nonnull));

/*
 * reconcile an fsent against the filesystem, update its hash, and invalidate it as needed
 *
 * n            - file or symlink vertex
 * path         - path to n if available
 * invalidation - invalidation context
 */
void walker_poke(
    struct fsent * restrict n
  , char * restrict path
  , uint16_t path_len
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 4)));

/*
 * check whether an fsent is matched by the walker exclude list
 */
bool walker_exclude_list_evaluate(struct fsent * restrict n)
  __attribute__((nonnull));
