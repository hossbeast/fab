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

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

/// stats_parse
//
// SUMMARY
//  populates g_stats
//
xapi stats_setup(void);

/// stats_teardown
//
// free g_stats
//
xapi stats_cleanup(void);

/// stats_report
//
// SUMMARY
//  log a description of stats
//
xapi stats_report(void);

void stats_timing_start(struct timespec * restrict start, uint8_t n)
  __attribute__((nonnull));

void stats_timing_stop(struct timespec * restrict start, uint64_t * restrict stat)
  __attribute__((nonnull));

void stats_timing_mark(struct timespec * restrict start, uint64_t * restrict stat)
  __attribute__((nonnull));

#define STATS_TABLE                                                              \
  DEF(stathash, uint64_t)                 /* file is stat-hashed */              \
  DEF(contenthash, uint64_t)              /* file is content-hash */             \
  DEF(bsexec, uint64_t)                   /* build slot is executed */           \
  DEF(fstree_refresh, uint64_t)           /* fstree refreshes */                 \
  DEF(module_reload, uint64_t)            /* module is reloaded */               \
  DEF(model_reload, uint64_t)             /* model is reloaded */                \
  DEF(module_refresh, uint64_t)           /* module is refreshed */              \
  DEF(rule_run, uint32_t)                 /* a rule is run */                    \
  DEF(rules, uint64_t)                    /* extant rules */                     \
  DEF(rmas, uint64_t)                     /* extant rmas */                      \
  DEF(models, uint64_t)                   /* extant models */                    \
  DEF(modules, uint64_t)                  /* extant modules */                   \
  DEF(nodes_shadow, uint64_t)             /* extant shadow nodes */              \
  DEF(nodes, uint64_t)                    /* extant nodes */                     \
  DEF(rcu_synchronize_nobody, uint32_t)                                          \
  DEF(rcu_synchronize_wait, uint32_t)                                            \
  DEF(rcu_quiesce_nograce, uint32_t)                                             \
  DEF(rcu_quiesce_notlast, uint32_t)                                             \
  DEF(rcu_quiesce_wake, uint32_t)                                                \
  DEF(loader, uint64_t)                                                          \
  DEF(loader_reconfigure, uint64_t)                                              \
  DEF(loader_walker, uint64_t)                                              \
  DEF(loader_modules, uint64_t)                                              \
  DEF(loader_rules, uint64_t)                                              \

extern struct g_stats {
#undef DEF
#define DEF(x, t) t x;
STATS_TABLE
#undef DEF
} g_stats;

#define STATS_INC(x) do {   \
  g_stats.x++;              \
} while(0)

#define STATS_DEC(x) do {   \
  g_stats.x--;              \
} while(0)

#endif
