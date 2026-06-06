/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "special.h"
#include "args.h"

struct bacon_writer;

extern struct command namespaces_command;
extern struct command namespace_command;

void namespace_identify_by_name(struct command * restrict cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
