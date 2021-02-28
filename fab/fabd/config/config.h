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

#include "moria.h"

struct graph_invalidation_context;
struct narrator;
struct fsent;
struct configblob;

typedef struct config {
  moria_vertex vertex;

  /* regarding the config fsent */
  char self_node_abspath[512];
  uint16_t self_node_abspath_len;

  struct fsent *self_node;

  /* parsed from the file */
  struct configblob *cfg;
} config;



/// config_setup
//
// SUMMARY
//  initializes the config module, does not apply any config files
//
xapi config_setup(void);

/// config_cleanup
//
// suMMARY
//  free resources
//
xapi config_cleanup(void);

xapi config_system_bootstrap(void);
xapi config_system_bootstrap_late(void);

xapi config_system_reconcile(int walk_id, struct graph_invalidation_context * restrict invalidation, bool * restrict reconciled)
  __attribute__((nonnull));

xapi config_active_say(struct narrator * restrict N)
  __attribute__((nonnull));

xapi config_alloc(config ** restrict vp, struct moria_graph * restrict g)
  __attribute__((nonnull));

void config_release(config * restrict vp)
  __attribute__((nonnull));

#endif
