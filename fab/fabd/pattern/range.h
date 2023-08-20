/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

union pattern_segment;
struct yyu_location;

void pattern_range_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , uint8_t start
  , uint8_t end
)
  __attribute__((nonnull));
