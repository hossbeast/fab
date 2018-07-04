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

typedef struct graph_test
{
  xunit_test;

  char * graph;
  uint32_t identity;
  char * operations;

  char ** labels;

  char * expected_label;
  uint32_t expected_attrs;
} graph_test;

static xapi graph_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_setup);
  fatal(logger_finalize);

  finally : coda;
}

static xapi graph_test_entry(xunit_test * _test)
{
  enter;

  graph * g = 0;
  void * mm_tmp = 0;
  list * operations = 0;
  vertex * actual[2];

  graph_test * test = (void*)_test;

  fatal(graph_parser_graph_create, &g, test->identity);
  if(test->graph)
    fatal(graph_parser_parse, 0, MMS(test->graph), &g, 0);

  if(test->operations)
  {
    fatal(operations_parser_parse, 0, MMS(test->operations), &operations);
    fatal(operations_perform, g, graph_operations_dispatch, operations);
  }

  // act
  int r;
  fatal(graph_lookup
    , g
    , graph_lookup_sentinel
    , (graph_lookup_sentinel_context[]) {{ labels : test->labels }}
    , &mm_tmp
    , actual
    , &r
  );

  if(test->expected_label || test->expected_attrs)
  {
    assert_notnull(actual[0]);
    if(test->expected_label)
      assert_eq_s(test->expected_label, actual[0]->label);
    if(test->expected_attrs)
      assert_eq_u32(test->expected_attrs, actual[0]->attrs);
  }
  else
  {
    assert_null(actual[0]);
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
      // graph lookup on the identity subgraph
        (graph_test[]){{
            identity       : 1
          , operations     : " +x:1:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "x", 0 }
          , expected_label : "x"
        }}
      , (graph_test[]){{
            identity       : 1
          , operations     : " +x:1:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "y", 0 }
          , expected_label : "y"
        }}
      , (graph_test[]){{
            identity       : 1
          , operations     : " +x:1:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "y", "x", 0 }
          , expected_label : "y"
        }}
      , (graph_test[]){{
            identity       : 1
          , operations     : " +x:1:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", 0 }
          , expected_label : "B"
        }}
      , (graph_test[]){{
            identity       : 1
          , operations     : " +x:1:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", "y", 0 }
          , expected_label : "B"
        }}
      , (graph_test[]){{
            identity       : 1
          , operations     : " +x:1:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", "y", "x", 0 }
          , expected_label : "B"
        }}
      , 0
  }
};
