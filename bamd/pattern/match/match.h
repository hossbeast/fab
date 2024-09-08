/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct pattern;
struct fsent;

/*
 * Determine whether an fsent matches a match pattern
 */
void pattern_match(
    const struct pattern * restrict pat
  , const struct fsent * restrict node
  , bool * restrict matched
)
  __attribute__((nonnull));
