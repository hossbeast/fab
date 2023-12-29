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

struct channel;
struct bamipc_message;
struct bacon_writer;

/* mask of events for which any handler has subscribed */
extern uint32_t events_mask;

/*
 * check whether any subscribers exist for the specified event type
 *
 * first  - (returns) handler for the first subscriber
 * msg    - (returns) msg associated with the first channel
 * writer - (returns) writer open to msg
 *
 * returns true if any subscribers exist
 */
bool events_would(
    bamipc_event_type type
  , struct channel * restrict * restrict first
  , struct bamipc_message ** restrict msg
)
  __attribute__((nonnull));

/*
 * publish an event - called after events_would
 */
void events_publish(struct channel * first, struct bamipc_message * msg)
  __attribute__((nonnull(2)));
