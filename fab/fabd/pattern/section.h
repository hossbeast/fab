/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "pattern.h"

struct narrator;
struct pattern_section;
struct pattern_segments;
struct yyu_location;
struct bacon_value;
struct bacon_writer;
struct llist;

void pattern_section_mk(
    struct pattern_section ** restrict rv
  , const struct yyu_location * restrict loc
  , enum pattern_nodeset nodeset
  , enum pattern_graph graph
  , enum pattern_axis axis
  , struct llist * restrict qualifiers // struct pattern_segments * restrict qualifiers
  , bool has_type_qualifiers
)
  __attribute__((nonnull(1, 2)));

void pattern_section_say(const struct pattern_section * section, bool first, struct narrator * restrict N)
  __attribute__((nonnull));

void pattern_section_describe(const struct pattern_section * section, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

int pattern_sections_cmp(const struct llist *A, const struct llist *B); // const struct pattern_section * A, const struct pattern_section * B);
int pattern_section_cmp(const struct pattern_section *A, const struct pattern_section *B);

size_t pattern_section_cat(
    char * restrict dst
  , size_t sz
  , const struct bacon_value * restrict vars
  , const struct pattern_section * restrict section
)
  __attribute__((nonnull(1, 4)));
