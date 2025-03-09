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
struct channel_thread;
struct plugin_thread;

typedef struct request_handler {
  llist lln;      /* freelist */

  /* mutable runtime state */
  struct module *mod;
  struct selection *selection;
  bool invalidated_any;

  /* id of the currently-processing request */
  uint64_t request_msgid;
  bool request_error;

  /* bamipc channel for the client, if any */
  struct channel_thread *channel_thread;
  struct channel *channel;

  /* message pipe towards the plugin */
  struct plugin_thread *plugin_thread;
  struct bamipc_pipe *pipe;

  /* scratch space not reset across realloc */
  union {
    char scratch_space;

    struct {
      selector_context sel_ctx;
      struct request_parser *request_parser;
    };
  };
} request_handler;

void request_handler_setup(void);
void request_handler_cleanup(void);
void request_handler_system_reload(struct request_handler * restrict ctx)
  __attribute__((nonnull));

/* create/release handlers */
request_handler *request_handler_alloc(void);

void request_handler_release(request_handler * restrict ctx)
  __attribute__((nonnull));

void request_handler_reset(request_handler * restrict ctx)
  __attribute__((nonnull));

/* complete a request */
void request_handler_process_command(struct request_handler * restrict ctx, struct command* restrict cmd)
  __attribute__((nonnull));

struct bamipc_message *request_handler_tx(request_handler * restrict handler)
  __attribute__((nonnull));

void request_handler_post(request_handler * restrict handler, struct bamipc_message * restrict msg)
  __attribute__((nonnull));
