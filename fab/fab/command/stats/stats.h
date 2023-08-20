/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

extern struct stats_args {
  bead targets[64];
  uint16_t targets_len;
  bool reset;
} stats_args;

extern struct command stats_command;
