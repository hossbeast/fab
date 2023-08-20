/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "moria.h"

struct fsent;
struct value;
struct variant;
struct value_parser;
struct moria_graph;

/* VERTEX_VAR */
typedef struct var {
  moria_vertex vertex;

  /* regarding the var fsent */
  char self_node_abspath[512];
  uint16_t self_node_abspath_len;

  struct fsent *self_node;
  fab_var_stats stats;
  uint16_t reconciliation_id;

  /* parsed from the file */
  struct value *val;
} var;

void var_setup(void);
void var_cleanup(void);

/*
 * out - (returns) key -> value set
 */
void var_denormalize(struct value_parser * restrict parser, struct variant * restrict var, struct value * restrict valset, struct value ** restrict out)
  __attribute__((nonnull(1, 3, 4)));

void var_system_reconcile(void);

/*
 * Write a vars stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
void var_collate_stats(void *dst, size_t sz, var *vp, bool reset, size_t *zp)
  __attribute__((nonnull));

/* allocate a new var in the invalid state */
void var_alloc(var ** restrict vp, struct moria_graph * restrict g)
  __attribute__((nonnull));

/* reload the var if invalid */
void var_reconcile(var * restrict vp)
  __attribute__((nonnull));
