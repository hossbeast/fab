/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"
#include "moria/load.h"

#include "moria.h"
#include "graph.internal.h"
#include "parser.internal.h"
#include "vertex.h"
#include "dispatch.h"

typedef struct graph_test {
  XUNITTEST;

  char * graph;       // starting graph
  char * operations;

  char * down_from;
  char * down_label;

  char * expected;
  uint32_t expected_attrs;
} graph_test;

static void graph_unit_setup(xunit_unit * unit)
{
  moria_load();
}

static void graph_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
}

//
// TESTs for vertex_identity_travel
//

static void graph_test_entry(xunit_test * _test)
{
  moria_graph g;
  moria_parser * p = 0;
  moria_vertex * from = 0;
  moria_vertex * v;
  int r;

  graph_test * test = containerof(_test, graph_test, xu);

  moria_graph_init(&g);
  moria_parser_create(&p, &g, 0, 0, moria_operations_dispatch, 0, 0);

  if(test->graph) {
    moria_parser_graph_parse(p, MMS(test->graph));
  }

  if(test->operations) {
    r = moria_parser_operations_parse(p, MMS(test->operations));
    if(r == 1) {
      printf("%s\n", p->operations_yyu.error_str);
    }
  }

  llist_foreach(&p->vertices, v, owner) {
    if(test->down_from && strcmp(v->label, test->down_from) == 0)
    {
      from = v;
      break;
    }
  }

  assert_notnull(from);

  // act
  moria_vertex * actual = moria_vertex_downs(from, test->down_label);

  if(test->expected)
  {
    assert_notnull(actual);
    assert_eq_s(test->expected, actual->label);
    if(test->expected_attrs)
      assert_eq_u64(test->expected_attrs, actual->attrs);

    // reverse traversal
    moria_vertex * reverse = moria_vertex_up(actual);
    assert_eq_p(from, reverse);
  }
  else
  {
    assert_null(actual);
  }

  moria_graph_destroy(&g);
  moria_parser_free(p);
}

static xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_cleanup = graph_unit_cleanup
  , .xu_entry = graph_test_entry
  , .xu_tests = (void*)(graph_test*[]) {
      // find identity edges by label
        (graph_test[]){{
            operations  : " +a:0x40000000:A +b:0x40000000:A +c:0x40000000:A"
          , down_from   : "A"
          , down_label  : "a"
          , expected    : "a"
        }}
      , (graph_test[]){{
            operations  : " +a:0x40000000:A +b:0x40000000:A +c:0x40000000:A"
          , down_from   : "A"
          , down_label  : "b"
          , expected    : "b"
        }}
      , (graph_test[]){{
            operations  : " +a:0x40000000:A +b:0x40000000:A +c:0x40000000:A"
          , down_from   : "A"
          , down_label  : "c"
          , expected    : "c"
        }}
      // not found - not identity edges
      , (graph_test[]){{
            graph       : "2-B:0x4:1-A"
          , down_from   : "B"
          , down_label  : "A"
        }}
      , (graph_test[]){{
            graph       : "2-B:1-A"
          , down_from   : "B"
          , down_label  : "A"
        }}
      , (graph_test[]){{
            graph       : "1-A:2-B"
          , down_from   : "A"
          , down_label  : "B"
        }}
      // not found - no such down_label
      , (graph_test[]){{
            graph       : "B:0x40000000:A"
          , down_from   : "A"
          , down_label  : "C"
        }}
      , (graph_test[]){{
            graph       : "B:0x40000000:A"
          , down_from   : "A"
          , down_label  : "C"
        }}
      , (graph_test[]){{
            graph       : " 1-A 2-B 3-C 4-D"
                          " 2:0x40000000:1 3:0x40000000:1"
          , down_from   : "A"
          , down_label  : "D"
        }}
      , (graph_test[]){{
            graph       : " 1-A 2-B 3-C 4-D"
                          " 2:0x40000000:1 3:0x40000000:1"
          , down_from   : "A"
          , down_label  : "D"
        }}
      // edges not unique by down_label - mixing identity and non identity edges
      , (graph_test[]){{
            graph          : " 1-A 4-a 5-b!0x42 6-c 9-b"
                             " 4:0x40000000:1 5:0x40000000:1 6:0x40000000:1 1:0x1:9"
          , down_from      : "A"
          , down_label     : "b"
          , expected       : "b"
          , expected_attrs : 0x42
        }}
      , (graph_test[]){{
            graph          : " 3-C 4-a 5-b 6-c!0x42 8-b 9-c"
                             " 4:0x40000000:3 5:0x40000000:3 6:0x40000000:3 3:0x1:9"
          , down_from      : "C"
          , down_label     : "c"
          , expected       : "c"
          , expected_attrs : 0x42
        }}

      , (graph_test[]){{
            graph          : " 1-A 2-B!0x7 3-B 4-B"
                             " 2:0x40000000:1 1:3 1:4"
          , down_from      : "A"
          , down_label     : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , (graph_test[]){{
            graph          : " 1-A 2-B 3-B!0x7 4-B"
                             " 1:2 3:0x40000000:1 1:4"
          , down_from      : "A"
          , down_label     : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , (graph_test[]){{
            graph          : " 1-A 2-B 3-B 4-B!0x7"
                             " 1:2 1:3 4:0x40000000:1"
          , down_from      : "A"
          , down_label     : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , 0
  }
};
XUNIT_UNIT(xunit);
