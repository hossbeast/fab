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

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/map.h"
#include "valyria/multimap.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "moria/load.h"

#include "moria.h"
#include "graph.internal.h"
#include "logging.internal.h"
#include "operations.h"
#include "parser.internal.h"
#include "vertex.h"

#include "xlinux/KERNEL.errtab.h"

typedef struct graph_test {
  XUNITTEST;

  char * graph;       // starting graph
  char * operations;

  char * down_from;
  char * down_label;

  char * expected;
  uint32_t expected_attrs;
} graph_test;

static xapi graph_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(moria_load);
  fatal(logger_finalize);

  finally : coda;
}

static xapi graph_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(moria_unload);

  finally : coda;
}

//
// TESTs for vertex_identity_travel
//

static xapi graph_test_entry(xunit_test * _test)
{
  enter;

  moria_graph g;
  graph_parser * p = 0;
  moria_vertex * from = 0;
  moria_vertex * v;

  graph_test * test = containerof(_test, graph_test, xu);

  moria_graph_init(&g);
  fatal(graph_parser_create, &p, &g, 0, graph_operations_dispatch, 0, 0);

  if(test->graph) {
    fatal(graph_parser_parse, p, MMS(test->graph));
  }

  if(test->operations) {
    fatal(graph_parser_operations_parse, p, MMS(test->operations));
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

finally:
  moria_graph_destroy(&g);
  fatal(graph_parser_xfree, p);
coda;
}

xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_cleanup = graph_unit_cleanup
  , .xu_entry = graph_test_entry
  , .xu_tests = (void*)(graph_test*[]) {
      // find identity edges by label
        (graph_test[]){{
            operations  : " +A:0x40000000:a +A:0x40000000:b +A:0x40000000:c"
          , down_from   : "A"
          , down_label  : "a"
          , expected    : "a"
        }}
      , (graph_test[]){{
            operations  : " +A:0x40000000:a +A:0x40000000:b +A:0x40000000:c"
          , down_from   : "A"
          , down_label  : "b"
          , expected    : "b"
        }}
      , (graph_test[]){{
            operations  : " +A:0x40000000:a +A:0x40000000:b +A:0x40000000:c"
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
            graph       : "A:0x40000000:B"
          , down_from   : "A"
          , down_label  : "C"
        }}
      , (graph_test[]){{
            graph       : "A:0x40000000:B"
          , down_from   : "A"
          , down_label  : "C"
        }}
      , (graph_test[]){{
            graph       : " 1-A 2-B 3-C 4-D"
                          " 1:0x40000000:2 1:0x40000000:3"
          , down_from   : "A"
          , down_label  : "D"
        }}
      , (graph_test[]){{
            graph       : " 1-A 2-B 3-C 4-D"
                          " 1:0x40000000:2 1:0x40000000:3"
          , down_from   : "A"
          , down_label  : "D"
        }}
      // edges not unique by down_label - mixing identity and non identity edges
      , (graph_test[]){{
            graph          : " 1-A 4-a 5-b!0x42 6-c 9-b"
                             " 1:0x40000000:4 1:0x40000000:5 1:0x40000000:6 1:0x1:9"
          , down_from      : "A"
          , down_label     : "b"
          , expected       : "b"
          , expected_attrs : 0x42
        }}
      , (graph_test[]){{
            graph          : " 3-C 4-a 5-b 6-c!0x42 8-b 9-c"
                             " 3:0x40000000:4 3:0x40000000:5 3:0x40000000:6 3:0x1:9"
          , down_from      : "C"
          , down_label     : "c"
          , expected       : "c"
          , expected_attrs : 0x42
        }}

      , (graph_test[]){{
            graph          : " 1-A 2-B!0x7 3-B 4-B"
                             " 1:0x40000000:2 1:3 1:4"
          , down_from      : "A"
          , down_label     : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , (graph_test[]){{
            graph          : " 1-A 2-B 3-B!0x7 4-B"
                             " 1:2 1:0x40000000:3 1:4"
          , down_from      : "A"
          , down_label     : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , (graph_test[]){{
            graph          : " 1-A 2-B 3-B 4-B!0x7"
                             " 1:2 1:3 1:0x40000000:4"
          , down_from      : "A"
          , down_label     : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , 0
  }
};
