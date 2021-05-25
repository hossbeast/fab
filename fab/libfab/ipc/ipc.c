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
  , uint32_t mask
)
{
  uint32_t tail;
  uint32_t index;
  struct fabipc_page *page;

  tail = __atomic_fetch_add(ring_tail, 1, __ATOMIC_SEQ_CST);

  // overflow check
  RUNTIME_ASSERT((tail + 1) != *ring_head);

  index = tail & mask;
  page = &pages[index];

page->tail = tail;

  return &page->msg;
}

void API fabipc_post(
    fabipc_message * restrict msg
  , int32_t * restrict waiters
)
{
  int32_t one = 1;
  int32_t zero = 0;
  fabipc_page *page;

  page = containerof(msg, fabipc_page, msg);
  page->state = 1;
  smp_wmb();

  if(atomic_cas_i32(waiters, &one, &zero)) {
    smp_wmb();
    syscall(SYS_futex, FUTEX_WAKE, 1, 0, 0, 0);
  }
}

fabipc_message * API fabipc_acquire(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict ring_tail
  , uint32_t mask
)
{
  uint32_t head;
  uint32_t tail;
  uint32_t index;
  fabipc_page *page;

  head = *ring_head;
  tail = *ring_tail;
  smp_rmb();

  while(head != tail)
  {
    index = head & mask;
    page = &pages[index];

    if(page->state == 1) {
      page->state = 2;
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
  , uint32_t mask
)
{
  fabipc_page *page;
  uint32_t head;
  uint32_t x;
  uint32_t index;

  /* fast path with exactly one page oustanding */
  page = containerof(msg, fabipc_page, msg);
  head = page->head;
  if(__atomic_compare_exchange_n(ring_head, &head, page->head + 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
    return;
  }

  /* check oustanding pages */
  page->state = 2;
  for(x = *ring_head; x != (page->head + 1); x++)
  {
    index = x & mask;
    page = &pages[index];
    if(page->state != 2) {
      return;
    }
  }

  /* all pages in [ ring_head, page->head ] are marked ; advance the ring head */
  __atomic_store_n(ring_head, page->head, __ATOMIC_SEQ_CST);
}

void API fabipc_release(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , fabipc_message * restrict msg
  , uint32_t mask
)
{
  fabipc_page *page;

  page = containerof(msg, fabipc_page, msg);
  __atomic_store_n(ring_head, page->head, __ATOMIC_SEQ_CST);
}
