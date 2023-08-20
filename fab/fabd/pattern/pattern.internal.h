/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "pattern.h"

#include "macros.h"

struct narrator;
struct yyu_location;
struct narrator;
struct pattern_section;
union pattern_segment;

void pattern_segment_init(union pattern_segment * restrict n, const pattern_segment_vtable * restrict vtab, const yyu_location * restrict loc)
  __attribute__((nonnull));

void pattern_mk(
    struct pattern ** restrict rv
  , const struct yyu_location * restrict loc
  , struct pattern_section * restrict sections
)
  __attribute__((nonnull));

void pattern_segments_mk(
    struct pattern_segments ** restrict rv
  , const struct yyu_location * restrict loc
  , pattern_qualifier_type qualifier_type
  , union pattern_segment * restrict segments
)
  __attribute__((nonnull));

void pattern_attrs_say(uint16_t attrs, struct narrator * restrict N)
  __attribute__((nonnull));

void pattern_segments_say(const pattern_segments * segment_list, bool only, struct narrator * restrict N)
  __attribute__((nonnull));
