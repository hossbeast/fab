/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_FORMULA_H
#define FABD_FORMULA_H

/* A formula is a program which can be run to update the targets of a dependency edge. */

#include "xapi.h"
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

xapi formula_create(formula ** restrict fml, struct moria_graph * restrict g)
  __attribute__((nonnull));

xapi formula_reconcile(formula * restrict fml)
  __attribute__((nonnull));

xapi formula_system_reconcile(void);

xapi formula_setup(void);
xapi formula_cleanup(void);

/*
 * Write a formulas stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
xapi formula_collate_stats(void *dst, size_t sz, formula *fml, bool reset, size_t *zp)
  __attribute__((nonnull));

#endif
