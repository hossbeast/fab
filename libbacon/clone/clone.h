/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct value;
struct value_parser;

void value_clone(
    struct value_parser * const restrict parser
  , struct value ** dst
  , const struct value * src
)
  __attribute__((nonnull));
