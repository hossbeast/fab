/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct handler_context;
struct command;

void tree_list_handler(struct handler_context * restrict ctx, struct command * restrict cmd)
  __attribute__((nonnull));
