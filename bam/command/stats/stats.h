/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

typedef struct {
  command cmd;

  bead targets[64];
  uint16_t targets_len;

  bool reset;
  int special;

  uint64_t requestid;
  bool any;
  bool context_module;
} b;

extern command stats_command;
