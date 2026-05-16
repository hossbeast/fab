/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>

#include "xunit.h"
#include "xunit/assert.h"

#include "adaptor.h"

#define OP_PUSH       1
#define OP_POP        2
#define OP_CHECK      3
#define OP_DUMP0      5
#define OP_DUMP1      6
#define OP_LAST       7
#define OP_OVERWRITE  8

typedef struct adaptor_test {
  XUNITTEST;

  uint32_t head;
  uint32_t cons;
  uint32_t tail;

  struct adaptor_test_op {
    int op;
    char *buf;
    uint32_t len;
    uint32_t cons;
    uint32_t buf_len;
  } **ops;

  uint32_t expected_head;
  uint32_t expected_tail;
} adaptor_test;

static void adaptor_test_entry(xunit_test * _test)
{
  adaptor_test *test;
  char buf[32];
  char last[32];
  struct adaptor adaptor = {};
  uint32_t head = 0;
  uint32_t tail = 0;
  int x;
  uint32_t len;
  void *p;
  struct adaptor_test_op *op;

  test = containerof(_test, adaptor_test, xu);

  memset(buf, 0, sizeof(buf));
  head = test->head;
  tail = test->tail;
  adaptor.buf = buf;
  adaptor.sz = sizeof(buf);
  adaptor.head = &head;
  adaptor.tail = &tail;

  for(x = 0; x < sentinel(test->ops); x++) {
    op = test->ops[x];
    switch(op->op) {
      case OP_PUSH:
        adaptor_push(&adaptor, test->ops[x]->buf, test->ops[x]->len);
        break;
      case OP_OVERWRITE:
        adaptor_overwrite(&adaptor, test->ops[x]->buf, test->ops[x]->len);
        break;
      case OP_POP:
        len = adaptor_pop(&adaptor, op->cons, last, sizeof(last));
        assert_eq_w(test->ops[x]->buf, test->ops[x]->len, last, len);
        break;
      case OP_DUMP0:
        len = adaptor_dump0(&adaptor, &p);
        assert_eq_w(test->ops[x]->buf, test->ops[x]->len, p, len);
        break;
      case OP_DUMP1:
        len = adaptor_dump1(&adaptor, &p);
        assert_eq_w(test->ops[x]->buf, test->ops[x]->len, p, len);
        break;
      case OP_LAST:
        len = adaptor_last(&adaptor, op->len, last, sizeof(last));
        assert_eq_w(op->buf, strlen(op->buf), last, len);
        break;
    }
  }

  assert_eq_u32(test->expected_head, *adaptor.head);
  assert_eq_u32(test->expected_tail, *adaptor.tail);
}

//
// public
//

static xunit_unit xunit = {
    xu_entry : adaptor_test_entry
  , xu_tests : (void*)(adaptor_test*[]) {
      /* basic functional tests */
        /* empty */
        (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foo", 3, .cons = 3 }}
              , 0
            }
          , expected_head : 3
          , expected_tail : 3
        }}
      , (adaptor_test[]){{
            head : 30
          , tail : 30
          , cons : 30
          , ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 2 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "o", 1 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foo", 3, .cons = 33, }}
              , 0
            }
          , expected_head : 33
          , expected_tail : 33
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foo", 3, .cons = 3 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foo", 3, .cons = 6 }}
              , 0
            }
          , expected_head : 6
          , expected_tail : 6
        }}
      , (adaptor_test[]){{
            head : 30
          , tail : 30
          , cons : 30
          , ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "fo", 2 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "o", 1 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foo", 3, .cons = 33 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foo", 3, .cons = 36 }}
              , 0
            }
          , expected_head : 36
          , expected_tail : 36
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "bar", 3 }}
              , (struct adaptor_test_op[]) {{ OP_POP, "foobar", 6, .cons = 6 }}
              , 0
            }
          , expected_head : 6
          , expected_tail : 6
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_DUMP0, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_DUMP1, 0, 0 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 3
        }}
      , (adaptor_test[]){{
            head : 10
          , tail : 10
          , ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 3 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "bar", 3 }}
              , (struct adaptor_test_op[]) {{ OP_DUMP0, "foobar", 6 }}
              , (struct adaptor_test_op[]) {{ OP_DUMP1, 0, 0 }}
              , 0
            }
          , expected_head : 10
          , expected_tail : 16
        }}
      , (adaptor_test[]){{
            head : 30
          , tail : 30
          , ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "foo", 2 }}
              , (struct adaptor_test_op[]) {{ OP_PUSH, "o", 1 }}
              , (struct adaptor_test_op[]) {{ OP_DUMP0, "fo", 2 }}
              , (struct adaptor_test_op[]) {{ OP_DUMP1, "o", 1 }}
              , 0
            }
          , expected_head : 30
          , expected_tail : 33
        }}

      /* last - no wrap */
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "abcd", 5 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 4
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "abcd", 4 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 4
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "bcd", 3 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 4
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "cd", 2 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 4
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "d", 1 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 4
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_PUSH, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "", 0 }}
              , 0
            }
          , expected_head : 0
          , expected_tail : 4
        }}

      /* last - wraped */
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_OVERWRITE, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "abcd", 5 }}
              , 0
            }
          , head : 30
          , tail : 30
          , expected_head : 30
          , expected_tail : 34
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_OVERWRITE, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "abcd", 4 }}
              , 0
            }
          , head : 30
          , tail : 30
          , expected_head : 30
          , expected_tail : 34
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_OVERWRITE, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "bcd", 3 }}
              , 0
            }
          , head : 30
          , tail : 30
          , expected_head : 30
          , expected_tail : 34
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_OVERWRITE, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "cd", 2 }}
              , 0
            }
          , head : 30
          , tail : 30
          , expected_head : 30
          , expected_tail : 34
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_OVERWRITE, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "d", 1 }}
              , 0
            }
          , head : 30
          , tail : 30
          , expected_head : 30
          , expected_tail : 34
        }}
      , (adaptor_test[]){{
            ops : (struct adaptor_test_op*[]) {
                (struct adaptor_test_op[]) {{ OP_OVERWRITE, "abcd", 4 }}
              , (struct adaptor_test_op[]) {{ OP_LAST, "", 0 }}
              , 0
            }
          , head : 30
          , tail : 30
          , expected_head : 30
          , expected_tail : 34
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
