/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "bam/stats.h"

struct moria_vertex;

extern bam_global_stats g_stats;

#define STATS_INC(x) do {   \
  (x)++;                    \
} while(0)

#define STATS_DEC(x) do {   \
  (x)--;                    \
} while(0)

size_t stats_global_collate(void *dst, size_t sz, bool reset)
  __attribute__((nonnull));

//size_t stats_vertex_fsent_collate(void *dst, size_t sz, bool reset, struct moria_vertex * restrict v)
//  __attribute__((nonnull));

size_t stats_collate_counters(
    void * restrict dst
  , size_t sz
  , const descriptor_type * restrict desc
  , bam_stats_record *record
  , int off
)
  __attribute__((nonnull));
