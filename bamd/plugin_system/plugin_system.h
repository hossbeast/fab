/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct config;
struct graph_invalidation_context;

/*
 * reapply config to the plugin_system subsystem
 *
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
void plugin_system_reconfigure(struct config * restrict cfg, bool dryrun, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void plugin_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
