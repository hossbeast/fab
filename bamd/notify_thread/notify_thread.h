/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fsent;
struct bacon_writer;
struct map;

enum notify_state {
  NOTIFY_MONITOR = 0,   // inotify can mark this node as stale
  NOTIFY_SUPPRESS,      // inotify events for this node temporarily ignored
  NOTIFY_EXPIRING,      // inotify to be re-enabled for this node on the next epoch
};

/* incremented each time new events are available */
extern uint16_t notify_thread_epoch;

extern struct map *notify_thread_fsent_by_in_wd;

void notify_thread_setup(void);
void notify_thread_cleanup(void);
void notify_thread_launch(void);

void notify_thread_add_watch(struct fsent * restrict n)
  __attribute__((nonnull));

void notify_thread_rm_watch(struct fsent * restrict n)
  __attribute__((nonnull));
