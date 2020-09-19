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

attrs32 * APIDATA fabipc_event_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FABIPC_EVENT_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FABIPC_EVENT_TYPE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(fabipc_msg_type_attrs);
  attrs32_init(fabipc_event_type_attrs);
}

//
// api
//

fabipc_message * API fabipc_produce(
    fabipc_page * restrict pages
  , uint32_t * restrict ring_head
  , uint32_t * restrict local_tail
  , uint32_t * restrict tail_used
  , uint32_t mask
)
{
  uint32_t tail;
  uint32_t index;
  struct fabipc_page *page;

  tail = __atomic_fetch_add(local_tail, 1, __ATOMIC_SEQ_CST);

  // overflow check
  RUNTIME_ASSERT((tail + 1) != *ring_head);

  *tail_used = tail;
  index = tail & mask;
  page = &pages[index];

  return &page->msg;
}

void API fabipc_post(
    uint32_t * restrict ring_tail
  , uint32_t * restrict local_tail
  , uint32_t tail_used
  , int32_t * restrict waiters
)
{
  int32_t one = 1;
  int32_t zero = 0;
  uint32_t tail;

  tail = __atomic_load_n(local_tail, __ATOMIC_SEQ_CST);

  if(tail != (tail_used + 1)) {
    /* another post is in process */
    return;
  }

  *ring_tail = tail;
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

  if(head == tail) {
    return 0;
  }

  index = head & mask;
  page = &pages[index];
  return &page->msg;
}

void API fabipc_consume(uint32_t * restrict ring_head)
{
  (*ring_head)++;
}
