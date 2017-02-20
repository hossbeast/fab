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
#include "valyria/dictionary.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "errtab/MORIA.errtab.h"

#include "macros.h"

#define ATTR_TABLE(x)             \
  ATTR_DEF(WEAK , 0x01  , x)

enum {
#define ATTR_DEF(a, b, x) ATTR_ ## a = UINT32_C(b),
ATTR_TABLE(0)
#undef ATTR_DEF
};

#define ATTR_DEF(a, b, x) (x) == b ? #a :
#define ATTR_STRING(x) ATTR_TABLE(x) "UNKNOWN"

typedef struct graph_test
{
  xunit_test;

  struct edges {
    char *    edges;
    uint32_t  attrs;
  } ** edges;

  uint32_t  skip;
  uint32_t  finish;
  uint32_t  stop;
  uint32_t  attrs;
  char *    from;
  char *    expected;   // expected path
  int       cycle;      // expected only that a cycle exists
  char *    cycle_path; // expected cycle path
} graph_test;

typedef struct item
{
  char label;
  vertex * v;
} item;

static xapi vertex_say(vertex * const restrict v, narrator * const restrict N)
{
  enter;

  sayc(((item*)vertex_value(v))->label);

  finally : coda;
}

static xapi graph_test_entry(graph_test * test)
{
  enter;

  char space[128];
  graph * g = 0;
  dictionary * items = 0;
  narrator * N = 0;

  fatal(xapi_errtab_register, perrtab_MORIA);

  fatal(graph_create, &g, vertex_say);
  fatal(dictionary_create, &items, sizeof(item));
  fatal(narrator_growing_create, &N);

  // setup the test graph
  int x;
  for(x = 0; x < sentinel(test->edges); x++)
  {
    char * edges = test->edges[x]->edges;
    uint32_t attrs = test->edges[x]->attrs;
    while(*edges)
    {
      item * A;
      if((A = dictionary_get(items, MM(edges[0]))) == 0)
      {
        fatal(dictionary_set, items, MM(edges[0]), &A);
        fatal(graph_vertex_create, g, &A->v);
        A->label = edges[0];
        vertex_value_set(A->v, A);
      }

      item * B;
      if((B = dictionary_get(items, MM(edges[1]))) == 0)
      {
        fatal(dictionary_set, items, MM(edges[1]), &B);
        fatal(graph_vertex_create, g, &B->v);
        B->label = edges[1];
        vertex_value_set(B->v, B);
      }

      fatal(graph_relate, g, A->v, B->v, attrs);

      edges += 2;
      while(*edges && *edges == ' ')
        edges++;
    }
  }

  // perform the traversal
  item * root = dictionary_get(items, MMS(test->from));
  xapi exit;
  if((exit = invoke(graph_traverse, g, root->v, (void*)vertex_say, test->skip, test->finish, test->stop, test->attrs, N)))
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
  fatal(dictionary_xfree, items);
  fatal(narrator_xfree, N);
coda;
}

xunit_unit xunit = {
    .xu_entry = graph_test_entry
  , .xu_tests = (xunit_test*[]) {
        // downward, breadth
        (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AC CD DE" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , expected  : "ABCDE"
        }}
        // downward, depth
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AC CD DE" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_DEPTH
          , from      : "A"
          , expected  : "EDCBA"
        }}
        // downward, depth
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AC CD DE" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_DEPTH
          , from      : "C"
          , expected  : "EDC"
        }}
        // upward, breadth, skip weak
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AD DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_BREADTH
          , from      : "C"
          , skip      : ATTR_WEAK
          , expected  : "CBA"
        }}
        // downward, breadth, treat weak normally
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , expected  : "ABCDE"
        }}
        // downward, breadth, skip weak
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , skip      : ATTR_WEAK
          , expected  : "ABCE"
        }}
        // downward, breadth, finish
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , finish    : ATTR_WEAK
          , expected  : "ABCD"
        }}
        // downward, breadth, stop on weak
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , stop      : ATTR_WEAK
          , expected  : "ABC"
        }}
        // cycle detection
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC CD DE EF FG GA" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , cycle_path: "A -> B -> C -> D -> E -> F -> G -> A"
        }}
        // self-referential cycle
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BA" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , cycle_path: "A -> B -> A"
        }}
        // duplicate labels
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AB CD AB AB" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , expected  : "ABCD"
        }}
        // cycle that exceeds the stack size
      , (graph_test[]){{
            edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "ab bc cd de ef fg gh hi ij jk kl lm mn no op pq qr rs st tu uv vw wx xy yz zA AB BC CD DE EF FG GH HI IJ JK KL LM MN NO OP PQ QR RS ST TU UV VW WX XY YZ Za" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "a"
          , cycle     : 1
        }}
      , 0
  }
};
