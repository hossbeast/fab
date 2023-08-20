/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
union pattern_segment;
struct pattern_segments;

void pattern_class_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , struct pattern_segments * restrict segments
  , bool invert
)
  __attribute__((nonnull));
