/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "rcu_list.h"
#include "selector.h"
#include "rule.h"

#include "locks.h"

struct channel;
struct fabipc_message;
struct request;
struct request_parser;
struct selection;
struct command;
struct module;

extern rcu_list g_handlers; // list of active handlers

/* lock for running the build, e.g. build or autobuild commands */
extern struct trylock handler_build_lock;

typedef struct handler_context {
  union {
    llist lln;      // freelist
    rcu_list stk;   // g_handlers
  };

  /* scratch */
  selector_context sel_ctx;
  struct graph_invalidation_context invalidation;
  struct request_parser * request_parser;
  char *scratch;
  size_t scratch_sz;

  /* context */
  struct module *mod;
  struct selection *selection;
  bool running;

  pid_t tid;
  pid_t client_pid;
  pid_t client_tid;
  uint32_t client_msg_id;

  union {
    char channel_state;

    struct {
      /* fabipc channel for the client */
      struct channel * chan;
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

void handler_release(handler_context * restrict ctx);
void handler_reset(handler_context * restrict ctx);

/* complete a request */
void handler_process_command(struct handler_context * restrict ctx, struct command* restrict cmd)
  __attribute__((nonnull));
