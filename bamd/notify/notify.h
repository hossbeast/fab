/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

#include "stats.h"
#include "descriptor.h"

struct fsent;
struct bacon_writer;
struct notify_entry;
struct graph_invalidation_context;

typedef union stats_notify {
  struct {
      uint32_t count;      /* inotify watch descriptors */
      uint32_t put;        /* add_watch events */
      uint32_t del;        /* rm_watch events */
  };

  stats_record record;
} stats_notify;

extern llist notify_watched_files;
extern union stats_notify notify_stats;
extern struct moria_vertex *g_notify;
extern descriptor_type descriptor_stats_notify;

/* index a non-shadow directory node for graph lookup by label */
void notify_put(struct fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* remove a previously indexed node from the graph lookup data structure */
void notify_del(struct fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void notify_show(void);
void notify_describe(struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void notify_setup(void);
void notify_cleanup(void);

void notify_stats_build(bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t notify_id_cat(char *dst, size_t sz)
  __attribute__((nonnull));

void notify_add_watch(struct fsent * restrict n)
  __attribute__((nonnull));

void notify_rm_watch(struct fsent * restrict n)
  __attribute__((nonnull));
