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

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/multimap.h"
#include "valyria/list.h"
#include "logger.h"
#include "yyutil/load.h"
#include "xapi/calltree.h"

#include "internal.h"
#include "operations.internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "logging.internal.h"
#include "MORIA.errtab.h"

typedef struct operations_test
{
  xunit_test;

  char * operations;
  uint32_t identity;

  char * expected;
  xapi   expected_status;
} operations_test;

static xapi operations_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(yyutil_load);

  // logging
  fatal(logging_setup);
  fatal(logger_finalize);

  finally : coda;
}

static xapi operations_unit_cleanup(xunit_unit * unit)
{
  xproxy(yyutil_unload);
}

static xapi operations_test_entry(xunit_test * _test)
{
  enter;

  graph * g = 0;
  narrator * N = 0;
  list * operations = 0;

  operations_test * test = (typeof(test))_test;

  fatal(graph_create, &g, test->identity ?: 1);
  fatal(narrator_growing_create, &N);

  // arrange
  fatal(operations_parser_parse, 0, MMS(test->operations), &operations);

  // act
  xapi status = invoke(operations_perform, g, graph_operations_dispatch, operations);
  if(status)
  {
    if(status != test->expected_status)
      fail(0);

    xapi_calltree_unwind();
  }

  assert_eq_e(test->expected_status, status);
  if(test->expected)
  {
    fatal(narrator_xreset, N);
    fatal(graph_say, g, 0, N);
    const char * graph = narrator_growing_buffer(N);
    size_t graph_len = narrator_growing_size(N);
    assert_eq_w(test->expected, strlen(test->expected), graph, graph_len);
  }

finally:
  fatal(graph_xfree, g);
  fatal(narrator_xfree, N);
  fatal(list_xfree, operations);
coda;
}

xunit_unit xunit = {
    .xu_setup = operations_unit_setup
  , .xu_cleanup = operations_unit_cleanup
  , .xu_entry = operations_test_entry
  , .xu_tests = (xunit_test*[]) {
        // connect with implicit identity edges
        (operations_test[]){{
            operations : "+A/C:2:B"
          , expected   : "1-A 2-B 3-C 1:0x1:3 3:0x2:2"
        }}

        // connect nodes having the same label but different parents on the identity subgraph
      , (operations_test[]){{
            operations : "+A/C:2:B/C"
          , expected   : "1-A 2-B 3-C 4-C 1:0x1:3 2:0x1:4 3:0x2:4"
        }}

        // connecting identical duplicate edges are ignored (idempotency)
      , (operations_test[]){{
            operations : " +A:2:B +B:2:C +A:2:B +C:2:D"
                         " +A:2:B +A:2:B"
          , expected   : "1-A 2-B 3-C 4-D 1:0x2:2 2:0x2:3 3:0x2:4"
        }}

        // connecting duplicate edges with different attributes causes an error
      , (operations_test[]){{
            operations      : " +A:2:B +B:2:C"
                              " +A:3:B"
          , expected_status : MORIA_VERTEXEXISTS
        }}

      , (operations_test[]){{
            operations : "+y/A:y/B +y/B:y/C"
          , expected   : "1-A 2-B 3-C 4-y 1:2 2:3 4:0x1:1 4:0x1:2 4:0x1:3"
        }}

        // disconnect internal edges
      , (operations_test[]){{
            operations : "+A:B +B:D +A:C +D:E -D:E"
          , expected   : "1-A 2-B 3-C 4-D 5-E 1:2 1:3 2:4"
        }}

        // disconnect leaf edges
      , (operations_test[]){{
            operations : "+A:B +B:D +A:C +D:E -B:D"
          , expected   : "1-A 2-B 3-C 4-D 5-E 1:2 1:3 4:5"
        }}

        // multiple vertices having the same label
      , (operations_test[]){{
            operations : "+C/A:B +D/A:B"
          , expected   : "1-A 2-A 3-B 4-C 5-D 1:3 2:3 4:0x1:1 5:0x1:2"
        }}
      , (operations_test[]){{
            operations : "+C/A:B +D/A:B +E/A:B"
          , expected   : "1-A 2-A 3-A 4-B 5-C 6-D 7-E 1:4 2:4 3:4 5:0x1:1 6:0x1:2 7:0x1:3"
        }}

        // disintegrate
      , (operations_test[]){{
            operations : " +A:B +B:C +C:D +C:E"
                         " ~B:C"
          , expected   : "1-A 2-B 3-C 4-D 5-E 1:2"
        }}
/*
 /x          A
  /y         |
   /A        B     <<
   /B        |     <<
   /C        C     <<
   /D       / \
   /E      D   E
*/
      , (operations_test[]){{
            operations : " +x:1:y"
                         " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
                         " ~B:C"
          , expected   :  "1-A 2-B 3-C 4-D 5-E 6-x 7-y"
                         " 1:2"
                         " 6:0x1:7 7:0x1:1 7:0x1:2 7:0x1:3 7:0x1:4 7:0x1:5"
        }}
      , (operations_test[]){{
            operations : " +x:1:y"
                         " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
                         " ~x:y"
          , expected   :  "1-A 2-B 3-C 4-D 5-E 6-x 7-y"
                         " 1:2 2:3 3:4 3:5"
        }}

        // node B has C downstream on two separate subgraphs
      , (operations_test[]){{
            operations      : "+A/B:2:B/C"
          , expected_status : MORIA_VERTEXEXISTS
        }}
      , (operations_test[]){{
            identity    : 8
          , operations  : " +A:0x8:a +A:0x8:b +A:0x8:c"
                          " +B:0x8:a +B:0x8:b +B:0x8:c"
                          " +C:0x8:a +C:0x8:b +C:0x8:c"
          , expected_status : MORIA_UPEXISTS
        }}

      // inline attrs definitions
      , (operations_test[]){{
            operations : " X 0x8"
                         " +A:X:a"
          , expected   : "1-A 2-a 1:0x8:2"
        }}

      // declare vertices
      , (operations_test[]){{
            operations : "A"
          , expected   : "1-A"
        }}
      , (operations_test[]){{
            operations : "A B!0x42"
          , expected   : "1-A 2-B!0x42"
        }}
      , (operations_test[]){{
            operations : "A/B/C"
          , expected   : "1-A 2-B 3-C 1:0x1:2 2:0x1:3"
        }}
      , (operations_test[]){{
            operations : "A/B A/C.c"
          , identity   : 8
          , expected   : "1-A 2-B 3-C.c 1:0x8:2 1:0x8:3"
        }}

    , 0
  }
};
