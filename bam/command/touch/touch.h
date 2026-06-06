/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

extern struct touch_args {
  /* targets args */
  const char *targets[64];
  uint16_t targets_lens[64];
  uint16_t targets_len;
  bool nofollow;
  bool hidden;

  /* general */
  bool recursive;
  bool primary;
  bool cross_module;
  bool cross_fstree;
  uint16_t min_distance;
  uint16_t max_distance;
} touch_args;

extern struct command touch_command;
