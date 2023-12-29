/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct pattern;
struct entlist;
struct bacon_value;
struct fsent;
struct yyu_location;
struct pattern_section;
struct llist;
struct entlist;

void pattern_lookup_relative(
    const struct pattern * restrict ref
  , const struct bacon_value * restrict vars
  , const struct fsent * restrict base
  , struct entlist * restrict nodes
)
  __attribute__((nonnull(1, 3, 4)));

void pattern_lookup_indexed(
    const struct pattern * restrict ref
  , const struct bacon_value * restrict vars
  , struct entlist * restrict index_hits
  , struct entlist * restrict hits
)
  __attribute__((nonnull(1, 4)));

//#define PATTERN_LOOKUP_MODULE  0x01 /* only match directories which have a module file */

/*
 * Render a lookup pattern, and lookup a node for up each fragment. The lookup must yield exactly
 * one node for each fragment.
 *
 * ref    - reference pattern
 * base   - for relative patterns, the base node
 * attrs  - bitwise combo of PATTERN_LOOKUP_*
 * nodes  - found nodes added here
 */
void pattern_lookup(
    const struct pattern * restrict ref
  , const struct bacon_value * restrict vars
  , const struct fsent * restrict base
  , struct entlist * restrict nodes
)
  __attribute__((nonnull(1, 3)));

void pattern_resolve_lookup(
    struct pattern * restrict ref
  , struct bacon_value * restrict vars
  , const struct fsent * restrict base
  , struct entlist * restrict scratch
  , struct fsent ** restrict dirp
)
  __attribute__((nonnull(1, 5)));
