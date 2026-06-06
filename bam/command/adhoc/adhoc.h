/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

extern struct adhoc_args {
  const char *request;
  uint16_t request_len;
} adhoc_args;

extern struct command adhoc_command;
