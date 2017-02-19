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

struct graph_test;
#define TEST_TYPE struct graph_test
#include "xunit.h"
#include "xunit/assert.h"
#include "valyria/map.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "errtab/MORIA.errtab.h"

#include "macros.h"

#define ATTR_WEAK 0x01
#define ATTR_STRONG 0x02

typedef struct graph_test
{
  xunit_test;

  char * constr;

  uint32_t  travel;
  uint32_t  visit;

  uint32_t  attrs;
  char *    from;
  char *    expected;   // expected path
  int       cycle;      // expected only that a cycle exists
  char *    cycle_path; // expected cycle path
} graph_test;

static xapi vertex_visit(vertex * const restrict v, int level, void * arg)
{
  enter;

  narrator * N = arg;
  says(v->label);

  finally : coda;
}

static xapi graph_test_entry(graph_test * test)
{
  enter;

  char space[128];
  graph * g = 0;
  map * vertices = 0;
  narrator * N = 0;

  fatal(graph_create, &g);
  fatal(map_create, &vertices);
  fatal(narrator_growing_create, &N);

  // setup the test graph
  char * c = test->constr;
  while(*c)
  {
    vertex * A;
    if((A = map_get(vertices, MM(*c))) == 0)
    {
      fatal(graph_vertex_createw, &A, g, MM(*c), 0);
      fatal(map_set, vertices, MM(*c), A);
    }
    c++;

    uint32_t attrs = 0;
    if(*c == '!' || *c == '~')
    {
      if(*c == '!')
        attrs = ATTR_STRONG;
      if(*c == '~')
        attrs = ATTR_WEAK;
      c++;
    }

    vertex * B;
    if((B = map_get(vertices, MM(*c))) == 0)
    {
      fatal(graph_vertex_createw, &B, g, MM(*c), 0);
      fatal(map_set, vertices, MM(*c), B);
    }
    c++;

    fatal(graph_connect_edge, g, A, B, attrs);

    while(*c == ' ')
      c++;
  }

  // perform the traversal
  xapi exit = invoke(graph_traverse_vertices
    , g
    , map_get(vertices, MMS(test->from))
    , vertex_visit
    , 0
    , 0
    , 0
    , test->travel
    , test->visit
    , test->attrs
    , N
  );
  if(exit)
  {
    // unexpected error
    if(exit != MORIA_CYCLE)
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
    assert_eq_s(test->expected, narrator_growing_buffer(N));
  }
  else if(test->cycle || test->cycle_path)
  {
    // appropriate exit status
    assert_eq_e(MORIA_CYCLE, exit);

    // appropriate error message
    if(test->cycle_path)
      assert_eq_s(test->cycle_path, space);
  }

finally:
  fatal(graph_xfree, g);
  fatal(map_xfree, vertices);
  fatal(narrator_xfree, N);
coda;
}

xunit_unit xunit = {
    .xu_entry = graph_test_entry
  , .xu_tests = (xunit_test*[]) {
        // downward, preorder
        (graph_test[]){{
            constr    : "AB BC AC CD DE"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "A"
          , expected  : "ABCDE"
        }}
        // downward, preorder, from another node
      , (graph_test[]){{
            constr    : "AB BC AC CD DE"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "B"
          , expected  : "BCDE"
        }}
        // downward, postorder
      , (graph_test[]){{
            constr    : "AB BC AC CD DE"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_POST
          , from      : "A"
          , expected  : "EDCBA"
        }}
        // downward, postorder, from another node
      , (graph_test[]){{
            constr    : "AB BC AC CD DE"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_POST
          , from      : "C"
          , expected  : "EDC"
        }}

        // visit and travel weak edges
      , (graph_test[]){{
            constr    : "A!B B!C D!E B~D"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "A"
          , travel    : ATTR_STRONG | ATTR_WEAK
          , visit     : ATTR_STRONG | ATTR_WEAK
          , expected  : "ABCDE"
        }}
        // travel but not visit (skip) weak edges
      , (graph_test[]){{
            constr    : "A!B B!C E!D D~B F!D"
          , attrs     : GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_PRE
          , from      : "C"
          , travel    : ATTR_STRONG | ATTR_WEAK
          , visit     : ATTR_STRONG
          , expected  : "CBAEF"
        }}
        // visit but not travel (finish) weak edges
      , (graph_test[]){{
            constr    : "A!B B!C D!E B~D"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "A"
          , travel    : ATTR_STRONG
          , visit     : ATTR_STRONG | ATTR_WEAK
          , expected  : "ABCD"
        }}
        // neither visit nor travel (stop) on weak edges, down
      , (graph_test[]){{
            constr    : "A!B B!C D!E B~D"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "A"
          , travel    : ATTR_STRONG
          , visit     : ATTR_STRONG
          , expected  : "ABC"
        }}
        // neither visit nor travel (stop) on weak edges, up
      , (graph_test[]){{
            constr    : "A!B B!C D!E B~D"
          , attrs     : GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_PRE
          , from      : "E"
          , travel    : ATTR_STRONG
          , visit     : ATTR_STRONG
          , expected  : "ED"
        }}

        // cycle detection
      , (graph_test[]){{
            constr    : "AB BC CD DE EF FG GA"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "A"
          , cycle_path: "A -> B -> C -> D -> E -> F -> G -> A"
        }}
        // self-referential cycle
      , (graph_test[]){{
            constr    : "AB BA"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "A"
          , cycle_path: "A -> B -> A"
        }}
        // cycle that exceeds the stack size
      , (graph_test[]){{
            constr    : "ab bc cd de ef fg gh hi ij jk kl lm mn no op pq qr rs st tu uv vw wx xy yz zA AB BC CD DE EF FG GH HI IJ JK KL LM MN NO OP PQ QR RS ST TU UV VW WX XY YZ Za"
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
          , from      : "a"
          , cycle     : 1
        }}
      , 0
  }
};
