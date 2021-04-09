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

#ifndef FABD_CONFIG_H
#define FABD_CONFIG_H

#include "types.h"
#include "xapi.h"
#include "fab/stats.h"

#include "moria.h"

struct narrator;
struct fsent;
struct configblob;
struct graph_invalidation_context;
struct moria_graph;

/* global/protected config nodes */
extern struct fsent *system_config_node;
extern struct fsent *user_config_node;

/* VERTEX_CONFIG */
typedef struct config {
  moria_vertex vertex;

  /* regarding the corresponding VERTEX_CONFIG_FILE */
  struct fsent *self_node;
  fab_config_stats stats;
  char self_node_abspath[512];
  uint16_t self_node_abspath_len;

  /* parsed from the file */
  struct configblob *cfg;
} config;

/* initializes the config module, does not apply any config files */
xapi config_setup(void);

/* free resources */
xapi config_cleanup(void);

xapi config_system_bootstrap(void);

xapi config_system_reconcile(
    bool * restrict work
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

xapi config_active_say(struct narrator * restrict N)
  __attribute__((nonnull));

xapi config_alloc(config ** restrict vp, struct moria_graph * restrict g)
  __attribute__((nonnull));

/*
 * Write a config stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
xapi config_collate_stats(void *dst, size_t sz, config *cfg, bool reset, size_t *zp)
  __attribute__((nonnull));

#endif
