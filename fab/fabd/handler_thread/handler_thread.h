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

#ifndef FABD_HANDLER_THREAD_H
#define FABD_HANDLER_THREAD_H

/*

*/

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "valyria/stack.h"
#include "value/writer.h"

#include "fab/build.h"

#include "selector.h"
#include "graph.h"

struct selection;
struct request_parser;
struct fabipc_channel;

extern stack g_handlers;    // list of active handlers
extern int32_t g_handlers_lock;

//enum handler_build_state {
//  BUILD_NOT_STARTED,
//  BUILD_IN_PROGRESS,
//  BUILD_SUCCEEDED,
//  BUILD_FAILED,
//  BUILD_NONE,
//};

typedef struct handler_context {
  union {
    llist lln;    // freelist
    stack stk;    // g_handlers
  };
  selector_context sel_ctx;
  struct selection * selection;
  struct graph_invalidation_context invalidation;
  struct request_parser * request_parser;
  pid_t tid;
  pid_t client_pid;
  pid_t client_tid;
  uint32_t client_msg_id;
  bool autorun;
//  enum handler_build_state build_state;
  enum fab_build_state build_state;
  struct fabipc_channel * chan;
  uint32_t tail_next;
  value_writer writer;
  uint32_t event_mask;
} handler_context;

xapi handler_thread_setup(void);

xapi handler_thread_cleanup(void);

xapi handler_thread_launch(pid_t client_pid, pid_t client_tid, bool autorun)
  __attribute__((nonnull));

#endif
