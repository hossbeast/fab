/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

There is up to one plugin thread per plugin

*/

#include "types.h"

#include "rcu_list.h"
#include "locks.h"
#include "valyria/llist.h"

struct request_handler;
struct bamipc_pipe;

typedef struct plugin_thread {
  struct plugin *plugin;
  pid_t tid;
  llist lln;      // freelist
  rcu_list stk;   // thread list
  struct bamipc_pipe *pipe;   // for receiving events
  uint32_t event_mask;
  bool shutdown;
} plugin_thread;

extern rcu_list plugin_thread_list;
extern struct futexlock plugin_thread_list_lock;

/* launch a plugin thread */
void plugin_thread_launch(struct plugin * restrict plugin)
  __attribute__((nonnull));

void plugin_thread_unready(struct plugin_thread * restrict thread)
  __attribute__((nonnull));

void plugin_thread_kill(struct plugin_thread * restrict thread)
  __attribute__((nonnull));

plugin_thread *plugin_thread_alloc(void)
  __attribute__((nonnull));

void plugin_thread_release(plugin_thread *);

void plugin_thread_cleanup(void);
