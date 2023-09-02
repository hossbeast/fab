/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <errno.h>

#include "value.h"
#include "bacon/assert.h"
#include "bacon/load.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/load.h"
#include "narrator/growing.h"
#include "valyria/pstring.h"
#include "common/attrs.h"

#include "glue.h"
#include "eval.h"
#include "eval.h"
#include "make.h"
#include "writer.h"

typedef struct eval_test {
  XUNITTEST;

  /* inputs */
  char * vars;
  char * expr;

  /* expected */
  char * expected;
  int expected_rv;
} eval_test;

static void test_eval_setup(xunit_unit * unit)
{
  bacon_load();
  narrator_load();
}

static void test_eval_cleanup(xunit_unit * unit)
{
  bacon_unload();
  narrator_unload();
}

static void test_eval_entry(xunit_test * _test)
{
  eval_test * test = containerof(_test, eval_test, xu);

  bacon_parser * parser = 0;
  bacon_value *actual = 0;
  bacon_value *vars = 0;
  bacon_value *expected = 0;
  bacon_expr *expr = 0;
  narrator_growing * N0 = 0;
  yyu_location loc;
  int r;

  bacon_parser_alloc(&parser);
  narrator_growing_create(&N0);

  // parse
  if(test->vars) {
    if((bacon_parser_parse(parser, MMS(test->vars), 0, BACON_TYPE_SET, &vars))) {
      printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
    }
  }
  if((bacon_parser_expr_parse_partial(parser, test->expr, strlen(test->expr) + 2, "-test-", 0, &loc, &expr))) {
    printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
  }
  assert_eq_u32(strlen(test->expr), loc.l);

  // expected
  if(test->expected) {
    if((bacon_parser_parse(parser, MMS(test->expected), 0, 0, &expected))) {
      printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
    }
  }

  // act
  r = bacon_eval(parser, expr, vars, &actual);

  // assert
  assert_eq_d(test->expected_rv, r);
  assert_eq_value(expected, actual);

  bacon_parser_expr_free(parser, expr);
  bacon_parser_value_free(parser, vars);
  bacon_parser_value_free(parser, expected);
  bacon_parser_value_free(parser, actual);
  bacon_parser_free(parser);
  narrator_growing_free(N0);
}

//
// public
//

