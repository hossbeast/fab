/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
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
void config_setup(void);

/* free resources */
void config_cleanup(void);

void config_system_bootstrap(void);

void config_system_reconcile(
    bool * restrict work
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void config_active_say(struct narrator * restrict N)
  __attribute__((nonnull));

void config_alloc(config ** restrict vp, struct moria_graph * restrict g)
  __attribute__((nonnull));

/*
 * Write a config stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
void config_collate_stats(void *dst, size_t sz, config *cfg, bool reset, size_t *zp)
  __attribute__((nonnull));
