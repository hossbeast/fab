/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

//struct exec_builder;
//struct exec_render_context;
//struct build_slot;
struct fsent;
struct buildplan_entity;
struct bacon_parser;
struct bacon_value;

void sysvar_render(
    struct bacon_value ** restrict vars
  , struct bacon_parser * restrict parser
  , const struct buildplan_entity * restrict bpe
  , const struct fsent * restrict mod
)
  __attribute__((nonnull(1, 2, 3)));
