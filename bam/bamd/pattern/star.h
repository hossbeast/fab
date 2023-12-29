/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct yyu_location;
union pattern_segment;

void pattern_star_mk(union pattern_segment ** restrict rv, const struct yyu_location * restrict loc)
  __attribute__((nonnull));
