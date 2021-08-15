/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include "ipc.h"
#include "common/attrs.h"

attrs32 * APIDATA fabipc_msg_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FABIPC_MSG_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FABIPC_MSG_TYPE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(fabipc_msg_type_attrs);
}

//
// api
//

fabipc_message * API fabipc_produce(
    fabipc_page * restrict pages
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
  struct fabipc_page *page;

  /* wait until producer slot is available */
  producer = atomic_fetch_inc(producers);
  while(producer >= FABIPC_PRODUCERS)
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
    delta = (tail - head) & (FABIPC_RINGSIZE - 1);
    if((delta & ~(FABIPC_PRODUCERS - 1)) != ((FABIPC_RINGSIZE - 1) & ~(FABIPC_PRODUCERS - 1))) {
      /* not full */
      break;
    }

    /* claimed index not yet available - waiting for consumer (1) */
    atomic_store(waiters, 1);
    syscall(SYS_futex, waiters, FUTEX_WAIT, 1, 0, 0, 0);
  }

  index = tail & (FABIPC_RINGSIZE - 1);
  page = &pages[index];
  page->tail = tail;

  RUNTIME_ASSERT(page->state == FABIPC_PAGE_STATE_UNUSED);

  return &page->msg;
}

void API fabipc_post(
    fabipc_message * restrict msg
  , uint32_t * restrict producers
  , int32_t * restrict waiters
)
{
  fabipc_page *page;

  page = containerof(msg, fabipc_page, msg);
  page->state = FABIPC_PAGE_STATE_POSTED;

  atomic_fetch_dec(producers);
  if(atomic_exchange(waiters, 0)) {
    syscall(SYS_futex, waiters, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  }
}

fabipc_message * API fabipc_acquire(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
)
{
  uint32_t head;
  uint32_t tail;
  uint32_t index;
  fabipc_page *page;

  head = *ring_head;
  tail = *ring_tail;
  smp_mb();

  while(head != tail)
  {
    index = head & (FABIPC_RINGSIZE - 1);
    page = &pages[index];

    if(page->state == FABIPC_PAGE_STATE_POSTED) {
      page->state = FABIPC_PAGE_STATE_ACQUIRED;
      page->head = head;
      return &page->msg;
    }

    head++;
  }

  return 0;
}

void API fabipc_consume(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , fabipc_message * restrict msg
  , int32_t * restrict waiters
)
{
  fabipc_page *page;
  uint32_t head;
  uint32_t x;
  uint32_t index;

  /* fast path with exactly one acquired page */
  page = containerof(msg, fabipc_page, msg);
  page->state = FABIPC_PAGE_STATE_UNUSED;
  head = page->head;
  if(atomic_cas(ring_head, &head, page->head + 1)) {
    goto waken;
  }

  /* check whether posted pages up to this point have all been consumed */
  for(x = *ring_head; x != (page->head + 1); x++)
  {
    index = x & (FABIPC_RINGSIZE - 1);
    page = &pages[index];
    if(page->state != FABIPC_PAGE_STATE_UNUSED) {
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

void API fabipc_release(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , fabipc_message * restrict msg
)
{
  fabipc_page *page;

  page = containerof(msg, fabipc_page, msg);
  atomic_store(ring_head, page->head);
}
