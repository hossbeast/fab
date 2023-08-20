/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/rbtree.h"

#include "edge.h"

struct moria_vertex;

typedef struct edge_key {
  uint32_t attrs;
  struct moria_vertex ** Alist;   // sorted in ptr order
  uint16_t Alen;
  struct moria_vertex ** Blist;   // sorted in ptr order
  uint16_t Blen;
} edge_key;

int edge_cmp_up(const struct moria_edge * restrict a, const struct moria_edge * restrict b)
  __attribute__((nonnull));

int edge_cmp_rbn_up(const rbnode * restrict an, const rbnode * restrict bn)
  __attribute__((nonnull));

int edge_cmp_key_up(void * restrict key, const rbnode * restrict bn)
  __attribute__((nonnull));

int edge_cmp_down(const struct moria_edge * restrict a, const struct moria_edge * restrict b)
  __attribute__((nonnull));

int edge_cmp_key_down(void * restrict key, const rbnode * restrict rbn)
  __attribute__((nonnull));

int edge_cmp_rbn_down(const rbnode * restrict an, const rbnode * restrict bn)
  __attribute__((nonnull));
