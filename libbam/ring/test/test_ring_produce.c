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

  uint32_t prod;
  uint32_t tail;
  uint32_t head;

  uint32_t expected_slot;
  uint32_t expected_prod;
  uint32_t expected_head;
  uint32_t expected_tail;
} ipc_test;

static void ipc_test_entry(xunit_test * _test)
{
  ipc_test *test;
  bamipc_ring ring;
  uint32_t slot;
  uint32_t head;
  int r;

  test = containerof(_test, ipc_test, xu);

  memset(&ring, 0, sizeof(ring));
  ring.prod = test->prod;
  ring.tail = test->tail;
  ring.head = test->head;

  slot = bamipc_produce(&ring);
  if((r = bamipc_reserve(&ring, slot, &head)) == BAMIPC_OK) {
    bamipc_post(&ring, slot);
  }

  if(r == BAMIPC_OK) {
    assert_eq_u32(test->expected_slot, slot);
  }
  assert_eq_u32(test->expected_prod, ring.prod);
  assert_eq_u32(test->expected_tail, ring.tail);
  assert_eq_u32(test->expected_head, ring.head);
}

//
// public
//

static xunit_unit xunit = {
    xu_entry : ipc_test_entry
  , xu_tests : (void*)(ipc_test*[]) {
      /* basic functional tests */
        /* empty */
        (ipc_test[]){{
            head : 0
          , tail : 0
          , prod : 0
          , expected_slot : 0
          , expected_head : 0
          , expected_tail : 1
          , expected_prod : 1
        }}
      , (ipc_test[]){{
            head : 10
          , tail : 10
          , prod : 10
          , expected_slot : 10
          , expected_head : 10
          , expected_tail : 11
          , expected_prod : 11
        }}
      , (ipc_test[]){{
            head : 511
          , tail : 511
          , prod : 511
          , expected_slot : 511
          , expected_head : 511
          , expected_tail : 512
          , expected_prod : 512
        }}
      , (ipc_test[]){{
            head : 512
          , tail : 512
          , prod : 512
          , expected_slot : 512
          , expected_head : 512
          , expected_tail : 513
          , expected_prod : 513
        }}
      , (ipc_test[]){{
            head : UINT32_MAX-1
          , tail : UINT32_MAX-1
          , prod : UINT32_MAX-1
          , expected_slot : UINT32_MAX-1
          , expected_head : UINT32_MAX-1
          , expected_tail : UINT32_MAX
          , expected_prod : UINT32_MAX
        }}
      , (ipc_test[]){{
            head : UINT32_MAX
          , tail : UINT32_MAX
          , prod : UINT32_MAX
          , expected_slot : UINT32_MAX
          , expected_head : UINT32_MAX
          , expected_tail : 0
          , expected_prod : 0
        }}
        /* full */
      , (ipc_test[]){{
            head : 0
          , tail : 512
          , prod : 512
          , expected_head : 0
          , expected_tail : 512
          , expected_prod : 513
        }}
      , (ipc_test[]){{
            head : 10
          , tail : 10+512
          , prod : 10+512
          , expected_head : 10
          , expected_tail : 10+512
          , expected_prod : 10+513
        }}
      , (ipc_test[]){{
            head : UINT32_MAX
          , tail : UINT32_MAX+512
          , prod : UINT32_MAX+512
          , expected_head : UINT32_MAX
          , expected_tail : UINT32_MAX+512
          , expected_prod : UINT32_MAX+513
        }}
      , (ipc_test[]){{
            head : UINT32_MAX-25
          , tail : UINT32_MAX-25+512
          , prod : UINT32_MAX-25+512
          , expected_head : UINT32_MAX-25
          , expected_tail : UINT32_MAX-25+512
          , expected_prod : UINT32_MAX-25+513
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
