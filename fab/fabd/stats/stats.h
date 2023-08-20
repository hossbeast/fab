/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "fab/stats.h"

struct fsent;

extern fab_global_stats g_stats;

#define STATS_INC(x) do {   \
  (x)++;                    \
} while(0)

#define STATS_DEC(x) do {   \
  (x)--;                    \
} while(0)

void stats_global_collate(void *dst, size_t sz, bool reset, size_t *zp)
  __attribute__((nonnull));

void stats_node_collate(void *dst, size_t sz, struct fsent * restrict n, bool reset, size_t *zp)
  __attribute__((nonnull));
