/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* A formula is a program which can be run to update the targets of a dependency edge. */

#include "types.h"

#include "moria.h"
#include "valyria/llist.h"
#include "bam/stats.h"

#include "graph.h"

struct fsent;
struct formula_value;
struct moria_graph;
struct channel;
struct rule_module_association;
//struct meta_edge;

/* VERTEX_TYPE_FML */
typedef struct formula {
  moria_vertex vertex;

  /* runtime state */
  char id[40];
  uint16_t id_len;
  char token[40];
  uint16_t token_len;
  bam_vertex_stats stats;
  int fd;
  llist invalid_lln;

  /* regarding the formula node itself */
  char abspath[512];
  uint16_t abspath_len;
  struct fsent * self_node;
  bam_formula_stats fml_stats;

  /* parsed from the embedded bacon block */
  bool shebang;
  struct formula_value *file;   // primitive
  struct formula_value *envs;   // set of mappings
  struct formula_value *args;   // list
} formula;

void formula_create(formula ** restrict fml, struct moria_graph * restrict g)
  __attribute__((nonnull));

void formula_reparse(formula * restrict fml)
  __attribute__((nonnull));

void formula_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void formula_setup(void);
void formula_cleanup(void);

void formula_release(formula * restrict fml, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * Write a formulas stats to a buffer
 *
 * reset - true to reset the stats while reading them
 */
size_t formula_stats_collate(void *dst, size_t sz, bool reset, formula * restrict fml)
  __attribute__((nonnull));

void formula_parse_enqueue(formula * restrict fml)
  __attribute__((nonnull));
