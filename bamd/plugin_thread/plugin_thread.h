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

struct handler_context;

typedef struct plugin_thread {
  struct plugin *plugin;
  pid_t tid;
  rcu_list stk;
  struct handler_context *handler;
  bool shutdown;
  bool ready_requested;
  bool ready_reported;
  bool ready_quiescent;
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
