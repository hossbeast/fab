/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "value.h"
#include "bacon/assert.h"
#include "bacon/load.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/pstring.h"

#include "glue.h"
#include "merge.h"
#include "make.h"
#include "writer.h"

typedef struct merge_test {
  XUNITTEST;

  char * dst;
  char * src;
  char * exp; // expected
} merge_test;

static void test_merge_setup(xunit_unit * unit)
{
  bacon_load();
}

static void test_merge_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

#include "common/attrs.h"

static void show(rbnode *rbn, int lvl);
static __attribute__((unused)) void show(rbnode *rbn, int lvl)
{
  bacon_value *v;

  if(rbn == &rbleaf) {
    return;
  }

  v = containerof(rbn, typeof(*v), rbn);
  printf("%*s", lvl * 2, "");
  printf("type : %12s hash : %08x %p ", attrs32_name_byvalue(&value_type_attrs, v->type), v->hash, v);
  bacon_value_say_flat(v, g_narrator_stdout);
  printf("\n");
  show(rbn->left, lvl + 1);
  show(rbn->right, lvl + 1);
}

static void test_merge_entry(xunit_test * _test)
{
  merge_test * test = containerof(_test, merge_test, xu);

  bacon_parser *parser;
  bacon_value *src = 0;
  bacon_value *dst = 0;
  narrator_growing *N0 = 0;
  bacon_value *expected;
  int r;

  narrator_growing_create(&N0);

  bacon_parser_alloc(&parser);

  // setup
  if((r = bacon_parser_parse(parser, MMS(test->exp), 0, 0, &expected))) {
    dprintf(2, "%.*s\n", (int)parser->yyu->error_len, parser->yyu->error_str);
  }

  // parse
  if((r = bacon_parser_parse(parser, MMS(test->src), 0, 0, &src))) {
    dprintf(2, "%.*s\n", (int)parser->yyu->error_len, parser->yyu->error_str);
  }

  if(test->dst) {
    if((r = bacon_parser_parse(parser, MMS(test->dst), 0, 0, &dst))) {
      dprintf(2, "%.*s\n", (int)parser->yyu->error_len, parser->yyu->error_str);
    }
  }

  // act
  bacon_merge(parser, &dst, src);

  // assert
  assert_eq_value(expected, dst);

  bacon_value_free(parser, src);
  bacon_value_free(parser, dst);
  bacon_value_free(parser, expected);
  bacon_parser_free(parser);
  narrator_growing_free(N0);
}

//
// public
//

static xunit_unit xunit = {
    xu_setup : test_merge_setup
  , xu_cleanup : test_merge_cleanup
  , xu_entry : test_merge_entry
  , xu_tests : (merge_test*[]) {
    /* lists */
      (merge_test[]){{
          dst : "[ a ]"
        , src : "[ b ]"
        , exp : "[ a b ]"
      }}
    , (merge_test[]){{
          dst : "[ ]"
        , src : "[ a b ]"
        , exp : "[ a b ]"
      }}
    , (merge_test[]){{
          dst : "[ a b ]"
        , src : "[ ]"
        , exp : "[ a b ]"
      }}

    /* sets */
    , (merge_test[]){{
          dst : "{ true : false }"
        , src : "{ 1 : 2 }"
        , exp : "{ true : false 1 : 2 }"
      }}
    , (merge_test[]){{
          dst : "{ 1 }"
        , src : "{ 1 }"
        , exp : "{ 1 }"
      }}
    , (merge_test[]){{
          dst : "{ 42 true false }"
        , src : "{ 42 true false }"
        , exp : "{ 42 true false }"
      }}
    , (merge_test[]){{
          dst : "{ true : false }"
        , src : "{ }"
        , exp : "{ true : false }"
      }}
    , (merge_test[]){{
          dst : "{ true : false }"
        , src : "a : b"
        , exp : "{ true : false a : b }"
      }}
    , (merge_test[]){{
          dst : "{ a : false }"
        , src : "a : b"
        , exp : "{ a : [ b false ] }"
      }}

    /* mix */
    , (merge_test[]){{
          dst : "[ true { 1 : false } ]"
        , src : "[ ]"
        , exp : "[ true { 1 : false } ]"
      }}
    , (merge_test[]){{
          dst : "x : [ 1 ]"
        , src : "x : [ ]"
        , exp : "x : [ 1 ]"
      }}
    , (merge_test[]){{
          dst : "{ 1 }"
        , src : "[ 1 ]"
        , exp : "{ 1 }"
      }}
    , (merge_test[]){{
          dst : "[ 1 ]"
        , src : "{ 1 }"
        , exp : "[ 1 1 ]"
      }}
    , (merge_test[]){{
          dst : "x : [ 1 ]"
        , src : "x : [ ]"
        , exp : "x : [ 1 ]"
      }}
    , (merge_test[]){{
          dst : "x : [ ]"
        , src : "x : [ 1 ]"
        , exp : "x : [ 1 ]"
      }}

    /* scalar/aggregate promotions */
    , (merge_test[]){{
          src : "foo : { bar }"
        , dst : "foo : baz"
        , exp : "foo : { bar baz }"
      }}
    , (merge_test[]){{
          src : "foo : bar"
        , dst : "foo : { baz }"
        , exp : "foo : { bar baz }"
      }}
    , (merge_test[]){{
          src : "foo : [ B ]"
        , dst : "foo : A"
        , exp : "foo : [ B A ]"
      }}
    , (merge_test[]){{
          src : "foo : B"
        , dst : "foo : [ A ]"
        , exp : "foo : [ A B ]"
      }}

    /* differently typed aggregates are combined */
    , (merge_test[]){{
          dst : "foo : { bar : { baz : 10 } }"
        , src : "foo : { bar : [ 10 ] }"
        , exp : "foo : { bar : { baz : 10 10 } }"
      }}

    /* list vivification */
    , (merge_test[]){{
          dst : "1"
        , src : "1"
        , exp : "[ 1 1 ]"
      }}
    , (merge_test[]){{
          dst : "B"
        , src : "A"
        , exp : "[ A B ]"
      }}
    , (merge_test[]){{
          dst : "B"
        , src : "[ A ]"
        , exp : "[ A B ]"
      }}
    , (merge_test[]){{
          dst : "B"
        , src : "{ A }"
        , exp : "{ B A }"
      }}
    , (merge_test[]){{
          dst : "B : E"
        , src : "A : C"
        , exp : "B : [ C E ]"
      }}

    /* other */
    , (merge_test[]){{
          dst : "B : E"
        , src : "42"
        , exp : "B : [ 42 E ]"
      }}
    , (merge_test[]){{
          dst : "B : E"
        , src : "[ 14 ]"
        , exp : "[ 14 B : E ]"
      }}
    , (merge_test[]){{
          dst : "B : E"
        , src : "{ 14 }"
        , exp : "{ B : E 14 }"
      }}

    /* null dst */
    , (merge_test[]){{
          src : "[ a b ]"
        , exp : "[ a b ]"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
