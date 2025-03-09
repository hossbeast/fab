/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "ipc.h"
#include "pipe.h"
#include "ring.h"

//
// api
//

void bamipc_pipe_init(struct bamipc_pipe * restrict pipe)
{
  int x;

  memset(pipe, 0, sizeof(*pipe));

  for(x = 0; x < ARRAY_LEN(pipe->messages); x++) {
    pipe->messages[x].size = BAMIPC_PAGESIZE;
  }
}

void bamipc_pipe_reset(struct bamipc_pipe * restrict pipe)
{
  memset(&pipe->ring, 0, sizeof(pipe->ring));
}

int bamipc_tx(bamipc_pipe * restrict pipe, struct timespec * restrict timeout, struct bamipc_message ** restrict msg)
{
  uint32_t slot;
  uint32_t index;
  uint32_t head;
  bamipc_page *page;
  bool waited;
  int x;
  int r;

  uint32_t prev;
  uint32_t next;

  /* get the next slot from the tail for which there is capacity to post */
  for(x = 0 ;; x++) {
    slot = atomic_load(&pipe->ring.prod);
    if((r = bamipc_reserve(&pipe->ring, slot, &head)) == BAMIPC_OK) {
      goto produce;
    }
    RUNTIME_ASSERT(r == BAMIPC_STALLED);

    /* insufficient capacity */
    r = bamipc_wait_head(&pipe->ring, head, timeout);
    switch(r) {
      case BAMIPC_ETIMEDOUT:
      case BAMIPC_EINTR:
        return r;
      case BAMIPC_WAITED:
        waited = true;
      case BAMIPC_EAGAIN:
        continue;
      default:
        RTA;
    }

produce:
    /* produce */
    prev = slot;
    next = slot + 1;
    if(atomic_cas(&pipe->ring.prod, &prev, next)) {
      break;
    }
  }

  index = slot % BAMIPC_RINGSIZE;
  *msg = &pipe->messages[index];
  page = &pipe->pages[index];
  (*msg)->slot = slot;
  (*msg)->index = index;
  (*msg)->text = page->text;

  if(x == 0) {
    (*msg)->flags |= BAMIPC_COUNTER_TX_FAST;
  } else if(waited) {
    (*msg)->flags |= BAMIPC_COUNTER_TX_WAIT;
  } else {
    (*msg)->flags |= BAMIPC_COUNTER_TX_STALL;
  }

  return BAMIPC_OK;
}

void bamipc_publish(bamipc_pipe * restrict pipe, bamipc_message * restrict msg)
{
  int x;
  int r;

  /* synchronize with other producers to publish this slot */
  for(x = 0 ;; x++) {
    if((r = bamipc_post(&pipe->ring, msg->slot)) == BAMIPC_OK) {
      break;
    }
    RUNTIME_ASSERT(r == BAMIPC_STALLED);
  }

  bamipc_wake_tail(&pipe->ring);

  if(x == 0) {
    msg->flags |= BAMIPC_COUNTER_PUBLISH_FAST;
  } else {
    msg->flags |= BAMIPC_COUNTER_PUBLISH_STALL;
  }
}

int bamipc_rx(struct bamipc_pipe * restrict pipe, struct timespec * restrict timeout, bamipc_message ** restrict msg)
{
  uint32_t slot;
  uint32_t index;
  uint32_t tail;
  bamipc_page *page;
  int r;

  while(1) {
    r = bamipc_acquire(&pipe->ring, &tail);
    switch(r) {
      case BAMIPC_OK:
        goto leave;
      case BAMIPC_STALLED:
        break;
      default:
        RTA;
    }

    r = bamipc_wait_tail(&pipe->ring, tail, timeout);
    switch(r) {
      case BAMIPC_EINTR:
      case BAMIPC_ETIMEDOUT:
        return r;
      case BAMIPC_EAGAIN:
      case BAMIPC_WAITED:
        break;
      default:
        RTA;
    }
  }

leave:
  slot = pipe->ring.head;
  index = slot % BAMIPC_RINGSIZE;
  *msg = &pipe->messages[index];
  page = &pipe->pages[index];
  (*msg)->text = page->text;

  return BAMIPC_OK;
}

void bamipc_release(struct bamipc_pipe * restrict pipe, bamipc_message * restrict msg)
{
  uint32_t flags;

  flags = msg->flags;
  bamipc_consume(&pipe->ring);
  bamipc_wake_head(&pipe->ring);

  switch(flags & 0x0f) {
    case BAMIPC_COUNTER_TX_FAST:
      pipe->stats.tx_fast++;
      break;
    case BAMIPC_COUNTER_TX_WAIT:
      pipe->stats.tx_waits++;
      break;
    case BAMIPC_COUNTER_TX_STALL:
      pipe->stats.tx_stalls++;
      break;
  }
  switch(flags & 0xf0) {
    case BAMIPC_COUNTER_PUBLISH_FAST:
      pipe->stats.publish_fast++;
      break;
    case BAMIPC_COUNTER_PUBLISH_STALL:
      pipe->stats.publish_stalls++;
      break;
  }
}
