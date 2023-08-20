/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct pattern;
struct selection;
struct channel;

#define PATTERN_LOOKUP_DIR       0x01 /* only match directories */
#define PATTERN_LOOKUP_MODULE    0x03 /* only match directories which have a module file */
#define PATTERN_LOOKUP_MODEL     0x05 /* only match directories which have a model file */

/*
 * Render a lookup pattern, and lookup a node for up each fragment. The lookup must yield exactly
 * one node for each fragment.
 *
 * ref    - reference pattern
 * attrs  - bitwise combo of PATTERN_LOOKUP_*
 * nodes  - found nodes added here
 */
void pattern_lookup(
    const struct pattern * restrict ref
  , uint32_t attrs
  , struct selection * restrict nodes
  , struct channel * restrict chan
)
  __attribute__((nonnull(1, 3)));
