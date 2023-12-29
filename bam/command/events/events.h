/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

extern struct events_args {
  uint32_t event_mask;
} events_args;

extern struct command events_command;
