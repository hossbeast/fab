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

#ifndef FABD_HANDLER_H
#define FABD_HANDLER_H

#include "types.h"
#include "xapi.h"

#include "fab/build.h"

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

extern rcu_list g_handlers;    // list of active handlers

/* lock for running the build, e.g. build or autobuild commands */
extern struct trylock handler_build_lock;

enum handler_state {
  /* build thread dependent states */
    HANDLER_BUILD_PENDING     = FAB_BUILD_PENDING
  , HANDLER_BUILD_IN_PROGRESS = FAB_BUILD_IN_PROGRESS
  , HANDLER_BUILD_SUCCEEDED   = FAB_BUILD_SUCCEEDED
  , HANDLER_BUILD_FAILED      = FAB_BUILD_FAILED
  , HANDLER_BUILD_NONE        = FAB_BUILD_NONE

  /* reconcile thread dependent states */
  , HANDLER_RECONCILE_PENDING
  , HANDLER_RECONCILE_IN_PROGRESS
  , HANDLER_RECONCILE_DONE
};

typedef struct handler_context {
  union {
    llist lln;      // freelist
    rcu_list stk;   // g_handlers
  };

  selector_context sel_ctx;
//  rule_run_context rule_ctx;
  struct selection * selection;
  struct graph_invalidation_context invalidation;
  struct request_parser * request_parser;
  bool autorun;

  enum handler_state state;

  pid_t tid;
  pid_t client_pid;
  pid_t client_tid;
  uint32_t client_msg_id;

  union {
    char channel_state;

    struct {
      /* fabipc channel for the client */
      struct channel * chan;

      /* subscribed events */
      uint32_t event_mask;
    };
  };
} handler_context;

xapi handler_setup(void);
xapi handler_cleanup(void);
xapi handler_system_reload(struct handler_context * restrict ctx)
  __attribute__((nonnull));

/* create/release handlers */

xapi handler_alloc(handler_context ** restrict rv)
  __attribute__((nonnull));

void handler_release(handler_context * restrict ctx);
void handler_reset(handler_context * restrict ctx);

/* complete a request */
xapi handler_process_command(struct handler_context * restrict ctx, struct command* restrict cmd)
  __attribute__((nonnull));

#if 0
void handler_request_completes(struct handler_context * restrict ctx, int code, const char * restrict text, uint16_t text_len)
  __attribute__((nonnull(1)));

void handler_request_complete(struct handler_context * restrict ctx, int code)
  __attribute__((nonnull));
#endif

#endif
