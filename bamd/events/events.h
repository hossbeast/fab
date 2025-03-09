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
struct futexlock;
struct request_handler;
struct command;
struct channel_thread;
struct plugin_thread;

#define EVENT_SYSTEM_ERROR 0x8000

typedef struct events_context {
  struct bamipc_message *msg;

  /* channel thread */
  struct channel_thread *channel;

  /* plugin thread */
  struct plugin_thread *plugin;
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
struct bamipc_message *events_bacon_would(events_context * restrict ctx, bamipc_event_type type)
  __attribute__((nonnull));

struct bamipc_message *events_binary_would(events_context * restrict ctx, bamipc_event_type type)
  __attribute__((nonnull));

bool events_would(bamipc_event_type type);

/*
 * publish an event - called after events_would
 */
void events_publish(events_context * restrict ctx)
  __attribute__((nonnull));

void events_setup(void);
void events_cleanup(void);

void event_describe(bamipc_message * restrict e, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void event_show(bamipc_message * restrict e)
  __attribute__((nonnull));

void events_trigger(struct request_handler * restrict handler, uint32_t delta)
  __attribute__((nonnull));
