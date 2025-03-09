/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct llist;
struct bacon_value;
struct bacon_writer;
struct pattern_segments;
struct moria_vertex;
enum pattern_qualifier_type;

size_t pattern_segments_list_cat(
    void * restrict dst
  , size_t dst_size
  , const struct bacon_value * restrict vars
  , const struct llist * restrict segments
  , bool only
)
  __attribute__((nonnull(1, 4)));

void pattern_segments_mk(
    struct pattern_segments ** restrict rv
  , const struct yyu_location * restrict loc
  , enum pattern_qualifier_type qualifier_type
  , uint32_t type_qualifiers_mask
  , struct llist * restrict segments
)
  __attribute__((nonnull));

void pattern_segments_say(
    const struct pattern_segments * restrict segments
  , bool only
  , struct narrator * restrict N
)
  __attribute__((nonnull));

void pattern_segments_list_say(
    const struct llist * restrict segments_list
  , bool only
  , struct narrator * restrict N
)
  __attribute__((nonnull));

void pattern_segments_list_describe(
    const struct llist * restrict segments_list
  , struct bacon_writer * restrict builder_writer
)
  __attribute__((nonnull));

bool pattern_segments_evaluate_type_qualifiers(
    const struct pattern_segments * restrict segments
  , const struct moria_vertex * restrict v
)
  __attribute__((nonnull));

size_t pattern_segments_cat(
    void * restrict dst
  , size_t sz
  , const struct bacon_value * restrict vars
  , const struct pattern_segments * restrict segments
)
  __attribute__((nonnull));
