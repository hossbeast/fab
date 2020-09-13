/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR src PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <string.h>

#include "xapi.h"
#include "xapi/errtab.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xapi/SYS.errtab.h"
#include "value.h"
#include "value/assert.h"
#include "value/load.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"
#include "logger.h"

#include "store.internal.h"
#include "merge.internal.h"
#include "VALUE.errtab.h"
#include "make.internal.h"
#include "parser.internal.h"
#include "writer.internal.h"
#include "say.internal.h"

typedef struct merge_test {
  XUNITTEST;

  char * dst;
  char * src;
  char * exp; // expected
  xapi expected_exit;
} merge_test;

static xapi test_merge_setup(xunit_unit * unit)
{
  enter;

  fatal(value_load);
  fatal(logger_finalize);

  finally : coda;
}

static xapi test_merge_cleanup(xunit_unit * unit)
{
  enter;

  fatal(value_unload);

  finally : coda;
}

static xapi test_merge_entry(xunit_test * _test)
{
  enter;

  merge_test * test = containerof(_test, merge_test, xu);

  value_parser * parser = 0;
  value * src = 0;
  value * dst = 0;
  narrator_growing * N0 = 0;
  xapi exit;

  fatal(narrator_growing_create, &N0);

  fatal(value_parser_create, &parser);

  // parse
  fatal(value_parser_parse, parser, MMS(test->src), 0, 0, &src);
//printf(">> SRC\n");
//fatal(value_say, src, g_narrator_stdout);
//printf("\n");

  if(test->dst)
    fatal(value_parser_parse, parser, MMS(test->dst), 0, 0, &dst);
//printf(">> DST\n");
//fatal(value_say, dst, g_narrator_stdout);
//printf("\n");

  // merge
  exit = invoke(value_merge, parser, &dst, src, 0);
  if(exit != test->expected_exit)
    fail(exit ?: SYS_ABORT);

  xapi_calltree_unwind();

  if(test->exp)
  {
//printf(">> RESULT\n");
//fatal(value_say, dst, g_narrator_stdout);
//printf("\n");

    fatal(value_say, dst, &N0->base);

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

finally:
  fatal(value_parser_xfree, parser);
  fatal(narrator_growing_free, N0);
coda;
}

//
// public
//

xunit_unit xunit = {
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
