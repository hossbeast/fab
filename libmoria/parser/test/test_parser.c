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

#include "types.h"
#include "xapi.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "xapi/calltree.h"
#include "xapi/errtab.h"
#include "xlinux/xstdlib.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "yyutil/load.h"

#include "graph.h"
#include "parser.internal.h"
#include "logging.internal.h"
#include "MORIA.errtab.h"

#include "macros.h"

typedef struct graph_parser_test {
  xunit_test;

  char * graph;
  uint32_t identity;

  char * expected;
  xapi expected_exit;
} graph_parser_test;

//
// public
//

static xapi graph_parser_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(yyutil_load);

  // logging
  fatal(logging_setup);
  fatal(logger_finalize);

  finally : coda;
}

static xapi graph_parser_unit_cleanup(xunit_unit * unit)
{
  xproxy(yyutil_unload);
}

//
// tests
//

static xapi graph_parser_test_entry(xunit_test * _test)
{
  enter;

  graph_parser_test * test = (typeof(test))_test;

  graph * g = 0;
  narrator * N1 = 0;

  // arrange 
  fatal(narrator_growing_create, &N1);

  // act
  xapi exit = invoke(graph_parser_parse, 0, MMS(test->graph), &g, test->identity);
  if(exit)
  {
    if(exit != test->expected_exit)
      fail(0);

    xapi_calltree_unwind();
  }

  // assert
  assert_eq_e(test->expected_exit, exit);
  if(test->expected)
  {
    fatal(graph_say, g, 0, N1);
    const char * actual = narrator_growing_buffer(N1);
    assert_eq_s(test->expected, actual);
  }

finally:
  fatal(narrator_xfree, N1);
  fatal(graph_xfree, g);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : graph_parser_unit_setup
  , xu_cleanup : graph_parser_unit_cleanup
  , xu_entry : graph_parser_test_entry
  , xu_tests : (xunit_test*[]) {
      // unique labels
      (graph_parser_test[]) {{
          graph    : "A:B"
        , expected : "1-A 2-B 1:2"
      }}
    , (graph_parser_test[]) {{
          graph    : "A:B C:D E:F"
        , expected : "1-A 2-B 3-C 4-D 5-E 6-F 1:2 3:4 5:6"
      }}

      // unique labels, attrs
    , (graph_parser_test[]) {{
          graph    : "A!0x42:B"
        , expected : "1-A!0x42 2-B 1:2"
      }}
    , (graph_parser_test[]) {{
          graph    : "A:B!0x42"
        , expected : "1-A 2-B!0x42 1:2"
      }}
    , (graph_parser_test[]) {{
          graph    : "A:0x42:B"
        , expected : "1-A 2-B 1:0x42:2"
      }}

      // non unique labels
    , (graph_parser_test[]) {{
          graph    : "42-B!0x1:72-B!0x2"
        , expected : "1-B!0x1 2-B!0x2 1:2"
      }}

      // not unique labels, identity
    , (graph_parser_test[]) {{
          identity : 8
        , graph    : " 1-A 2-b 3-b"
                     " 1:0x8:2 1:0x1:3"
        , expected :  "1-A 2-b 3-b"
                     " 1:0x8:2 1:0x1:3"
      }}

      // expected failure cases
    , (graph_parser_test[]){{
          identity      : 8
        , graph         : " 1-A 2-B 3-B"
                          " 1:0x8:2"
                          " 1:0x8:3"
        , expected_exit : MORIA_LABELEXISTS
      }}
    , (graph_parser_test[]){{
          identity      : 8
        , graph         : " 1-A 2-B"
                          " 1:0x8:2"
                          " 1:2"
        , expected_exit : MORIA_VERTEXEXISTS
      }}
    , (graph_parser_test[]){{
          identity      : 8
        , graph         : " 1-A 2-B"
                          " 1:2"
                          " 1:0x8:2"
        , expected_exit : MORIA_VERTEXEXISTS
      }}
    , (graph_parser_test[]){{
          identity      : 8
        , graph         : " 1-A 2-B 3-C"
                          " 1:0x8:3"
                          " 2:0x8:3"
        , expected_exit : MORIA_UPEXISTS
      }}

    // inline attrs definitions
    , (graph_parser_test[]){{
          graph     : " X 0x32"
                      " 1-A!X"
        , expected  :  "1-A!0x32"
      }}
    , (graph_parser_test[]){{
          graph     : " X 0x32 Y 0x17"
                      " 1-A!X 2-B 1:Y:2"
        , expected  :  "1-A!0x32 2-B 1:0x17:2"
      }}
    , 0
  }
};
