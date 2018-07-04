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

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"
#include "xapi/errtab.h"
#include "xapi/info.h"

#include "xlinux/xstdlib.h"

struct graph_test;
#define TEST_TYPE struct graph_test
#include "xunit.h"
#include "xunit/assert.h"
#include "valyria/multimap.h"
#include "valyria/array.h"
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "internal.h"
#include "graph.internal.h"
#include "edge.internal.h"
#include "vertex.internal.h"
#include "errtab/MORIA.errtab.h"
#include "operations.internal.h"
#include "parser.internal.h"
#include "logging.internal.h"

#include "macros.h"
#include "zbuffer.h"

#define ATTR_WEAK   0x1
#define ATTR_STRONG 0x2

typedef struct graph_test
{
  xunit_test;

  char * graph;         // starting graph
  char * operations;

  char *    from;
  uint32_t  attrs;
  uint32_t  travel;
  uint32_t  visit;

  char **   expected;   // expected path
  int       cycle;      // expected only that a cycle exists
  char *    cycle_path; // expected cycle path
} graph_test;

typedef struct visit
{
  int position;
  char A;
  char B;
} visit;

static int visitsearch(void * ud, const void * el, size_t idx)
{
  return     ((char*)ud)[0] - ((const visit*)el)->A
          ?: ((char*)ud)[2] - ((const visit*)el)->B
          ;
}

static int visitsort(const void * A, const void * B, void * arg)
{
  return     ((const visit*)A)->A - ((const visit*)B)->A
          ?: ((const visit*)A)->B - ((const visit*)B)->B
          ;
}

static int visit_counter;
static xapi edge_visit(edge * const restrict e, int distance, void * arg)
{
  enter;

  array * visitations = arg;
  visit * visitation;
  fatal(array_push, visitations, &visitation);
  visitation->position = ++visit_counter;
  visitation->A = e->A->label[0];
  visitation->B = e->B->label[0];

  finally : coda;
}

static xapi graph_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_setup);
  fatal(logger_finalize);

  finally : coda;
}

static xapi graph_test_entry(graph_test * test)
{
  enter;

  char cycle_path[128];
  graph * g = 0;
  void * mm_tmp = 0;
  list * operations = 0;
  array * visitations = 0;
  const char ** keys = 0;
  int x;

  fatal(array_create, &visitations, sizeof(visit));

  // setup the test graph
  uint32_t identity = 0;
  if(test->operations)
    identity = 1;

  fatal(graph_parser_graph_create, &g, identity);
  if(test->graph)
    fatal(graph_parser_parse, 0, MMS(test->graph), &g, 0);

  if(test->operations)
  {
    fatal(operations_parser_parse, 0, MMS(test->operations), &operations);
    fatal(operations_perform, g, graph_operations_dispatch, operations);
  }

  // find the starting edge
  vertex * fromv[2] = {};
  for(x = 0; x < graph_vertices(g)->l; x++)
  {
    vertex * v = list_get(graph_vertices(g), x);
    if(test->from && v->label[0] == test->from[0])
      fromv[0] = v;
    if(test->from && v->label[0] == test->from[2])
      fromv[1] = v;

    if(fromv[0] && fromv[1])
      break;
  }

  edge * from = edge_between(fromv[0], fromv[1]);
  assert_notnull(from);

  // perform the traversal
  xapi status = invoke(graph_traverse_edges
    , g
    , from
    , edge_visit
    , 0
    , (traversal_criteria[]) {{
          edge_travel : test->travel
        , edge_visit : test->visit
      }}
    , test->attrs
    , visitations
  );

  if(status)
  {
    // unexpected error
    if(status != MORIA_CYCLE)
      fail(0);

    // cycle not expected for this test
    else if(!test->cycle && !test->cycle_path)
      fail(0);

    xapi_trace_info("path", cycle_path, sizeof(cycle_path));
    xapi_calltree_unwind();
  }

  if(test->expected)
  {
    array_sort(visitations, visitsort, 0);

    char ** expected[2] = { [1] = test->expected };
    while(*expected[1])
    {
      // these vertices were visited after those in the preceeding group
      if(expected[0])
      {
        char * e = *expected[1];
        while(*e)
        {
          visit * evisit = array_search(visitations, e, visitsearch);
          assert_true(evisit);

          char * p = *expected[0];
          while(*p)
          {
            visit * pvisit = array_search(visitations, p, visitsearch);

            assert_gt_d(pvisit->position, evisit->position);
            p += 3;
            if(*p == ' ')
              p++;
          }

          e += 3;
          if(*e == ' ')
            e++;
        }
      }

      // these vertices were visited
      else
      {
        char * p = *expected[1];
        while(*p)
        {
          char * spec = p;
          p += 3;
          if(*p == ' ')
            p++;

          visit * visited = array_search(visitations, spec, visitsearch);

          xapi_info_pushf("edge", "%.*s", (int)(p - spec), spec);
          assert_true(visited);
          xapi_info_unstage();
        }
      }

      expected[0] = expected[1];
      expected[1]++;
    }

    // all visitations were in expected
    for(x = 0; x < visitations->l; x++)
    {
      visit * visitation = array_get(visitations, x);

      char ** expected = test->expected;
      while(*expected)
      {
        char * label = *expected;
        while(*label)
        {
          if(label[0] == visitation->A && label[2] == visitation->B)
            break;

          label++;
        }
        if(*label)
          break;

        expected++;
      }

      bool found = *expected;
      xapi_info_pushf("label", "%c:%c", visitation->A, visitation->B);
      assert_true(found);
      xapi_info_unstage();
    }
  }
  else if(test->cycle || test->cycle_path)
  {
    // appropriate exit status
    assert_eq_e(MORIA_CYCLE, status);

    // appropriate error message
    if(test->cycle_path)
      assert_eq_s(test->cycle_path, cycle_path);
  }

finally:
  fatal(graph_xfree, g);
  wfree(mm_tmp);
  fatal(list_xfree, operations);
  fatal(array_xfree, visitations);
  wfree(keys);
coda;
}

xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_entry = graph_test_entry
  , .xu_tests = (xunit_test*[]) {
        // downward, preorder
        (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "B:C"
          , expected   : (char*[]) { "B:C C:D D:E", 0 }
        }}
        // downward, preorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "B:C"
          , expected   : (char*[]) { "B:C C:D D:E", 0 }
        }}
        // downward, postorder
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from       : "A:B"
          , expected   : (char*[]) { "B:C C:D D:E", "A:B", 0 }
        }}
        // upward, postorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST
          , from       : "C:D"
          , expected   : (char*[]) { "A:B B:C", "C:D", 0 }
        }}

        // visit and travel weak edges
/*
 A
 v
 B .. D
 v    v
 C    E
*/
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +D:2:E +B:1:D"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from       : "D:E"
          , travel     : 3
          , visit      : 3
          , expected   : (char*[]) { "A:B B:D D:E", 0 }
        }}
        // travel but not visit (skip) weak edges
/*
 E F   A
 v v   v
  D .. B
       v
       C
*/
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +E:2:D +D:1:B +F:2:D"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from       : "B:C"
          , travel    : 3
          , visit     : 2
          , expected  : (char*[]) { "A:B B:C E:D F:D", 0 }
        }}
        // visit but not travel (finish) weak edges
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +D:2:E +B:1:D"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , travel     : ATTR_STRONG
          , visit      : ATTR_STRONG | ATTR_WEAK
          , expected   : (char*[]) { "A:B B:C B:D", 0 }
        }}
        // neither visit nor travel (stop) on weak edges, down
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +D:2:E +B:1:D"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , travel     : ATTR_STRONG
          , visit      : ATTR_STRONG
          , expected   : (char*[]) { "A:B B:C", 0 }
        }}
        // neither visit nor travel (stop) on weak edges, up
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +D:2:E +B:1:D"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from       : "D:E"
          , travel     : ATTR_STRONG
          , visit      : ATTR_STRONG
          , expected   : (char*[]) { "D:E", 0 }
        }}

        // cycle detection
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E +E:F +F:G +G:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , cycle_path : "A -> B -> C -> D -> E -> F -> G -> A"
        }}
        // self-referential cycle
      , (graph_test[]){{
            operations : "+A:B +B:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , cycle_path : "A -> B -> A"
        }}
      , 0
  }
};
