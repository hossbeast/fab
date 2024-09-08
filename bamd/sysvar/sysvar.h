/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct fsent;
struct build_slot;
struct bacon_parser;
struct bacon_value;

void sysvar_render(
    struct bacon_value ** restrict vars
  , struct bacon_parser * restrict parser
  , const struct build_slot * restrict bs
  , const struct fsent * restrict base
)
  __attribute__((nonnull(1, 2, 3)));
