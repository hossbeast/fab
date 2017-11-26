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

#include "lorien/load.h"
#include "value/load.h"
#include "yyutil/load.h"

struct ff_parser_test;
#define TEST_TYPE struct ff_parser_test
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "logger.h"
#include "logger/category.h"

#include "ff_parser.h"
#include "ff_node.h"
#include "FF.errtab.h"
#include "logging.h"

#include "macros.h"

typedef struct ff_parser_test {
  xunit_test;

  char * text;
} ff_parser_test;

//
// public
//

static xapi ff_parser_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(lorien_load);
  fatal(yyutil_load);
  fatal(value_load);

  // logging
  fatal(logging_finalize);

  finally : coda;
}

static xapi ff_parser_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(lorien_unload);
  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

//
// tests
//

static xapi ff_parser_test_entry(ff_parser_test * test)
{
  enter;

  ff_node * actual = 0;
  narrator * N1 = 0;

  fatal(narrator_growing_create, &N1);

  // parse
  fatal(ff_parser_parse, 0, MMS(test->text), 0, &actual);

  // for now only asserting round trip
  fatal(ffn_say_normal, actual, N1);
  assert_eq_s(test->text, narrator_growing_buffer(N1));

finally:
  fatal(narrator_xfree, N1);
  ffn_free(actual);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : ff_parser_unit_setup
  , xu_cleanup : ff_parser_unit_cleanup
  , xu_entry : ff_parser_test_entry
  , xu_tests : (xunit_test*[]) {
#define TEST(x) (ff_parser_test[]) {{ text : x }},
    TEST("artifact program.{{$final,$debug,$devel}{.pic,}}?")
    TEST("rule program.? : main?.o")
    TEST("rule program.? : main?.o")
    TEST("rule %.y : %.y.header, %.y.rules")
    TEST("rule .tab.{c,h} : %.y")
    TEST("rule %.tokens.{c,h} : %.tab.c")
    TEST("rule %.lex.{c,h} : %.l")
    TEST("rule %.states.{c,h} : %.lex.h")
    TEST("var cflags.$xapi.debug -m64 -Wall -Werror")
    TEST("rule xunit : A/BC/D")
    TEST("rule A/BC/D : xunit")
    TEST("rule xunit : A/{B,C}/D")
  /* alternations */
    TEST("rule A/{B,C}/D : xunit")
    TEST("rule A/{B,}/D : xunit")
    TEST("rule A/{,B}/D : xunit")
  /* classes */
    TEST("rule A/[a-z0-9]/D : xunit")
    TEST("rule A/[^a-z0-9]/D : xunit")
    TEST("rule A/[ABC0-9XY]/D : xunit")
    0
  }
};
