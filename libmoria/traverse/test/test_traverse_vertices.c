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

#include "xlinux/xstdlib.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "valyria/multimap.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/array.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "errtab/MORIA.errtab.h"
#include "operations.internal.h"
#include "parser.internal.h"
#include "logging.internal.h"

#include "macros.h"

typedef struct graph_test
{
  xunit_test;

  char * graph;         // starting graph
  char * operations;

  char *    from_label;
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
  char label;
} visit;

static int visitsearch(void * ud, const void * el, size_t idx)
{
  return *(char*)ud - ((const visit*)el)->label;
}

static int visitsort(const void * A, const void * B, void * arg)
{
  return ((const visit*)A)->label - ((const visit*)B)->label;
}

static xapi graph_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_setup);
  fatal(logger_finalize);

  finally : coda;
}

static int visit_counter;
static xapi vertex_visit(vertex * const restrict v, int level, void * arg)
{
  enter;

  array * visitations = arg;

  visit * visitation;
  fatal(array_push, visitations, &visitation);
  visitation->label = v->label[0];
  visitation->position = ++visit_counter;

  finally : coda;
}

static xapi graph_test_entry(xunit_test * _test)
{
  enter;

  graph_test * test = (typeof(test))_test;

  char space[128];
  graph * g = 0;
  void * mm_tmp = 0;
  list * operations = 0;
  array * visitations = 0;
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

  vertex * from = 0;
  for(x = 0; x < graph_vertices(g)->l; x++)
  {
    vertex * v = list_get(graph_vertices(g), x);
    if(test->from_label && strcmp(v->label, test->from_label) == 0)
      from = v;

    if(from)
      break;
  }

  // perform the traversal
  xapi status = invoke(graph_traverse_vertices
    , g
    , from 
    , vertex_visit
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

    // save the trace
    xapi_trace_info("path", space, sizeof(space));
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
            p++;
          }

          e++;
        }
      }

      // these vertices were visited
      else
      {
        char * e = *expected[1];
        while(*e)
        {
          visit * visited = array_search(visitations, e, visitsearch);
          assert_true(visited);

          e++;
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
          if(*label == visitation->label)
            break;

          label++;
        }
        if(*label)
          break;

        expected++;
      }

      bool found = *expected;
      xapi_info_pushf("label", "%c", visitation->label);
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
      assert_eq_s(test->cycle_path, space);
  }

finally:
  fatal(graph_xfree, g);
  wfree(mm_tmp);
  fatal(list_xfree, operations);
  fatal(array_xfree, visitations);
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
          , from_label : "A"
          , expected   : (char*[]) { "ABCDE", 0 }
        }}
/*
 A
 |\
 v |
 B |
  \|
   v
   C
   v
   D
   v
   E
*/
        // downward, preorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E" 
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "B"
          , expected   : (char*[]) { "B", "C", "D", "E", 0 }
        }}
        // downward, postorder
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E" 
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from_label : "A"
          , expected   : (char*[]) { "E", "D", "C", "B", "A", 0 }
        }}
        // downward, postorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from_label : "C"
          , expected   : (char*[]) { "EDC", 0 }
        }}
/*
 A
 |
 v
 B..>D
 |   |
 v   v
 C   E
*/
        // visit and travel weak edges
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 1:0x1:2 3:0x1:4"  // strong
                        " 1:0x2:3"                  // weak
          , from_label: "A"
          , attrs     : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , travel    : 0x3   // weak | strong
          , visit     : 0x3
          , expected  : (char*[]) { "A", "BCDE", 0 }
        }}
        // visit but not travel (finish) weak edges
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 1:0x1:2 3:0x1:4"          // strong
                        " 1:0x2:3"                          // weak
          , attrs     : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label: "A"
          , travel    : 0x1 // ATTR_STRONG
          , visit     : 0x3 // ATTR_STRONG | ATTR_WEAK
          , expected  : (char*[]) { "A", "BCD", 0 }
        }}
/*
  E  F A
  | /  |
  vv   v
  D...>B
       |   
       v   
       C   
*/
        // travel but not visit (skip) weak edges
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E 5-F"
                        " 0:0x1:1 1:0x1:2 4:0x1:3 5:0x1:3"  // strong
                        " 3:0x2:1"                          // weak
          , attrs     : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from_label: "C"
          , travel    : 0x3 // ATTR_STRONG | ATTR_WEAK
          , visit     : 0x1 // ATTR_STRONG
          , expected  : (char*[]) { "C", "B", "AEF", 0 }
        }}
        // neither visit nor travel (stop) on weak edges, down
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 1:0x1:2 3:0x1:4"          // strong
                        " 1:0x2:3"                          // weak
          , attrs     : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label: "A"
          , travel    : 0x1 // ATTR_STRONG
          , visit     : 0x1 // ATTR_STRONG
          , expected  : (char*[]) { "A", "BC", 0 }
        }}
        // neither visit nor travel (stop) on weak edges, up
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 1:0x1:2 3:0x1:4"          // strong
                        " 1:0x2:3"                          // weak
          , attrs     : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from_label: "E"
          , travel    : 0x1 // ATTR_STRONG
          , visit     : 0x1 // ATTR_STRONG
          , expected  : (char*[]) { "E", "D", 0 }
        }}

        // cycle detection
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E +E:F +F:G +G:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , cycle_path : "A -> B -> C -> D -> E -> F -> G -> A"
        }}
        // self-referential cycle
      , (graph_test[]){{
            operations : "+A:B +B:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , cycle_path : "A -> B -> A"
        }}
        // cycle which exceeds the size of the cycle reporting stack
      , (graph_test[]){{
            operations : "+a:b +b:c +c:d +d:e +e:f +f:g +g:h +h:i +i:j +j:k +k:l +l:m +m:n +n:o +o:p +p:q +q:r +r:s +s:t"
                         "+t:u +u:v +v:w +w:x +x:y +y:z +z:A +A:B +B:C +C:D +D:E +E:F +F:G +G:H +H:I +I:J +J:K +K:L +L:M"
                         "+M:N +N:O +O:P +P:Q +Q:R +R:S +S:T +T:U +U:V +V:W +W:X +X:Y +Y:Z +Z:a"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "a"
          , cycle      : 1
        }}
      , 0
  }
};
