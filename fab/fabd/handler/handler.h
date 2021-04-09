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

/*

*/

#include "types.h"
#include "xapi.h"
#include "locks.h"

#include "fab/build.h"

#include "rcu_list.h"
#include "selector.h"
#include "rule.h"

struct fabipc_channel;
struct fabipc_message;
struct request;
struct request_parser;
struct selection;

extern rcu_list g_handlers;    // list of active handlers

/* lock for running the build, e.g. build or autobuild commands */
extern struct trylock handler_build_lock;

typedef struct handler_context {
  union {
    llist lln;      // freelist
    rcu_list stk;   // g_handlers
  };
  selector_context sel_ctx;
  rule_run_context rule_ctx;
  struct selection * selection;
  struct graph_invalidation_context invalidation;
  struct request_parser * request_parser;
  bool autorun;
  char err[256];
  uint16_t errlen;

  enum fab_build_state build_state;

  pid_t tid;
  pid_t client_pid;
  pid_t client_tid;
  uint32_t client_msg_id;

  union {
    char channel_state;

    struct {
      /* fabipc channel for the client */
      struct fabipc_channel * chan;

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

/* send/receive messages over the channel */

struct fabipc_message * handler_produce(struct handler_context * restrict ctx)
  __attribute__((nonnull));

void handler_post(struct handler_context * restrict ctx, struct fabipc_message * restrict msg)
  __attribute__((nonnull));

struct fabipc_message * handler_acquire(struct handler_context * restrict ctx)
  __attribute__((nonnull));

void handler_consume(struct handler_context * restrict ctx, struct fabipc_message * restrict msg)
  __attribute__((nonnull));

/* complete a request */

xapi handler_process_request(struct handler_context * restrict ctx, struct request * restrict request)
  __attribute__((nonnull));

void handler_request_completes(struct handler_context * restrict ctx, int code, const char * restrict text, uint16_t text_len)
  __attribute__((nonnull(1)));

void handler_request_complete(struct handler_context * restrict ctx, int code)
  __attribute__((nonnull));

#endif
