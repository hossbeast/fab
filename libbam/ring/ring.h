/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "ipc.h"

struct timespec;

//
// multi-producer, single-consumer, unidirectional
//

/*
 * producer - prepare to use the next slot at the ring tail
 *
 * ring - the ring
 *
 * RETURNS the slot
 */
uint32_t bamipc_produce(bamipc_ring * restrict ring)
  __attribute__((nonnull));

/*
 * synchronize with the consumer on producing the specified tail slot
 *
 * ring  - the ring
 * slot  - tail slot
 * head  - (returns) ring head
 *
 * RETURNS
 *  OK      - the slot is unused and can be produced
 *  STALLED - the slot is in use and cannot be produced
 */
int bamipc_reserve(bamipc_ring * restrict ring, uint32_t slot, uint32_t *head)
  __attribute__((nonnull));

/*
 * post the previously produced message to the ring
 *
 * ring - message to post
 * slot - slot
 * tail - (returns) tail
 */
int bamipc_post(bamipc_ring * restrict ring, uint32_t slot)//, uint32_t *tail)
  __attribute__((nonnull));

/*
 * Get the next available message at the ring head. There may be only one consumer.
 *
 * pages      - pointer to ring pages
 * ring_head  - pointer to ring head
 * ring_tail  - pointer to ring tail
 */
int bamipc_acquire(bamipc_ring * restrict ring, uint32_t * restrict tail)
  __attribute__((nonnull));

int bamipc_peek(bamipc_ring * restrict ring)
  __attribute__((nonnull));

/*
 * Release one previously acquired message at the head
 *
 * ring - the ring
 */
void bamipc_consume(bamipc_ring * restrict ring)
  __attribute__((nonnull));

/*
 * block until the ring head has the specified value
 *
 * ring    - the ring
 * head    - head value to check for
 * timeout - maximum time to wait
 *
 * RETURNS
 *  WAITED   - waited and was awoken
 *  EINTR    - waited and interrupted
 *  TIMEDOUT - waited and timed out
 *  EAGAIN   - spurious wait
 */
int bamipc_wait_head(bamipc_ring * restrict ring, uint32_t head, struct timespec * restrict interval)
  __attribute__((nonnull(1)));

/*
 * as for bamipc_wait_head, but for the ring tail
 */
int bamipc_wait_tail(bamipc_ring * restrict ring, uint32_t tail, struct timespec * restrict interval)
  __attribute__((nonnull(1)));

/*
 * awaken waiters on the ring head
 *
 * ring - the ring
 *
 * RETURNS
 *  AWOKEN - one or more waiters was awoken
 *  OK     - no waiters
 */
int bamipc_wake_head(bamipc_ring * restrict ring)
  __attribute__((nonnull));

/*
 * as for bamipc_wake_head, but for the ring tail
 */
int bamipc_wake_tail(bamipc_ring * restrict ring)
  __attribute__((nonnull));
