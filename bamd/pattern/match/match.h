/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct match_pattern;
struct fsent;
struct llist;
struct pattern_section;
struct bacon_value;

/*
 * Determine whether an fsent matches a match pattern
 */
bool pattern_match(
    const struct match_pattern * restrict pat
  , const struct bacon_value * restrict vars
  , const struct fsent * restrict node
)
  __attribute__((nonnull(1, 3)));

bool pattern_match_sections(
    const struct llist * restrict head
  , const struct pattern_section * restrict section
  , const struct bacon_value * restrict vars
  , const struct fsent * restrict node
)
  __attribute__((nonnull(1, 2, 4)));

bool pattern_match_section(
    const struct pattern_section * restrict section
  , const struct bacon_value * restrict vars
  , const struct fsent * restrict node
)
  __attribute__((nonnull(1, 3)));
