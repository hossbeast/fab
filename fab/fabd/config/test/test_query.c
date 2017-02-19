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

#include "xapi.h"
#include "xapi/errtab.h"

#include "xlinux/load.h"
#include "valyria/load.h"
#include "lorien/load.h"
#include "yyutil/load.h"

#include "xlinux/xstdlib.h"
#include "value.h"
#include "value/store.h"
#include "value/merge.h"
#include "logger.h"
#include "logger/category.h"
#include "narrator.h"
#include "narrator/fixed.h"

struct config_test;
#define TEST_TYPE struct config_test
#include "xunit.h"
#include "xunit/assert.h"

#include "config.h"
#include "config_parser.h"
#include "errtab/CONFIG.errtab.h"
#include "logging.h"

#include "snarf.h"
#include "macros.h"
#include "strutil.h"

typedef struct config_test {
  xunit_test;

  char ** config_texts;

  struct {
    char * query;
    char * value;
  } * queries;
} config_test;

static xapi config_test_entry(config_test * test)
{
  enter;

  config_parser * parser = 0;
  value_store * store = 0;
  value * root = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(narrator_fixed_create, &N0, 1024);
  fatal(narrator_fixed_create, &N1, 1024);

  // merge config texts serially
  char ** config_text = test->config_texts;
  while(*config_text)
  {
    value * val;
    fatal(config_parser_parse, &parser, &store, MMS(*config_text), 0, &val);
    if(root == 0)
      root = val;
    else
      fatal(value_merge, root, val);

    config_text++;
  }

  // perform queries
  int x;
  for(x = 0; x < sentinel(test->queries); x++)
  {
    value * expected = 0;
    if(test->queries[x].value)
      fatal(config_parser_parse, &parser, &store, MMS(test->queries[x].value), 0, &expected);

    value * actual = 0;
    fatal(config_query, root, "(path)", test->queries[x].query, 0, &actual);

    fatal(narrator_reset, N0);
    fatal(value_say, expected, N0);
    fatal(narrator_reset, N1);
    fatal(value_say, actual, N1);
    assert_eq_s(narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));
  }

finally:
  fatal(config_parser_xfree, parser);
  fatal(value_store_xfree, store);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
coda;
}

static xapi config_unit_setup(xunit_unit * unit)
{
  enter;

  int x;
  for(x = 0; x < sentinel(unit->xu_tests); x++)
    unit->xu_tests[x]->xu_entry = config_test_entry;

  // load libraries
  fatal(lorien_load);
  fatal(yyutil_load);
  fatal(value_load);

  // modules
  fatal(config_setup);
  fatal(logging_setup, 0);

  // logging
  fatal(logger_finalize);

  finally : coda;
}

static xapi config_unit_cleanup(xunit_unit * unit)
{
  enter;

  // modules
  fatal(config_cleanup);

  // libraries
  fatal(lorien_unload);
  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

//
// public
//

xunit_unit xunit = {
    xu_setup : config_unit_setup
  , xu_cleanup : config_unit_cleanup
  , xu_tests : (xunit_test*[]) {
      (config_test[]){{
          config_texts: (char *[]) { "foo bar", 0 }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo", value : "bar" }
            , { query : "foot" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) { "foo { bar baz }", 0 }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo.bar", value : "baz" }
            , { query : "foo.boot" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) { "foo [ 1 2 3 ]", 0 }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo", value : "[ 1 2 3 ]" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo [ 1 2 ]"
            , "foo [ 3 4 ]"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo", value : "[ 1 2 3 4 ]" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo [ 1 2 ]"
            , "foo = [ 3 4 ]"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo", value : "[ 3 4 ]" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo [ 1 2 ]"
            , "foo += [ 3 4 ]"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo", value : "[ 1 2 3 4 ]" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo { bar true }"
            , "foo += { baz false }"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo.bar", value : "true" }
            , { query : "foo.baz", value : "false" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo { bar true }"
            , "foo = { baz false }"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo.bar" }
            , { query : "foo.baz", value : "false" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo { bar true }"
            , "foo += { baz false }"
            , "foo += { qux [ 1 2 3 ] }"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo.bar", value : "true" }
            , { query : "foo.baz", value : "false" }
            , { query : "foo.qux", value : "[1 2 3]" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo { bar true }"
            , "foo += { qux [ 1 2 3 ] }"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo.qux.0", value : "1" }
            , { query : "foo.qux.1", value : "2" }
            , { query : "foo.qux.2", value : "3" }
            , { }
        }
      }}
    , (config_test[]){{
          config_texts: (char *[]) {
              "foo.bar { A true B false }"
            , "foo.baz.qux false"
            , 0
          }
        , queries : (typeof(*((config_test*)0)->queries)[]) {
              { query : "foo.bar.A", value : "true" }
            , { query : "foo.bar.B", value : "false" }
            , { query : "foo.baz.qux", value : "false" }
            , { }
        }
      }}
    , 0
  }
};
