/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"

struct rule;
struct fsent;
struct module;
struct rule_module_association;

extern llist drm_list;      // active

/*
 * EDGE_DIR_RMA aka dirnode B -> rma A
 *
 * These edges are created when a rules' search pattern visits a directory node.
 * When the child-set of the directory node changes, that invalidation is propagated
 * upwards, that is, the rule is scheduled to be run again.
 *
 * Note that every dirnode belongs to exactly one module.
 */
typedef struct dir_rma_edge {
  moria_edge edge;

  /* cached ; these can all be looked up from the edge vertices */
  struct fsent *dir;                    // (down) B
  struct rule_module_association *rma;  // (up) A the rule -> module association
} dir_rma_edge;

/* idempotent create EDGE_DIR_RMA hyperedge */
void dir_rma_connect(struct fsent * restrict dir, struct rule_module_association * restrict rma)
  __attribute__((nonnull));

void dir_rma_connect_visit(void * restrict _rule, struct fsent * restrict dir)
  __attribute__((nonnull));

//void dir_rma_disconnect(dir_rma_edge * restrict rma)
//  __attribute__((nonnull));

void dir_rma_cleanup(void);

void dir_rma_edge_release(dir_rma_edge * restrict rde)
  __attribute__((nonnull));
