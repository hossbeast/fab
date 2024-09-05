/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "modules.h"

/* rmas commands */
extern struct command rma_command;
extern struct command rmas_command;

/* module rmas subcommand */
extern struct {
  char module_id[40];
  uint16_t module_id_len;
  uint64_t module_identify_id;
  uint64_t list_id;
} h;

extern command module_rmas_command;
