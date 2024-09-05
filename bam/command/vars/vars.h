/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "modules.h"

/* module vars subcommand */
extern struct {
  command cmd;

  char module_id[40];
  uint16_t module_id_len;

  uint64_t module_identify_id;
  uint64_t tell_id;
}g;

extern command vars_command;
