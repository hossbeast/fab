/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "rcu_list.h"
#include "selector.h"
#include "rule.h"

#include "locks.h"

struct channel;
struct bamipc_message;
struct request;
struct request_parser;
struct selection;
struct command;
struct module;
struct plugin_thread;

extern rcu_list g_handlers; // list of active handlers
extern struct futexlock handlers_lock;

/* lock for running the build, e.g. build or autobuild commands */
extern struct trylock handler_build_lock;

typedef struct handler_context {
  union {
    llist lln;      // freelist
    rcu_list stk;   // g_handlers
  };

  pid_t tid;        /* for receiving signals from the run thread */

  /* mutable runtime state */
  bool running;
  struct module *mod;
  struct selection *selection;
  bool invalidated_any;

  /* subscribed events */
  uint32_t event_mask;

  /* bacon events queue, when no channel is present */
  llist events_txq;
  struct futexlock events_txq_lock;
  pid_t events_tid;

  /* bamipc channel for the client, if any */
  struct channel *chan;

  /* scratch space not reset across realloc */
  union {
    char scratch_space;

    struct {
      selector_context sel_ctx;
      struct request_parser *request_parser;
    };
  };
} handler_context;

void handler_setup(void);
void handler_cleanup(void);
void handler_system_reload(struct handler_context * restrict ctx)
  __attribute__((nonnull));

/* create/release handlers */

void handler_alloc(handler_context ** restrict rv)
  __attribute__((nonnull));

void handler_release(handler_context * restrict ctx)
  __attribute__((nonnull));

void handler_reset(handler_context * restrict ctx)
  __attribute__((nonnull));

/* complete a request */
void handler_process_command(struct handler_context * restrict ctx, struct command* restrict cmd)
  __attribute__((nonnull));
