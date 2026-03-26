/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct fsent;
struct build_slot;
struct bacon_parser;
struct bacon_value;
struct module;

extern struct bacon_value *sysvar_globals;

struct bacon_value *sysvar_render(
    struct bacon_parser * restrict parser
  , const struct build_slot * restrict bs
  , const struct fsent * restrict base
)
  __attribute__((nonnull(1, 2, 3)));

void sysvar_setup(void);
void sysvar_cleanup(void);

struct bacon_value *sysvar_global_render(struct bacon_parser * restrict parser, struct bacon_value * restrict vars)
  __attribute__((nonnull(1)));

struct bacon_value *sysvar_module_render(struct bacon_parser * restrict parser, struct bacon_value * restrict vars, const struct module * restrict mod)
  __attribute__((nonnull(1)));
