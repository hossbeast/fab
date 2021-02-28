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
#include "moria/load.h"
#include "logger.h"

#include "graph.h"
#include "moria.h"
#include "parser.internal.h"
#include "MORIA.errtab.h"

#include "macros.h"

typedef struct graph_parser_test {
  XUNITTEST;

  char * graph;

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
  fatal(moria_load);

  // logging
  fatal(logger_finalize);

  finally : coda;
}

static xapi graph_parser_unit_cleanup(xunit_unit * unit)
{
  xproxy(moria_unload);
}

//
// tests
//

static xapi graph_parser_test_entry(xunit_test * _test)
{
  enter;

  graph_parser_test * test = (typeof(test))_test;

  moria_graph g;
  graph_parser * p = 0;
  narrator_growing * N1 = 0;
  const char * actual;
  size_t actual_len;

  // arrange
  moria_graph_init(&g);
  fatal(graph_parser_create, &p, &g, 0, graph_operations_dispatch, 0, 0);

  // act
  xapi exit = invoke(graph_parser_parse, p, MMS(test->graph));
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
    fatal(narrator_growing_create, &N1);
    fatal(moria_graph_say, &g, (llist *[]) { &p->vertices }, 1, (llist *[]) { &p->edges }, 1, 0, 0, &N1->base);
    actual = N1->s;
    actual_len = N1->l;
    assert_eq_w(test->expected, strlen(test->expected), actual, actual_len);
  }

finally:
  moria_graph_destroy(&g);
  fatal(narrator_growing_free, N1);
  fatal(graph_parser_xfree, p);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : graph_parser_unit_setup
  , xu_cleanup : graph_parser_unit_cleanup
  , xu_entry : graph_parser_test_entry
  , xu_tests : (void*)(graph_parser_test*[]) {
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
          graph    : "A:B!0x4"
        , expected : "1-A 2-B!0x4 1:2"
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
          graph    : " 1-A 2-b 3-b"
                     " 1:0x40000000:2 1:0x1:3"
        , expected :  "1-A 2-b 3-b"
                     " 1:0x1:3 1:0x40000000:2"
      }}

      // expected failure cases
    , (graph_parser_test[]){{
          graph         : " 1-A 2-B 3-B"
                          " 1:0x40000000:2"
                          " 1:0x40000000:3"
        , expected_exit : MORIA_LABELEXISTS
      }}
    , (graph_parser_test[]){{
          graph         : " 1-A 2-B"
                          " 1:0x40000000:2"
                          " 1:2"
        , expected_exit : MORIA_VERTEXEXISTS
      }}
    , (graph_parser_test[]){{
          graph         : " 1-A 2-B"
                          " 1:2"
                          " 1:0x40000000:2"
        , expected_exit : MORIA_VERTEXEXISTS
      }}
    , (graph_parser_test[]){{
          graph         : " 1-A 2-B 3-C"
                          " 1:0x40000000:3"
                          " 2:0x40000000:3"
        , expected_exit : MORIA_UPEXISTS
      }}

      // hyper-edges
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C"
                      " 1:0x80000000:2,3"
        , expected  :  "1-A 2-B 3-C"
                      " 1:0x80000000:2,3"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C"
                      " 1:0x80420000:2,3"
        , expected  :  "1-A 2-B 3-C"
                      " 1:0x80420000:2,3"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1:0x80000000:2,3,4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1:0x80000000:2,3,4"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C"
                      " 1,2:0x80000000:3"
        , expected  :  "1-A 2-B 3-C"
                      " 1,2:0x80000000:3"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2,3:0x80000000:4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2,3:0x80000000:4"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2,3:0x84200000:4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2,3:0x84200000:4"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2:0x80000000:3,4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2:0x80000000:3,4"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2:0x84200000:3,4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2:0x84200000:3,4"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x80000000:4,5,6"
        , expected  :  "1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x80000000:4,5,6"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x84200000:4,5,6"
        , expected  :  "1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x84200000:4,5,6"
      }}
      // hyper-edges, zero-to-many
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B"
                      " 1,2:0x80000000:_"
        , expected  :  "1-A 2-B"
                      " 1,2:0x80000000:_"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B"
                      " _:0x80000000:1,2"
        , expected  :  "1-A 2-B"
                      " _:0x80000000:1,2"
      }}
    , (graph_parser_test[]){{
          graph     : " 1-A 2-B"
                      " _:0x80000000:1"
        , expected  :  "1-A 2-B"
                      " _:0x80000000:1"
      }}
    , 0
  }
};
