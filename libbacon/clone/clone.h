/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct bacon_value;
struct bacon_parser;

struct bacon_value *bacon_clone(
    struct bacon_parser * restrict parser
  , struct bacon_value ** dst
  , const struct bacon_value * src
)
  __attribute__((nonnull(1, 3)));
