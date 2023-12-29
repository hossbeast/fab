/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "ipc.h"
#include "common/attrs.h"

attrs32 * APIDATA bamipc_msg_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      BAMIPC_MSG_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
BAMIPC_MSG_TYPE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(bamipc_msg_type_attrs);
}

//
// api
//

bamipc_message * API bamipc_produce(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
  , uint32_t * restrict producers
  , int32_t * restrict waiters
)
{
  uint32_t tail;
  uint32_t head;
  uint32_t index;
  uint32_t delta;
  uint32_t producer;
  struct bamipc_page *page;

  /* wait until producer slot is available */
  producer = atomic_fetch_inc(producers);
  while(producer >= BAMIPC_PRODUCERS)
  {
    atomic_fetch_dec(producers);
    atomic_store(waiters, 1);
    syscall(SYS_futex, waiters, FUTEX_WAIT, 1, 0, 0, 0);
    producer = atomic_fetch_inc(producers);
  }

  /* claim an index at the ring tail */
  tail = atomic_fetch_inc(ring_tail);
  while(1)
  {
    head = atomic_load(ring_head);
    delta = (tail - head) & (BAMIPC_RINGSIZE - 1);
    if((delta & ~(BAMIPC_PRODUCERS - 1)) != ((BAMIPC_RINGSIZE - 1) & ~(BAMIPC_PRODUCERS - 1))) {
      /* not full */
      break;
    }

    /* claimed index not yet available - waiting for consumer (1) */
    atomic_store(waiters, 1);
    syscall(SYS_futex, waiters, FUTEX_WAIT, 1, 0, 0, 0);
  }

  index = tail & (BAMIPC_RINGSIZE - 1);
  page = &pages[index];
  page->tail = tail;

  RUNTIME_ASSERT(page->state == BAMIPC_PAGE_STATE_UNUSED);

  return &page->msg;
}

void API bamipc_post(
    bamipc_message * restrict msg
  , uint32_t * restrict producers
  , int32_t * restrict waiters
)
{
  bamipc_page *page;

  page = containerof(msg, bamipc_page, msg);
  page->state = BAMIPC_PAGE_STATE_POSTED;

  atomic_fetch_dec(producers);
  if(atomic_exchange(waiters, 0)) {
    syscall(SYS_futex, waiters, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  }
}

bamipc_message * API bamipc_acquire(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
)
{
  uint32_t head;
  uint32_t tail;
  uint32_t index;
  bamipc_page *page;

  head = *ring_head;
  tail = *ring_tail;
  smp_mb();

  while(head != tail)
  {
    index = head & (BAMIPC_RINGSIZE - 1);
    page = &pages[index];

    if(page->state == BAMIPC_PAGE_STATE_POSTED) {
      page->state = BAMIPC_PAGE_STATE_ACQUIRED;
      page->head = head;

      return &page->msg;
    }

    head++;
  }

  return 0;
}

void API bamipc_consume(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , bamipc_message * restrict msg
  , int32_t * restrict waiters
)
{
  bamipc_page *page;
  uint32_t head;
  uint32_t x;
  uint32_t index;

  /* fast path with exactly one acquired page */
  page = containerof(msg, bamipc_page, msg);
  page->state = BAMIPC_PAGE_STATE_UNUSED;
  head = page->head;
  if(atomic_cas(ring_head, &head, page->head + 1)) {
    goto waken;
  }

  /* check whether posted pages up to this point have all been consumed */
  for(x = *ring_head; x != (page->head + 1); x++)
  {
    index = x & (BAMIPC_RINGSIZE - 1);
    page = &pages[index];
    if(page->state != BAMIPC_PAGE_STATE_UNUSED) {
      goto waken;
    }
  }

  /* all pages in [ ring_head, page->head ] are UNUSED ; advance the ring head */
  atomic_store(ring_head, page->head);

waken:
  if(atomic_exchange(waiters, 0)) {
    syscall(SYS_futex, waiters, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  }
}

void API bamipc_release(
    bamipc_page * restrict pages
  , uint32_t * restrict ring_head
  , bamipc_message * restrict msg
)
{
  bamipc_page *page;

  page = containerof(msg, bamipc_page, msg);
  atomic_store(ring_head, page->head);
}
