/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "ring.h"
#include "common/attrs.h"

static int bam_wait(uint32_t *p, uint32_t v, struct timespec * restrict interval)
{
  if(syscall(SYS_futex, p, FUTEX_WAIT, v, interval, 0, 0) == 0) {
    /* waited and was awoken */
    return BAMIPC_WAITED;
  }

  if(errno == EINTR) {
    /* interrupted by signal */
    return BAMIPC_EINTR;
  } else if(interval && errno == ETIMEDOUT) {
    /* waited and timed out */
    return BAMIPC_ETIMEDOUT;
  }

  /* spurious wait */
  RUNTIME_ASSERT(errno == EAGAIN);
  return BAMIPC_EAGAIN;
}

int bamipc_wait_head(bamipc_ring * restrict ring, uint32_t head, struct timespec * restrict interval)
{
  return bam_wait(&ring->head, head, interval);
}

int bamipc_wait_tail(bamipc_ring * restrict ring, uint32_t tail, struct timespec * restrict interval)
{
  return bam_wait(&ring->tail, tail, interval);
}

int bamipc_wake_head(bamipc_ring * restrict ring)
{
  if(syscall(SYS_futex, &ring->head, FUTEX_WAKE, UINT32_MAX) != 0) {
    return BAMIPC_AWOKEN;
  }

  return BAMIPC_OK;
}

int bamipc_wake_tail(bamipc_ring * restrict ring)
{
  if(syscall(SYS_futex, &ring->tail, FUTEX_WAKE, 1) != 0) {
    return BAMIPC_AWOKEN;
  }

  return BAMIPC_OK;
}

/*
 * tx
 */

uint32_t bamipc_produce(bamipc_ring * restrict ring)
{
  return atomic_fetch_inc(&ring->prod);
}

int bamipc_reserve(bamipc_ring * restrict ring, uint32_t slot, uint32_t * restrict head)
{
  uint32_t delta;
  int r;

  *head = atomic_load(&ring->head);
  delta = slot - *head;

  /* synchronize with consumer - is capacity available? */
  if(delta < BAMIPC_RINGSIZE) {
    r = BAMIPC_OK;
  } else {
    r = BAMIPC_STALLED;
  }

  return r;
}

int bamipc_post(bamipc_ring * restrict ring, uint32_t slot)
{
  uint32_t prev;
  uint32_t next;

  prev = slot;
  next = slot + 1;

  if(atomic_cas(&ring->tail, &prev, next)) {
    return BAMIPC_OK;
  }

  return BAMIPC_STALLED;
}

int bamipc_peek(bamipc_ring * restrict ring)
{
  if(ring->head != ring->tail) {
    return BAMIPC_OK;
  }

  return BAMIPC_STALLED;
}

int bamipc_acquire(bamipc_ring * restrict ring, uint32_t * restrict tail)
{
  uint32_t head;

  head = atomic_load(&ring->head);
  *tail = atomic_load(&ring->tail);

  if(head == *tail) {
    return BAMIPC_STALLED;
  }

  return BAMIPC_OK;
}

void bamipc_consume(bamipc_ring * restrict ring)
{
  atomic_fetch_inc(&ring->head);
}
