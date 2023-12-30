/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "moria.h"

struct fsent;
struct bacon_value;
struct moria_graph;
struct graph_invalidation_context;

/* VERTEX_VAR */
typedef struct var {
  moria_vertex vertex;

  /* runtime state */
  bam_vertex_stats stats;
  struct bacon_value *val;
  char id[40];
  uint16_t id_len;
  char token[40];
  uint16_t token_len;

  /* self_node */
  struct fsent *self_node;    // VERTEX_VAR_FILE var.bam
  bam_var_stats var_stats;
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;
} var;

void var_setup(void);
void var_cleanup(void);

extern llist var_list;    // active

/*
 * Write a vars stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
size_t var_stats_collate(void *dst, size_t sz, bool reset, struct var * restrict var)
  __attribute__((nonnull));

/* allocate a new var in the invalid state */
void var_alloc(var ** restrict vp, struct moria_graph * restrict g)
  __attribute__((nonnull));

/* parse the var if invalid */
void var_reparse(var * restrict var)
  __attribute__((nonnull));

void var_release(var * restrict var, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void var_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
