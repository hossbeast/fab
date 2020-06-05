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
#include "xlinux/xstdlib.h"

#include "value/load.h"
#include "yyutil/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "logger.h"
#include "logger/category.h"

#include "value.h"
#include "value.h"
#include "assert.internal.h"
#include "make.internal.h"
#include "parser.internal.h"
#include "store.internal.h"

#include "macros.h"
#include "common/attrs.h"

typedef struct parser_test {
  XUNITTEST;

  char * text;
  value_type container;
  char * expected;
} parser_test;

static xapi test_parser_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(value_load);
  fatal(logger_finalize);

  finally : coda;
}

static xapi test_parser_cleanup(xunit_unit * unit)
{
  enter;

  fatal(value_unload);

  finally : coda;
}

static xapi test_parser_entry(xunit_test * _test)
{
  enter;

  parser_test * test = containerof(_test, parser_test, xu);

  char buf[512];
  value_parser * parser = 0;
  value * val0 = 0;
  value * val1 = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(narrator_growing_create, &N0);
  fatal(narrator_growing_create, &N1);

  fatal(value_parser_create, &parser);

printf("\n---------------\n");
printf(">> INITIAL\n%s\n", test->text);

  // parse
  fatal(value_parser_parse, parser, MMS(test->text), 0, test->container, &val0);
  fatal(value_say, val0, N0);

printf(">> FIRST\n%s\n", narrator_growing_buffer(N0));

  // round-trip
  if(val0)
  {
    size_t len = narrator_growing_size(N0);
    fatal(narrator_xseek, N0, 0, NARRATOR_SEEK_SET, 0);
    fatal(narrator_xread, N0, buf, len, 0);
    buf[len] = 0;
    fatal(value_parser_parse, parser, buf, len, 0, 0, &val1);
  }
  fatal(value_say, val1, N1);

printf(">> SECOND\n%s\n", narrator_growing_buffer(N1));

  char norm[512];
  int y = 0;
  int x;
  for (x = 0; x < narrator_growing_size(N1); x++)
  {
    char c = narrator_growing_buffer(N1)[x];
    if (c == '\n') { c = ' '; }
    if (c == ' ')
    {
      if (y && norm[y - 1] == ' ') { continue; }
    }

    norm[y++] = c;
  }
  norm[y] = 0;

  const char *actual = norm;
  assert_eq_value(val0, val1);
  assert_eq_s(test->expected ?: test->text, actual);

finally:
  fatal(value_parser_xfree, parser);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
coda;
} 

//
// manifest
//

xunit_unit xunit = {
    xu_setup : test_parser_setup
  , xu_entry : test_parser_entry
  , xu_cleanup : test_parser_cleanup
  , xu_tests : (parser_test*[]) {
    /* primitives */
      (parser_test[]) {{ text : "true" }}
    , (parser_test[]) {{ text : "false" }}
    , (parser_test[]) {{ text : "1.42" }}
    , (parser_test[]) {{ text : "-1.42" }}
    , (parser_test[]) {{ text : "-142" }}
    , (parser_test[]) {{ text : "142" }}
    , (parser_test[]) {{ text : "0x142", expected : "322" }}
    , (parser_test[]) {{ text : "foo" }}
    , (parser_test[]) {{ text : "foo/bar" }}
    , (parser_test[]) {{ text : "foo-bar" }}
    , (parser_test[]) {{ text : "/bar" }}
    , (parser_test[]) {{ text : "bar/" }}
    , (parser_test[]) {{ text : "bar-" }}
    , (parser_test[]) {{ text : "\"bar\"", expected: "bar" }}
    , (parser_test[]) {{ text : "\"ba\\x65r\"", expected: "baer" }}
    , (parser_test[]) {{ text : "\"ba\\x90r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x00r\"", expected : "\"ba\\x00r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x{0}r\"", expected : "\"ba\\x00r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x{00}r\"", expected : "\"ba\\x00r\"" }}

    /* mappings */
    , (parser_test[]) {{ text : "true : foo" }}
    , (parser_test[]) {{ text : "false : true" }}
    , (parser_test[]) {{ text : "142 : false" }}
    , (parser_test[]) {{ text : "1.42 : 142" }}
    , (parser_test[]) {{ text : "foo : 1.42" }}

    /* lists */
    , (parser_test[]) {{
          text : "[ 1 true 2 foo ]"
      }}
    , (parser_test[]) {{
          text : "[ [ 1 true ] 2 foo ]"
      }}
    , (parser_test[]) {{
          text : "[ 1 true [ 2 foo ] ]"
      }}
    , (parser_test[]) {{
          text : "[ 1 : true 2 : false ]"
      }}
    , (parser_test[]) {{
          text : "[ true : [ 1 2 3 ] ]"
      }}
    , (parser_test[]) {{
          text : "[ true true 1 1 ]"
      }}
    , (parser_test[]) {{
          text : "true true 1 1"
        , expected : "[ true true 1 1 ]"
        , container : VALUE_TYPE_LIST
      }}
    , (parser_test[]) {{
          text : "true : true 1 : 1"
        , expected : "[ true : true 1 : 1 ]"
        , container : VALUE_TYPE_LIST
      }}

    /* sets */
    , (parser_test[]) {{
          text : "{ true true 1 1 }"
        , expected : "{ true 1 }"
      }}
    , (parser_test[]) {{
          text : "{ foo true 1 2.00 }"
      }}
    , (parser_test[]) {{
          text : "foo true 1 2.00"
        , expected : "{ foo true 1 2.00 }"
        , container : VALUE_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "foo : true 1 : 2.00"
        , expected : "{ foo : true 1 : 2.00 }"
        , container : VALUE_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "foo : true 1 : 2.00 foo : true"
        , expected : "{ foo : true 1 : 2.00 }"
        , container : VALUE_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "core : { foo true 1 2.00 }"
      }}
    , (parser_test[]) {{
          text : "/ : { foo true 1 2.00 }"
      }}
    , (parser_test[]) {{
          text : "core : { foo : { bar : baz } qux : true }"
      }}
    , (parser_test[]) {{
          text : "core : { / : { invalidate : notify } /mnt/remote : { invalidate : stat } }"
      }}

    /* other */
    , (parser_test[]) {{
          text : ""
            "bisonflags   : \"--warnings=error\"\n"
            "cflags       : -g\n"
            "cflags.eapi  : [ -DEAPI -DXAPI_MODE_ERRORCODE ]\n"
        , container : VALUE_TYPE_SET
        , expected : "{ bisonflags : \"--warnings=error\" cflags : \"-g\" cflags.eapi : [ \"-DEAPI\" \"-DXAPI_MODE_ERRORCODE\" ] }"
      }}
    
    , 0
  }
};
