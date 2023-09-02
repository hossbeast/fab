/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "fab/stats.h"

struct graph_invalidation_context;

extern bool config_system_reconfigured;

/* initializes the config module, does not apply any config files */
void config_system_setup(void);
void config_system_cleanup(void);

void config_system_bootstrap(void);

void config_system_reconcile(
    bool * restrict work
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void config_system_invalidate(void);
