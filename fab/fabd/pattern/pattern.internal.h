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
struct bacon_writer;
struct llist;

void pattern_segment_init(union pattern_segment * restrict n, const pattern_segment_vtable * restrict vtab, const yyu_location * restrict loc)
  __attribute__((nonnull));

void pattern_mk(
    struct pattern ** restrict rv
  , const struct yyu_location * restrict loc
  , struct llist * restrict sections
)
  __attribute__((nonnull));

void pattern_attrs_say(uint16_t attrs, struct narrator * restrict N)
  __attribute__((nonnull));

#define PATTERN_SEGMENT_VTABLE(T, t, f)     \
  {                                         \
      type : t                              \
    , flags : f                             \
    , say : T ## _say                       \
    , describe : T ## _describe             \
    , destroy : T ## _destroy               \
    , search : T ## _search                 \
    , generate : T ## _generate             \
    , render : T ## _render                 \
    , cmp : T ## _cmp                       \
  }
