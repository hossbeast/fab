/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

extern struct rule_args {
  bead target;
} rule_args;

extern struct rules_args {
} rules_args;

extern struct command rule_command;
extern struct command rules_command;
