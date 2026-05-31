/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
union pattern_segment;
struct pattern_segments;
struct moria_vertex;
enum pattern_qualifier_type;
enum vertex_type;

void pattern_qualifier_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , enum pattern_qualifier_type qualifier_type
)
  __attribute__((nonnull));

bool pattern_qualifier_match(
    enum pattern_qualifier_type qualifier_type
  , const struct moria_vertex * restrict v
)
  __attribute__((nonnull));

bool pattern_qualifier_check(
    enum pattern_qualifier_type qualifier_type
  , enum vertex_type type
)
  __attribute__((nonnull));
