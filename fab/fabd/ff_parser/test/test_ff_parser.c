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
  char * expected;
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

  ff_node * ffn = 0;
  narrator * N1 = 0;

  fatal(narrator_growing_create, &N1);

  // parse
  fatal(ff_parser_parse, 0, MMS(test->text), 0, &ffn);

  // for now only asserting round trip
  fatal(ffn_say_normal, ffn, N1);

  const char * actual = narrator_growing_buffer(N1);
  assert_eq_s(test->expected, actual);

finally:
  fatal(narrator_xfree, N1);
  ffn_free(ffn);
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
      (ff_parser_test[]) {{
          text     : "artifact program.{{$final,$debug,$devel}{.pic,}}?"
        , expected: "artifact program.{{$final,$debug,$devel}{.pic,}}?"
      }}
    , (ff_parser_test[]) {{
          text     : "rule program.? : main?.o"
        , expected : "rule program.? : main?.o"
     }}
    , (ff_parser_test[]) {{
          text     : "rule program.? : main?.o"
        , expected : "rule program.? : main?.o"
     }}
    , (ff_parser_test[]) {{
          text     : "rule %.y : %.y.header, %.y.rules"
        , expected : "rule %.y : %.y.header, %.y.rules"
     }}
    , (ff_parser_test[]) {{
          text     : "rule .tab.{c,h} : %.y"
        , expected : "rule .tab.{c,h} : %.y"
     }}
    , (ff_parser_test[]) {{
          text     : "rule %.tokens.{c,h} : %.tab.c"
        , expected : "rule %.tokens.{c,h} : %.tab.c"
     }}
    , (ff_parser_test[]) {{
          text     : "rule %.lex.{c,h} : %.l"
        , expected : "rule %.lex.{c,h} : %.l"
     }}
    , (ff_parser_test[]) {{
          text     : "rule %.states.{c,h} : %.lex.h"
        , expected : "rule %.states.{c,h} : %.lex.h"
     }}
    , (ff_parser_test[]) {{
          text     : "var cflags.$xapi.debug -m64 -Wall -Werror"
        , expected : "var cflags.$xapi.debug -m64 -Wall -Werror"
     }}
    , (ff_parser_test[]) {{
          text     : "rule xunit : A/BC/D"
        , expected : "rule xunit : A/BC/D"
     }}
    , (ff_parser_test[]) {{
          text     : "rule A/BC/D : xunit"
        , expected : "rule A/BC/D : xunit"
     }}
    , (ff_parser_test[]) {{
          text     : "rule xunit : A/{B,C}/D"
        , expected : "rule xunit : A/{B,C}/D"
     }}
  /* alternations */
    , (ff_parser_test[]) {{
          text     : "rule A/{B,C}/D : xunit"
        , expected : "rule A/{B,C}/D : xunit"
     }}
    , (ff_parser_test[]) {{
          text     : "rule A/{B,}/D : xunit"
        , expected : "rule A/{B,}/D : xunit"
     }}
    , (ff_parser_test[]) {{
          text     : "rule A/{,B}/D : xunit"
        , expected : "rule A/{,B}/D : xunit"
     }}
  /* classes */
    , (ff_parser_test[]) {{
          text     : "rule A/[a-z0-9]/D : xunit"
        , expected : "rule A/[a-z0-9]/D : xunit"
    }}
    , (ff_parser_test[]) {{
          text     : "rule A/[^a-z0-9]/D : xunit"
        , expected : "rule A/[^a-z0-9]/D : xunit"
     }}
    , (ff_parser_test[]) {{
          text     : "rule A/[ABC0-9XY]/D : xunit"
        , expected : "rule A/[ABC0-9XY]/D : xunit"
     }}
    , 0
  }
};
