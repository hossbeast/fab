/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fsent;
struct graph_invalidation_context;

extern struct fsent *user_config_dirnode;
extern char user_config_dir[PATH_MAX];
extern uint16_t user_config_dir_len;
extern char user_config_name[256];
extern uint16_t user_config_name_len;

extern struct fsent *system_config_dirnode;
extern char system_config_dir[PATH_MAX];
extern uint16_t system_config_dir_len;
extern char system_config_name[256];
extern uint16_t system_config_name_len;

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
