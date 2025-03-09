/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "ipc.h"

struct timespec;

//
// pipe - multi-producer, single-consumer, unidirectional
//

void bamipc_pipe_init(struct bamipc_pipe * restrict pipe)
  __attribute__((nonnull));

void bamipc_pipe_reset(struct bamipc_pipe * restrict pipe)
  __attribute__((nonnull));

/*
 * prepare to publish a message on the pipe
 *
 * pipe    - the pipe
 * timeout - maximum time to wait
 * msg     - (returns) the message
 *
 * RETURNS
 *  OK        message was received
 *  ETIMEDOUT waited and timed out
 *  EINTER    waited and interrupted
 *
 * NOTES
 *  supports multiple concurrent producers
 */
int bamipc_tx(struct bamipc_pipe * restrict pipe, struct timespec * restrict timeout, bamipc_message ** restrict msg)
  __attribute__((nonnull(1, 3)));

/*
 * publish the specified message
 *
 * pipe - the pipe
 * msg  - the message
 *
 * NOTES
 *  blocks until the message can be published
 */
void bamipc_publish(struct bamipc_pipe * restrict pipe, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

/*
 * consumer - receive a message on the pipe
 *
 * pipe    - the pipe
 * timeout - maximum time to wait
 * msg     - (returns) the message
 *
 * RETURNS
 *  OK        - message received
 *  ETIMEDOUT - waited and timed out
 *  EINTR     - waited and interrupted
 *
 * NOTES
 *  only a single consumer is permitted at a time
 */
int bamipc_rx(struct bamipc_pipe * restrict pipe, struct timespec * restrict timeout, bamipc_message ** restrict msg)
  __attribute__((nonnull(1, 3)));

/*
 * consumer - release the message at the head
 *
 * pipe - the pipe
 * msg  - the message
 *
 * NOTES
 *  the released message is always at the head. the message only carries flags
 *  used to decide which counters to increment
 */
void bamipc_release(struct bamipc_pipe * restrict pipe, bamipc_message * restrict msg)
  __attribute__((nonnull));
