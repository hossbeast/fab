/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "yyutil/parser.h"

struct bacon_parser;
struct bacon_expr;
struct bacon_value;

int bacon_eval(
    struct bacon_parser * restrict parser
  , struct bacon_expr * restrict expr
  , const struct bacon_value * restrict bag
  , struct bacon_value ** restrict rv
)
  __attribute__((nonnull(1, 2, 4)));
