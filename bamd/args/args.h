/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

extern struct g_args_t {
  /* devel only */
  char *system_config_path;
  char *user_config_path;
  uint16_t default_filesystem_invalidate;
  bool default_filesystem_mkdirs;
  bool default_filesystem_readlinks;
  bool default_filesystem_indexed;
  uint64_t sweeper_period_nsec;

  uint64_t hash;
  char *hash_str;
} g_args;

extern char *g_argv_base_dir_space;

void args_setup(int argc, char ** argv);
void args_teardown(void);
