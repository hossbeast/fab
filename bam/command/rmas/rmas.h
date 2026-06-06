/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "modules.h"

/* rmas commands */
extern struct command rma_command;
extern struct command rma_run_command;
extern struct command rmas_command;
extern struct command module_rma_command;
extern struct command module_rma_run_command;
extern struct command module_rmas_command;

extern struct rma_run_args {
  bool show_paths;
  bool show_ids;
} rma_run_args;
