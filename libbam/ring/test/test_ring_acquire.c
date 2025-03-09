/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>

#include "xunit.h"
#include "xunit/assert.h"

#include "ipc.h"
#include "ring.h"
#include "macros.h"

typedef struct ipc_test {
  XUNITTEST;

  uint32_t head;
  uint32_t tail;

  uint32_t expected_head;
  uint32_t expected_tail;
} ipc_test;

static void test_ring_acquire_entry(xunit_test * _test)
{
  ipc_test *test;
  bamipc_ring ring;
  uint32_t tail;

  test = containerof(_test, ipc_test, xu);

  memset(&ring, 0, sizeof(ring));

  ring.head = test->head;
  ring.tail = test->tail;

  if(bamipc_acquire(&ring, &tail) == BAMIPC_OK) {
    bamipc_consume(&ring);
  }

  assert_eq_u32(test->expected_head, ring.head);
  assert_eq_d(test->expected_tail, ring.tail);
}

//
// public
//

static xunit_unit xunit = {
    xu_entry : test_ring_acquire_entry
  , xu_tests : (void*)(ipc_test*[]) {
      /* basic functional tests */
        /* empty */
        (ipc_test[]){{
            head : 0
          , tail : 0
          , expected_head : 0
          , expected_tail : 0
        }}
      , (ipc_test[]){{
            head : 10
          , tail : 10
          , expected_head : 10
          , expected_tail : 10
        }}
      , (ipc_test[]){{
            head : 511
          , tail : 511
          , expected_head : 511
          , expected_tail : 511
        }}
      , (ipc_test[]){{
            head : 512
          , tail : 512
          , expected_head : 512
          , expected_tail : 512
        }}
      , (ipc_test[]){{
            head : UINT32_MAX-1
          , tail : UINT32_MAX-1
          , expected_head : UINT32_MAX-1
          , expected_tail : UINT32_MAX-1
        }}
      , (ipc_test[]){{
            head : UINT32_MAX
          , tail : UINT32_MAX
          , expected_head : UINT32_MAX
          , expected_tail : UINT32_MAX
        }}
        /* full */
      , (ipc_test[]){{
            head : 0
          , tail : 512
          , expected_head : 1
          , expected_tail : 512
        }}
      , (ipc_test[]){{
            head : 10
          , tail : 10+512
          , expected_head : 11
          , expected_tail : 10+512
        }}
      , (ipc_test[]){{
            head : UINT32_MAX
          , tail : UINT32_MAX+512
          , expected_head : UINT32_MAX+1
          , expected_tail : UINT32_MAX+512
        }}
      , (ipc_test[]){{
            head : UINT32_MAX-25
          , tail : UINT32_MAX-25+512
          , expected_head : UINT32_MAX-25+1
          , expected_tail : UINT32_MAX-25+512
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
