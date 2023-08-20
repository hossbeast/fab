/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * events - publish events to client subscribers
 *
 * may only be called from a thread which is registered for rcu
 */

#include "types.h"
#include "fab/events.h"

struct channel;
struct fabipc_message;

/* mask of events for which any handler has subscribed */
extern uint32_t events_mask;

/*
 * check whether any subscribers exist for the specified event type
 *
 * first - (returns) handler for the first subscriber
 * msg   - (returns) msg associated with the first channel
 *
 * returns true if any subscribers exist
 */
bool events_would(
    fabipc_event_type type
  , struct channel * restrict * restrict first
  , struct fabipc_message ** restrict msg
)
  __attribute__((nonnull));

/*
 * publish an event - called after events_would
 */
void events_publish(struct channel * first, struct fabipc_message * msg)
  __attribute__((nonnull));
