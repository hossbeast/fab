/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct llist;
struct moria_vertex;

void moria_graph_linear_search(
    struct llist * restrict vertex_list
  , const char * restrict label
  , uint16_t label_len
  , const uint32_t *mask_list
  , const uint32_t *bits_list
  , uint16_t mask_list_len
  , struct moria_vertex ** restrict rv
)
  __attribute__((nonnull(1, 2, 7)));
