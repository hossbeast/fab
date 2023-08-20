/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "bacon.h"
#include "bacon/assert.h"
#include "bacon/load.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"

#include "store.internal.h"
#include "merge.internal.h"
#include "make.internal.h"
#include "parser.internal.h"
#include "writer.internal.h"
#include "say.internal.h"

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

static void test_merge_entry(xunit_test * _test)
{
  merge_test * test = containerof(_test, merge_test, xu);

  value_parser * parser = 0;
  value * src = 0;
  value * dst = 0;
  narrator_growing * N0 = 0;

  narrator_growing_create(&N0);

  value_parser_create(&parser);

  // parse
  value_parser_parse(parser, MMS(test->src), 0, 0, &src);

  if(test->dst) {
    value_parser_parse(parser, MMS(test->dst), 0, 0, &dst);
  }

  // merge
  value_merge(parser, &dst, src, 0);

  if(test->exp)
  {
    value_say(dst, &N0->base);

    char norm[512];
    int y = 0;
    int x;
    for (x = 0; x < N0->l; x++)
    {
      char c = N0->s[x];
      if (c == '\n') { c = ' '; }
      if (c == ' ')
      {
        if (y && norm[y - 1] == ' ') { continue; }
      }

      norm[y++] = c;
    }
    norm[y] = 0;

    const char *actual = norm;
    assert_eq_s(test->exp, actual);
  }

  value_parser_xfree(parser);
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
          dst : "{ }"
        , src : "{ 1 : 2 }"
        , exp : "{ 1 : 2 }"
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
          dst : "[ 1 ]"
        , src : "{ 1 }"
        , exp : "[ 1 1 ]"
      }}

    /* attrs */
    , (merge_test[]){{
          dst : "x : [ 1 ]"
        , src : "x : [ ]"
        , exp : "x : [ 1 ]"
      }}
    , (merge_test[]){{
          dst : "x : [ 1 ]"
        , src : "x = [ ]"
        , exp : "x : [ ]"
      }}
    , (merge_test[]){{
          dst : "x : [ ]"
        , src : "x : [ 1 ]"
        , exp : "x : [ 1 ]"
      }}
    , (merge_test[]){{
          dst : "x : [ ]"
        , src : "x = [ 1 ]"
        , exp : "x : [ 1 ]"
      }}
    , (merge_test[]){{
          dst : "foo : { bar : { baz : 10 } }"
        , src : "foo : { bar = { true : true } }"
        , exp : "foo : { bar : { true : true } }"
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

    /* error cases - differently typed aggregates */
    , (merge_test[]){{
          dst : "foo : { bar : { baz : 10 } }"
        , src : "foo : { bar : [ 10 ] }"
        , exp : "foo : { bar : { baz : 10 10 } }"
      }}
    , (merge_test[]){{
          dst : "foo : { bar : { baz : 10 } }"
        , src : "foo : { bar = [ 10 ] }"
        , exp : "foo : { bar : [ 10 ] }"
      }}
    , (merge_test[]){{
          dst : "foo : { bar : { baz : 10 } }"
        , src : "foo : { bar = [ 10 ] }"
        , exp : "foo : { bar : [ 10 ] }"
      }}

    /* list vivification */
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
