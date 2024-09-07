/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "stats.h"
#include "descriptor.h"

struct fsent;
struct bacon_writer;
struct fsname;
struct graph_invalidation_context;

typedef union stats_fsindex {
  struct {
      uint32_t count;   // number of indexed directories
      uint32_t put;     // counter ; directory added to the index
      uint32_t del;     // counter ; directory removed from the index
  };

  stats_record record;
} stats_fsindex;

extern struct moria_vertex *g_fsindex;
extern descriptor_type descriptor_stats_fsindex;

/* index a non-shadow directory node for graph lookup by label */
void fsindex_put(struct fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

struct fsname *fsindex_miss(const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

/* remove a previously indexed node from the graph lookup data structure */
void fsindex_del(struct fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

struct fsname *fsindex_lookup(const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

void fsindex_show(void);
void fsindex_describe(struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void fsindex_setup(void);
void fsindex_cleanup(void);

void fsindex_stats_build(bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t fsindex_id_cat(char *dst, size_t sz)
  __attribute__((nonnull));
