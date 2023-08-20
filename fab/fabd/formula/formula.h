/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* A formula is a program which can be run to update the targets of a dependency edge. */

#include "types.h"

#include "moria.h"
#include "valyria/llist.h"
#include "fab/stats.h"

#include "graph.h"

struct fsent;
struct formula_value;
struct moria_graph;
struct channel;

/* VERTEX_TYPE_FML */
typedef struct formula {
  moria_vertex vertex;

  /* regarding the formula node itself */
  int fd;
  char self_node_abspath[512];
  uint16_t abspath_len;

  struct fsent * self_node;
  fab_formula_stats stats;

  /* reference counting of dependency edges having this formula */
  int32_t refs;

  /* parsed from the embedded bacon block */
  bool shebang;
  struct formula_value *file;   // primitive
  struct formula_value *envs;   // set of mappings
  struct formula_value *args;   // list
} formula;

void formula_create(formula ** restrict fml, struct moria_graph * restrict g)
  __attribute__((nonnull));

void formula_reconcile(formula * restrict fml)
  __attribute__((nonnull));

void formula_system_reconcile(void);

void formula_setup(void);
void formula_cleanup(void);

/*
 * Write a formulas stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
void formula_collate_stats(void *dst, size_t sz, formula *fml, bool reset, size_t *zp)
  __attribute__((nonnull));
