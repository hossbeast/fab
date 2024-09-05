/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct moria_vertex;

bool membership_vertex(
    const struct moria_vertex * restrict v
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
)
  __attribute__((nonnull));

bool membership_edge(
    const moria_edge * restrict e
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
)
  __attribute__((nonnull));
