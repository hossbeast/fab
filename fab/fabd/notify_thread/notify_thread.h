/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _NOTIFY_THREAD_H
#define _NOTIFY_THREAD_H

#include "types.h"

struct fsent;

enum notify_state {
  NOTIFY_MONITOR = 0,   // inotify can mark this node as stale
  NOTIFY_SUPPRESS,      // inotify events for this node temporarily ignored
  NOTIFY_EXPIRING,      // inotify to be re-enabled for this node on the next epoch
};

/* incremented each time new events are available */
extern uint16_t notify_thread_epoch;

void notify_thread_setup(void);
void notify_thread_cleanup(void);
void notify_thread_launch(void);

void notify_thread_add_watch(struct fsent * restrict n)
  __attribute__((nonnull));

void notify_thread_rm_watch(struct fsent * restrict n)
  __attribute__((nonnull));

#endif
