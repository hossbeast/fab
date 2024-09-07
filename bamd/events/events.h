/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * events - publish events to client subscribers
 *
 * may only be called from a thread which is registered for rcu
 */

#include "types.h"
#include "bam/events.h"
#include "bam/ipc.h"
#include "valyria/llist.h"

struct channel;
struct bamipc_message;
struct bacon_writer;
struct event_bacon;
struct futexlock;

/* this struct is defined in such a way as to be compatible with bamipc_message */
typedef struct event_bacon {
  union {
    llist lln;

    struct {
      char _pad0[offsetof(bamipc_message, code)];
      int32_t code;
    };
    struct {
      char _pad1[offsetof(bamipc_message, id)];
      uint64_t id;
    };
    struct {
      char _pad2[offsetof(bamipc_message, evtype)];
      uint32_t evtype;
    };
    struct {
      char _pad3[offsetof(bamipc_message, size)];
      uint32_t size;
    };

    struct {
      char _pad4[offsetof(bamipc_message, text)];
      char text[1024 * 16];
    };
  };
} event_bacon;

typedef struct events_context {
  bamipc_event_type type;
  struct channel *chan;
  struct llist *txq;
  struct futexlock *txq_lock;
  pid_t txq_tid;
  struct bamipc_message *msg;     // binary-bearing event
  struct event_bacon *pb;         // bacon-bearing event
} events_context;

/*
 * check whether any subscribers exist for the specified event type
 *
 * first  - (returns) handler for the first subscriber
 * msg    - (returns) msg associated with the first channel
 * writer - (returns) writer open to msg
 *
 * returns true if any subscribers exist
 */
event_bacon *events_bacon_would(events_context * restrict ctx, bamipc_event_type type)
  __attribute__((nonnull));

struct bamipc_message *events_binary_would(events_context * restrict ctx, bamipc_event_type type)
  __attribute__((nonnull));

/*
 * publish an event - called after events_would
 */
void events_publish(events_context * restrict ctx)
  __attribute__((nonnull));

event_bacon *event_bacon_alloc(void);
void event_bacon_free(event_bacon * restrict pb);

void event_bacon_free_list(llist * restrict list)
  __attribute__((nonnull));

void events_setup(void);
void events_cleanup(void);
