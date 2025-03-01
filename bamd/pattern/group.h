/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
union pattern_segment;
struct pattern_segments;
struct llist;

void pattern_group_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , struct llist * restrict segments
  , const char * restrict name
  , uint8_t name_len
  , uint16_t group_number
  , uint16_t capture_group_number
)
  __attribute__((nonnull(1, 2)));
