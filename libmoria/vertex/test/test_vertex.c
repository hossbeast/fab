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

#include "internal.h"
#include "graph.internal.h"
#include "logging.internal.h"
#include "operations.internal.h"
#include "parser.internal.h"
#include "vertex.internal.h"

#include "xlinux/KERNEL.errtab.h"

#define IDENTITY 0x8

typedef struct graph_test
{
  xunit_test;

  char * graph;       // starting graph
  char * operations;
  uint32_t identity;

  char * from;
  char * label;

  char * expected;
  uint32_t expected_attrs;
} graph_test;

static xapi graph_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_setup);
  fatal(logger_finalize);

  finally : coda;
}

//
// TESTs for vertex_identity_travel
//

static xapi graph_test_entry(xunit_test * _test)
{
  enter;

  graph * g = 0;
  vertex * from = 0;
  void * mm_tmp = 0;
  list * operations = 0;
  int x;

  graph_test * test = (void*)_test;

  fatal(graph_parser_graph_create, &g, test->identity);
  if(test->graph)
    fatal(graph_parser_parse, 0, MMS(test->graph), &g, 0);

  if(test->operations)
  {
    fatal(operations_parser_parse, 0, MMS(test->operations), &operations);
    fatal(operations_perform, g, graph_operations_dispatch, operations);
  }

  for(x = 0; x < graph_vertices(g)->l; x++)
  {
    vertex * v = list_get(graph_vertices(g), x);
    if(test->from && strcmp(v->label, test->from) == 0)
    {
      from = v;
      break;
    }
  }

  assert_notnull(from);

  // act
  vertex * actual = vertex_downs(from, test->label);

  if(test->expected)
  {
    assert_notnull(actual);
    if(test->expected)
      assert_eq_s(test->expected, actual->label);
    if(test->expected_attrs)
      assert_eq_u64(test->expected_attrs, actual->attrs);

    // reverse traversal
    vertex * reverse = vertex_up(actual);
    assert_eq_p(from, reverse);
  }
  else
  {
    assert_null(actual);
  }

finally:
  fatal(graph_xfree, g);
  wfree(mm_tmp);
  fatal(list_xfree, operations);
coda;
}

xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_entry = graph_test_entry
  , .xu_tests = (xunit_test*[]) {
      // find identity edges by label
        (graph_test[]){{
            identity    : 8
          , operations  : " +A:0x8:a +A:0x8:b +A:0x8:c"
          , from        : "A"
          , label       : "a"
          , expected    : "a"
        }}
      , (graph_test[]){{
            identity    : 8
          , operations  : " +A:0x8:a +A:0x8:b +A:0x8:c"
          , from        : "A"
          , label       : "b"
          , expected    : "b"
        }}
      , (graph_test[]){{
            identity    : 8
          , operations  : " +A:0x8:a +A:0x8:b +A:0x8:c"
          , from        : "A"
          , label       : "c"
          , expected    : "c"
        }}
      // not found - not identity edges
      , (graph_test[]){{
            graph    : "2-B:0x8:1-A"
          , from     : "B"
          , label    : "A"
        }}
      , (graph_test[]){{
            identity : 8
          , graph    : "2-B:1-A"
          , from     : "B"
          , label    : "A"
        }}
      , (graph_test[]){{
            identity : 8
          , graph    : "1-A:2-B"
          , from     : "A"
          , label    : "B"
        }}
      // not found - no such label
      , (graph_test[]){{
            identity : 8
          , graph    : "A:0x8:B"
          , from     : "A"
          , label    : "C"
        }}
      , (graph_test[]){{
            identity  : 8
          , graph     : "A:0x8:B"
          , from      : "A"
          , label     : "C"
        }}
      , (graph_test[]){{
            identity  : 8
          , graph     : " 1-A 2-B 3-C 4-D"
                        " 1:0x8:2 1:0x8:3"
          , from      : "A"
          , label     : "D"
        }}
      , (graph_test[]){{
            identity  : 8
          , graph     : " 1-A 2-B 3-C 4-D"
                        " 1:0x8:2 1:0x8:3"
          , from      : "A"
          , label     : "D"
        }}
      // edges not unique by label - mixing identity and non identity edges
      , (graph_test[]){{
            identity       : 8
          , graph          : " 1-A 4-a 5-b!0x42 6-c 9-b"
                             " 1:0x8:4 1:0x8:5 1:0x8:6 1:0x1:9"
          , from           : "A"
          , label          : "b"
          , expected       : "b"
          , expected_attrs : 0x42
        }}
      , (graph_test[]){{
            identity       : 8
          , graph          : " 3-C 4-a 5-b 6-c!0x42 8-b 9-c"
                             " 3:0x8:4 3:0x8:5 3:0x8:6 3:0x1:9"
          , from           : "C"
          , label          : "c"
          , expected       : "c"
          , expected_attrs : 0x42
        }}

      , (graph_test[]){{
            identity       : 8
          , graph          : " 1-A 2-B!0x7 3-B 4-B"
                             " 1:0x8:2 1:3 1:4"
          , from           : "A"
          , label          : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , (graph_test[]){{
            identity       : 8
          , graph          : " 1-A 2-B 3-B!0x7 4-B"
                             " 1:2 1:0x8:3 1:4"
          , from           : "A"
          , label          : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , (graph_test[]){{
            identity       : 8
          , graph          : " 1-A 2-B 3-B 4-B!0x7"
                             " 1:2 1:3 1:0x8:4"
          , from           : "A"
          , label          : "B"
          , expected       : "B"
          , expected_attrs : 7
        }}
      , 0
  }
};
