/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/list.h"

#include "moria.h"
#include "parser.internal.h"
#include "graph.internal.h"
#include "vertex.h"
#include "dispatch.h"

typedef struct operations_test {
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
  moria_parser * p = 0;
  const char * graph;
  size_t graph_len;
  int status;

  operations_test * test = containerof(_test, operations_test, xu);

  moria_graph_init(&g);
  narrator_growing_create(&N);

  // arrange
  moria_parser_create(&p, &g, 0, 0, moria_operations_dispatch, 0, 0);

  // act
  status = moria_parser_operations_parse(p, MMS(test->operations));
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
    narrator_reset(&N->base);
    moria_graph_say(&g, (llist *[]) { &p->vertices }, 1, (llist *[]) { &p->edges }, 1, 0, 0, 0, 0, 0, &N->base);
    graph = N->s;
    graph_len = N->l;
    assert_eq_w(test->expected, strlen(test->expected), graph, graph_len);
  }

end:
  moria_graph_destroy(&g);
  narrator_growing_free(N);
  moria_parser_free(p);
}

static xunit_unit xunit = {
    .xu_entry = operations_test_entry
  , .xu_tests = (void*)(operations_test*[]) {
        // connect with implicit identity edges
        (operations_test[]){{
            operations : "+A/C:2:B"
          , expected   : "1-A 2-B 3-C 3:0x2:2 3:0x40000000:1"
        }}

        // connect nodes having the same label but different parents on the identity subgraph
      , (operations_test[]){{
            operations : "+B/C:2:A/C"
          , expected   : "1-A 2-B 3-C 4-C 3:0x2:4 4:0x40000000:1 3:0x40000000:2"
        }}

        // connecting identical duplicate edges are ignored (idempotency)
      , (operations_test[]){{
            operations : " +A:2:B +B:2:C +A:2:B +C:2:D"
                         " +A:2:B +A:2:B"
          , expected   : "1-A 2-B 3-C 4-D 1:0x2:2 2:0x2:3 3:0x2:4"
        }}
      , (operations_test[]){{
            operations : "+y/A:y/B +y/B:y/C"
          , expected   : "1-A 2-B 3-C 4-y 1:2 2:3 1:0x40000000:4 2:0x40000000:4 3:0x40000000:4"
        }}

        // disconnect internal edges
      , (operations_test[]){{
            operations : " +A:B +B:D +A:C +D:E"
                         " =D:E"
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
          , expected   : "1-A 2-A 3-B 4-C 5-D 1:3 2:3 1:0x40000000:4 2:0x40000000:5"
        }}
      , (operations_test[]){{
            operations : "+C/A:B +D/A:B +E/A:B"
          , expected   : "1-A 2-A 3-A 4-B 5-C 6-D 7-E 1:4 2:4 3:4 1:0x40000000:5 2:0x40000000:6 3:0x40000000:7"
        }}

      // declare vertices
      , (operations_test[]){{
            operations : "A"
          , expected   : "1-A"
        }}
      , (operations_test[]){{
            operations : "A A"
          , expected   : "1-A"
        }}
      , (operations_test[]){{
            operations : "A B!0x4"
          , expected   : "1-A 2-B!0x4"
        }}
      , (operations_test[]){{
            operations : "A/B/C"
          , expected   : "1-A 2-B 3-C 2:0x40000000:1 3:0x40000000:2"
        }}
      , (operations_test[]){{
            operations : "A/B A/C.c"
          , expected   : "1-A 2-B 3-C.c 2:0x40000000:1 3:0x40000000:1"
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
