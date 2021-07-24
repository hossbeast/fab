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

#ifndef FABD_STATS_H
#define FABD_STATS_H

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

#endif
