/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "value.h"

struct bacon_value;
struct bacon_parser;

int bacon_value_coerce(
    struct bacon_parser * restrict parser
  , const struct bacon_value * src
  , bacon_type type
  , struct bacon_value ** restrict valp   // either src, or a newly allocated value
)
  __attribute__((nonnull(1, 4)));
