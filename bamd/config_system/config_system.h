/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "descriptor.h"
#include "stats.h"

struct fsent;
struct graph_invalidation_context;
struct moria_vertex;

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

typedef union stats_sys_config {
  struct {
    uint32_t reconfigure_ok;
    uint32_t reconfigure_err;
    uint32_t reconfigure_no_work;
  };

  stats_record record;
} stats_sys_config;

extern union stats_sys_config config_sys_stats;
extern descriptor_type descriptor_stats_sys_config;

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

void config_sys_tell(const struct moria_vertex * restrict v, struct request_handler * restrict handler)
  __attribute__((nonnull));

void config_sys_describe(const struct moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void config_sys_stats_build(const struct moria_vertex * restrict v, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void config_sys_id(const struct moria_vertex * restrict, const char ** restrict, uint16_t * restrict)
  __attribute__((nonnull));
