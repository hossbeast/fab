/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "pattern.h"

struct yyu_location;
union pattern_segment;

void pattern_variable_mk(
    union pattern_segment ** restrict rv
  , const struct yyu_location * restrict loc
  , enum pattern_variable_type variable_type
  , uint16_t num
  , const char * restrict name
  , uint16_t name_len
)
  __attribute__((nonnull(1, 2)));
