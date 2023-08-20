/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
union pattern_segment;
struct narrator;

void pattern_byte_mk(
    union pattern_segment ** restrict pat
  , const struct yyu_location * restrict loc
  , uint8_t code
)
  __attribute__((nonnull));

void byte_say(uint8_t code, struct narrator * restrict N)
  __attribute__((nonnull));
