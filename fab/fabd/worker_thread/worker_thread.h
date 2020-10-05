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

#ifndef _WORKER_THREAD_H
#define _WORKER_THREAD_H

#include "xapi.h"
#include "types.h"
#include "valyria/stack.h"
#include "valyria/llist.h"

#include "graph.h"

struct set;
struct work;
struct config;

typedef struct worker_thread_context {
  stack stk;
  int32_t tid;
  uint16_t workers_index;
  llist work_queue;

  /* scratch space */
  struct set * match_nodes;
  struct set * generate_nodes;
} worker_thread_context;

extern stack g_workers;         // list of active workers
extern uint16_t g_workers_len;

/// worker_thread_reconfigure
//
// SUMMARY
//  reapply configuration to the worker module
//
// PARAMETERS
/// ctx    - reconfiguration context
//  config - root of the config tree
//  dry    - whether to perform a dry-run
//
xapi worker_thread_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

xapi worker_thread_cleanup(void);

#endif