static xunit_unit xunit = {
    xu_setup : test_eval_setup
  , xu_cleanup : test_eval_cleanup
  , xu_entry : test_eval_entry
  , xu_tests : (eval_test*[]) {
    /* variables */
      (eval_test[]){{
          vars : "a : b"
        , expr : (char[]) { "$a\0" }
        , expected : "b"
      }}
    , (eval_test[]){{
          vars : "a : b"
        , expr : (char[]) { "$b\0" }
      }}
    , (eval_test[]){{
          vars : "a : 2"
        , expr : (char[]) { "$a == 2\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          vars : "a : 2"
        , expr : (char[]) { "$a + 2\0" }
        , expected : "4"
      }}
    , (eval_test[]){{
          vars : "a : { 2 3 }"
        , expr : (char[]) { "$a + 2\0" }
        , expected : "{ 2 3 }"
      }}
    , (eval_test[]){{
          vars : "a : { 2 3 }"
        , expr : (char[]) { "$a | 2\0" }
        , expected_rv : -EINVAL
      }}

    /* logical operations, integers */
    , (eval_test[]){{
          expr : (char[]) { "1 == 2\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 != 2\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 < 2\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "2 < 2\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 <= 2\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 > 2\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "2 > 1\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "2 >= 1\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "2 && 1\0" }
        , expected : "true"
      }}

    /* logical operations, strings */
    , (eval_test[]){{
          expr : (char[]) { "a == a\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "a != a\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "a < b\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "a > b\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "a && b\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "a || b\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "a || b || true\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "\"\" || b\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "b && \"\"\0" }
        , expected : "false"
      }}

    /* arithmetic expressions, integers */
    , (eval_test[]){{
          expr : (char[]) { "1 + 2\0" }
        , expected : "3"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 + -2\0" }
        , expected : "-1"
      }}
    , (eval_test[]){{
          expr : (char[]) { "-1 - -2\0" }
        , expected : "1"
      }}
    , (eval_test[]){{
          expr : (char[]) { "-1 + -2\0" }
        , expected : "-3"
      }}
    , (eval_test[]){{
          expr : (char[]) { "4 / 2\0" }
        , expected : "2"
      }}
    , (eval_test[]){{
          expr : (char[]) { "4 / 2.0\0" }
        , expected : "2.000000"
      }}
    , (eval_test[]){{
          expr : (char[]) { "4 % 2\0" }
        , expected : "0"
      }}
    , (eval_test[]){{
          expr : (char[]) { "17 % 4\0" }
        , expected : "1"
      }}
    , (eval_test[]){{
          expr : (char[]) { "17 * 4\0" }
        , expected : "68"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 << 2\0" }
        , expected : "4"
      }}
    , (eval_test[]){{
          expr : (char[]) { "8 >> 2\0" }
        , expected : "2"
      }}
    /* compound expressions */
    , (eval_test[]){{
          expr : (char[]) { "1 + 2 + 3\0" }
        , expected : "6"
      }}
    , (eval_test[]){{
          expr : (char[]) { "2 * 3 * 4\0" }
        , expected : "24"
      }}

    /* arithmetic expressions, float */
    , (eval_test[]){{
          expr : (char[]) { "1 + -2.23\0" }
        , expected : "-1.230000"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1.253 + 2\0" }
        , expected : "3.253000"
      }}

    /* arithmetic expressions, strings */
    , (eval_test[]){{
          expr : (char[]) { "a + b\0" }
        , expected : "ab"
      }}

    /* bitwise operations */
    , (eval_test[]){{
          expr : (char[]) { "1 | 2\0" }
        , expected : "3"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 & 2\0" }
        , expected : "0"
      }}
    , (eval_test[]){{
          expr : (char[]) { "1 ^ 2\0" }
        , expected : "3"
      }}

    /* unary operations */
    , (eval_test[]){{
          expr : (char[]) { "!1\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "!!1\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "~0xff\0" }
        , expected : "-256"
      }}
    , (eval_test[]){{
          expr : (char[]) { "!{ true }\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "!!{ true }\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "!{ }\0" }
        , expected : "true"
      }}

    /* logical operations - aggregates */
    , (eval_test[]){{
          expr : (char[]) { "{ 4 } == 4\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "4 == [ 4 ]\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 4 ] == { 4 }\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 4 ] == [ 4 ]\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ 4 } == { 4 }\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ 4 } && { 4 }\0" }
        , expected : "true"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ } && { 4 }\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ ] && [ 4 ]\0" }
        , expected : "false"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 1 ] && [ 4 ]\0" }
        , expected : "true"
      }}

    /* arithmetic operations - not defined for aggregates */

    /* arithmetic operations - mixed aggregates and scalars */
    , (eval_test[]){{
          expr : (char[]) { "{ 1 2 3 } + 4\0" }
        , expected : "{ 1 2 3 4 }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 1 2 3 ] + 4\0" }
        , expected : "[ 1 2 3 4 ]"
      }}
    , (eval_test[]){{
          expr : (char[]) { "true + { 1 2 3 }\0" }
        , expected : "{ 1 2 3 true }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "true + [ 1 2 3 ]\0" }
        , expected : "[ 1 2 3 true ]"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ 1 2 3 } + { 4 }\0" }
        , expected_rv : -EINVAL
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 1 ] + [ 4 ]\0" }
        , expected_rv : -EINVAL
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ 1 } + [ 4 ]\0" }
        , expected_rv : -EINVAL
      }}

    /* bitwise operations - aggregates */
    , (eval_test[]){{
          expr : (char[]) { "{ 1 2 3 } | { 4 }\0" }
        , expected : "{ 1 2 3 4 }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 1 2 3 ] | [ 4 ]\0" }
        , expected : "[ 1 2 3 4 ]"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ 1 2 } & { 2 }\0" }
        , expected : "{ 2 }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 1 0 2 3 ] & [ 1 3 2 ]\0" }
        , expected : "[ 1 2 ]"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ 1 2 } ^ { 2 }\0" }
        , expected : "{ 1 }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 1 2 ] ^ [ 2 ]\0" }
        , expected : "[ 1 2 2 ]"
      }}
    , (eval_test[]){{
          expr : (char[]) { "[ 2 3 ] ^ [ 2 ]\0" }
        , expected : "[ 3 ]"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ a : b } | { c : d }\0" }
        , expected : "{ a : b c : d }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "{ a : b } & { c : d }\0" }
        , expected : "{ }"
      }}
    , (eval_test[]){{
          expr : (char[]) { "!({ a : b } & { c : d })\0" }
        , expected : "true"
      }}

    , 0
  }
};
XUNIT_UNIT(xunit);
