/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FABD_VAR_H
#define FABD_VAR_H

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

#endif
