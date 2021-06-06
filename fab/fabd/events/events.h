/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_EVENTS_H
#define FABD_EVENTS_H

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
  , struct channel ** restrict first
  , struct fabipc_message ** restrict msg
)
  __attribute__((nonnull));

/*
 * publish an event - called after events_would
 */
void events_publish(struct channel * first, struct fabipc_message * msg)
  __attribute__((nonnull));

#endif
