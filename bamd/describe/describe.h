/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

void describe_handler(struct request_handler * restrict ctx, struct command * restrict cmd)
  __attribute__((nonnull));
