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

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/multimap.h"
#include "valyria/list.h"

#include "moria.h"
#include "parser.internal.h"
#include "graph.internal.h"
#include "vertex.h"

typedef struct operations_test
{
  XUNITTEST;

  // setup the graph
  char * operations;

  char * expected;
  int    expected_status;
} operations_test;

static void operations_test_entry(xunit_test * _test)
{
  moria_graph g;
  narrator_growing *N = 0;
  graph_parser * p = 0;
  const char * graph;
  size_t graph_len;
  int status;

  operations_test * test = containerof(_test, operations_test, xu);

  moria_graph_init(&g);
  narrator_growing_create(&N);

  // arrange
  graph_parser_create(&p, &g, 0, graph_operations_dispatch, 0, 0);

  // act
  status = graph_parser_operations_parse(p, MMS(test->operations));
  if(status)
  {
    if(status != test->expected_status) {
      assert_eq_d(0, 1);
      goto end;
    }
  }

  assert_eq_d(test->expected_status, status);
  if(test->expected)
  {
    narrator_xreset(&N->base);
    moria_graph_say(&g, (llist *[]) { &p->vertices }, 1, (llist *[]) { &p->edges }, 1, 0, 0, &N->base);
    graph = N->s;
    graph_len = N->l;
    assert_eq_w(test->expected, strlen(test->expected), graph, graph_len);
  }

end:
  moria_graph_destroy(&g);
  narrator_growing_free(N);
  graph_parser_xfree(p);
}

static xunit_unit xunit = {
    .xu_entry = operations_test_entry
  , .xu_tests = (void*)(operations_test*[]) {
        // connect with implicit identity edges
        (operations_test[]){{
            operations : "+A/C:2:B"
          , expected   : "1-A 2-B 3-C 3:0x2:2 1:0x40000000:3"
        }}

        // connect nodes having the same label but different parents on the identity subgraph
      , (operations_test[]){{
            operations : "+A/C:2:B/C"
          , expected   : "1-A 2-B 3-C 4-C 3:0x2:4 1:0x40000000:3 2:0x40000000:4"
        }}

        // connecting identical duplicate edges are ignored (idempotency)
      , (operations_test[]){{
            operations : " +A:2:B +B:2:C +A:2:B +C:2:D"
                         " +A:2:B +A:2:B"
          , expected   : "1-A 2-B 3-C 4-D 1:0x2:2 2:0x2:3 3:0x2:4"
        }}

#if 0
        // connecting duplicate edges with different attributes causes an error
      , (operations_test[]){{
            operations      : " +A:2:B +B:2:C"
                              " +A:3:B"
          , expected_status : MORIA_VERTEXEXISTS
        }}
#endif

      , (operations_test[]){{
            operations : "+y/A:y/B +y/B:y/C"
          , expected   : "1-A 2-B 3-C 4-y 1:2 2:3 4:0x40000000:1 4:0x40000000:2 4:0x40000000:3"
        }}

        // disconnect internal edges
      , (operations_test[]){{
            operations : "+A:B +B:D +A:C +D:E =D:E"
          , expected   : "1-A 2-B 3-C 4-D 5-E 1:2 1:3 2:4"
        }}

        // disconnect leaf edges
      , (operations_test[]){{
            operations : "+A:B +B:D +A:C +D:E =B:D"
          , expected   : "1-A 2-B 3-C 4-D 5-E 1:2 1:3 4:5"
        }}

        // multiple vertices having the same label
      , (operations_test[]){{
            operations : "+C/A:B +D/A:B"
          , expected   : "1-A 2-A 3-B 4-C 5-D 1:3 2:3 4:0x40000000:1 5:0x40000000:2"
        }}
      , (operations_test[]){{
            operations : "+C/A:B +D/A:B +E/A:B"
          , expected   : "1-A 2-A 3-A 4-B 5-C 6-D 7-E 1:4 2:4 3:4 5:0x40000000:1 6:0x40000000:2 7:0x40000000:3"
        }}

#if 0
        // node B has C downstream on two separate subgraphs
      , (operations_test[]){{
            operations      : "+A/B:2:B/C"
          , expected_status : MORIA_VERTEXEXISTS
        }}
      , (operations_test[]){{
            operations  : " +A:0x40000000:a +A:0x40000000:b +A:0x40000000:c"
                          " +B:0x40000000:a +B:0x40000000:b +B:0x40000000:c"
                          " +C:0x40000000:a +C:0x40000000:b +C:0x40000000:c"
          , expected_status : MORIA_UPEXISTS
        }}
#endif

      // declare vertices
      , (operations_test[]){{
            operations : "A"
          , expected   : "1-A"
        }}
      , (operations_test[]){{
            operations : "A B!0x4"
          , expected   : "1-A 2-B!0x4"
        }}
      , (operations_test[]){{
            operations : "A/B/C"
          , expected   : "1-A 2-B 3-C 1:0x40000000:2 2:0x40000000:3"
        }}
      , (operations_test[]){{
            operations : "A/B A/C.c"
          , expected   : "1-A 2-B 3-C.c 1:0x40000000:2 1:0x40000000:3"
        }}

      , (operations_test[]){{
            operations : " +A,B:C"
          , expected   : "1-A 2-B 3-C 1,2:0x80000000:3"
        }}
      , (operations_test[]){{
            operations : " +A:B,C"
          , expected   : "1-A 2-B 3-C 1:0x80000000:2,3"
        }}

        /* hyper-edges are order-invariant with respect to the vertices specified in the edge */
      , (operations_test[]){{
            operations : " +A,B,C:D,E,F"
          , expected   : "1-A 2-B 3-C 4-D 5-E 6-F 1,2,3:0x80000000:4,5,6"
        }}
      , (operations_test[]){{
            operations :
              " +A,B:D,E"
              " +B,A:E,D" // same edge
          , expected   : "1-A 2-B 3-D 4-E 1,2:0x80000000:3,4"
        }}
      , (operations_test[]){{
            operations :
              " +A,B,C:D,E,F"
              " +C,B,A:F,E,D" // same edge
          , expected   : "1-A 2-B 3-C 4-D 5-E 6-F 1,2,3:0x80000000:4,5,6"
        }}

        /* hyper-edge with zero vertices on a side */
      , (operations_test[]){{
            operations :
              " +A,B,C:_"
          , expected   : "1-A 2-B 3-C 1,2,3:0x80000000:_"
        }}
      , (operations_test[]){{
            operations :
              " +_:A"
          , expected   : "1-A _:0x80000000:1"
        }}
    , 0
  }
};
XUNIT_UNIT(xunit);
