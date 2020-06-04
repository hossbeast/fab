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
#include "moria/load.h"

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

typedef struct graph_test {
  XUNITTEST;

  char * graph;         // starting graph
  char * operations;

  char *    from_label; // inputs, outer traversal
  uint32_t  attrs;
  uint32_t  edge_travel;
  uint32_t  edge_visit;

  uint32_t  inner_attrs;
  uint32_t  inner_edge_travel;
  uint32_t  inner_edge_visit;

  char ***  expected;  // expected paths
} graph_test;

typedef struct visit
{
  int position;
  int distance;
  char label;
} visit;

static int visitcmp(const void * _A, size_t Asz, const void * _B, size_t Bsz)
{
  const visit * A = _A;
  const visit * B = _B;

  return INTCMP(A->label, B->label);
}

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

struct traverse_context {
  int visit_counter;
  graph * g;
  array * visitations;
  graph_test * test;
};

static xapi inner_vertex_visit(vertex * const restrict v, void * arg, traversal_mode mode, int distance, int * result)
{
  enter;

  struct traverse_context * ctx = arg;
  visit * visitation;

  fatal(array_push, ctx->visitations, &visitation);
  visitation->label = v->label[0];
  visitation->position = ++ctx->visit_counter;
  visitation->distance = distance;

  finally : coda;
}

static xapi outer_vertex_visit(vertex * const restrict v, void * arg, traversal_mode mode, int distance, int * result)
{
  enter;

  struct traverse_context * ctx = arg;
  visit * visitation;

  fatal(array_push, ctx->visitations, &visitation);
  visitation->label = v->label[0];
  visitation->position = ++ctx->visit_counter;
  visitation->distance = distance;

  if(!ctx->test->inner_attrs)
    goto XAPI_FINALLY;

  fatal(graph_traverse_vertices
    , ctx->g
    , v
    , inner_vertex_visit
    , 0
    , (traversal_criteria[]) {{
          edge_travel : ctx->test->inner_edge_travel
        , edge_visit : ctx->test->inner_edge_visit
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , ctx->test->inner_attrs | MORIA_TRAVERSE_DEPTH
    , ctx
  );

  finally : coda;
}

static xapi graph_test_entry(xunit_test * _test)
{
  enter;

  graph_test * test = (typeof(test))_test;

  graph_parser * p = 0;
  graph * g = 0;
  operations_parser * op = 0;
  list * operations = 0;
  array * visitations = 0;
  int x;
  vertex_t * v;

  struct traverse_context ctx;

  fatal(array_createx, &visitations, sizeof(visit), 0, visitcmp, 0, 0, 0, 0);

  // setup the test graph
  uint32_t identity = 0;
  if(test->operations)
    identity = 1;

  fatal(graph_parser_create, &p);
  fatal(graph_parser_graph_create, &g, identity);
  if(test->graph)
    fatal(graph_parser_parse, p, g, MMS(test->graph));

  if(test->operations)
  {
    fatal(operations_parser_operations_create, &operations);
    fatal(operations_parser_create, &op);
    fatal(operations_parser_parse, op, g, MMS(test->operations), operations);
    fatal(operations_perform, g, graph_operations_dispatch, operations);
  }

  vertex_t * from = 0;
  llist_foreach(&g->vertices, v, graph_lln) {
    if(test->from_label && strcmp(v->label, test->from_label) == 0)
      from = v;

    if(from)
      break;
  }

  // perform the traversal
  ctx = (typeof(ctx)) {
      visitations : visitations
    , test : test
    , g : g
  };
  fatal(graph_traverse_vertices
    , g
    , &from->vx
    , outer_vertex_visit
    , 0
    , (traversal_criteria[]) {{
          edge_travel : test->edge_travel
        , edge_visit : test->edge_visit
      }}
    , test->attrs
    , &ctx
  );

  array_sort(visitations, 0);

  if(test->expected)
  {
    char *** expected_sets = test->expected;
    while(*expected_sets)
    {
      char ** expected[2] = { [1] = *expected_sets };
      while(*expected[1])
      {
        // these vertices were visited
        if(!expected[0])
        {
          char * e = *expected[1];
          while(*e)
          {
            visit * visited;
            visit key = { label : *e };
            array_search(visitations, &key, 0, &visited, 0, 0);
            assert_true(visited);

            e++;
          }
        }

        // these vertices were visited after those in the preceeding group
        else
        {
          char * e = *expected[1];
          while(*e)
          {
            visit * evisit;
            visit key = { label : *e };
            array_search(visitations, &key, 0, &evisit, 0, 0);
            assert_true(evisit);

            char * p = *expected[0];
            while(*p)
            {
              visit key = { label : *p };
              visit * pvisit;
              array_search(visitations, &key, 0, &pvisit, 0, 0);

              assert_gt_d(pvisit->position, evisit->position);
              p++;
            }

            e++;
          }
        }

        expected[0] = expected[1];
        expected[1]++;
      }

      expected_sets++;
    }

    // all visitations were in expected
    for(x = 0; x < visitations->size; x++)
    {
      visit * visitation = array_get(visitations, x);

      char *** expected_sets = test->expected;
      while(*expected_sets)
      {
        char ** expected = *expected_sets;
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
        if(*expected)
          break;
        expected_sets++;
      }

      bool observed = *expected_sets;
      xapi_info_pushf("label", "%c", visitation->label);
      assert_true(observed);
      xapi_info_unstage();
    }
  }

finally:
  fatal(graph_xfree, g);
  fatal(list_xfree, operations);
  fatal(array_xfree, visitations);
  fatal(graph_parser_xfree, p);
  fatal(operations_parser_xfree, op);
coda;
}

xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_cleanup = graph_unit_cleanup
  , .xu_entry = graph_test_entry
  , .xu_tests = (void*)(graph_test*[]) {
        // downward, preorder
        (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , expected   : (char**[]) {
                (char *[]) { "ABCDE", 0 }
              , 0
            }
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
          , expected   : (char **[]) {
                  (char*[]) { "B", "C", "D", "E", 0 }
                , 0
              }
        }}

        // downward, preorder, hyper-edges
      , (graph_test[]){{
            operations : "+A:B +B:C,D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , expected   : (char**[]) {
                (char *[]) { "A", "B", "CD", 0 }
              , (char *[]) { "D", "E", 0 }
              , 0
            }
        }}

        // downward, postorder
      , (graph_test[]){{
/*
  A
  B
  C
  D
  E
 */
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from_label : "A"
          , expected   : (char**[]) {
                  (char *[]) { "E", "D", "C", "B", "A", 0 }
                , 0
              }
        }}

        // downward, postorder, hyper-edges
      , (graph_test[]){{
/*
    A
    B
   C D
     E
*/
            operations : "+A:B +B:C,D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from_label : "A"
          , expected   : (char**[]) {
                (char *[]) { "C", "B", "A", 0 }
              , (char *[]) { "E", "D", "B", "A", 0 }
              , 0
            }
        }}

        // downward, postorder, hyper-edges, zero-to-many
      , (graph_test[]){{
/*
    A
    B
   C D
     E
*/
            operations : "+A:B +B:C +B:D +D:E +_:E,F"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from_label : "A"
          , expected   : (char**[]) {
                (char *[]) { "C", "B", "A", 0 }
              , (char *[]) { "E", "D", "B", "A", 0 }
              , 0
            }
        }}

        // upward, postorder, hyper-edges, zero-to-many
      , (graph_test[]){{
/*
    A
    B
   C D
     EF
*/
            operations : "+A:B +B:C +B:D +D:E +_:E,F"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from_label : "E"
          , expected   : (char**[]) {
                (char *[]) { "E", "D", "B", "A", 0 }
              , 0
            }
        }}


        // downward, postorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from_label : "C"
          , expected   : (char**[]) {
                (char*[]) { "EDC", 0 }
              , 0
            }
        }}

        // neither visit nor travel (stop) on weak edges, down
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 1:0x1:2 3:0x1:4"          // strong
                        " 1:0x2:3"                          // weak
          , attrs     : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label: "A"
          , edge_travel    : 0x1 // ATTR_STRONG
          , edge_visit     : 0x1 // ATTR_STRONG
          , expected  : (char**[]) {
                (char *[]) { "A", "BC", 0 }
              , 0
            }
        }}
        // neither visit nor travel (stop) on weak edges, up
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 1:0x1:2 3:0x1:4"          // strong
                        " 1:0x2:3"                          // weak
          , attrs     : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from_label: "E"
          , edge_travel    : 0x1 // ATTR_STRONG
          , edge_visit     : 0x1 // ATTR_STRONG
          , expected  : (char**[]) {
                (char *[]) { "E", "D", 0 }
              , 0
            }
        }}

        /* multi-hop cycle */
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E +E:F +F:G +G:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , expected  : (char**[]) {
                (char *[]) { "A", "B", "C", "D", "E", "F", "G", 0 }
              , 0
            }
        }}
        // single-hop cycle
      , (graph_test[]){{
            operations : "+A:B +B:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , expected  : (char**[]) {
                (char *[]) { "A", "B", 0 }
              , 0
            }
        }}
        // cycle, hyper-edges
      , (graph_test[]){{
/*
      /-> C -> E -> G
A -> B
      \-> D -> F ==>> B
*/
            operations : "+A:B +B:C,D +C:E +E:G +D:F +F:B"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , expected  : (char**[]) {
                (char *[]) { "A", "B", "C", "E", "G", "D", "F", 0 }
              , 0
            }
        }}

/*
A -> B -> C -> D -> E
*/

        /* nested traversals */
      , (graph_test[]){{
            operations : "+A:B +B:C"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label : "A"
          , inner_attrs : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
        }}

/*

     0-A
   /     \
  1-B -> 3-D
  2-C    4-E

*/

        /* exhaustive traversal */
      , (graph_test[]){{
            graph     : " 0-A 1-B 2-C 3-D 4-E"
                        " 0:0x1:1 0:0x1:3 1:0x1:2 1:0x1:3 3:0x1:4"
          , attrs     : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from_label   : "A"
          , edge_travel  : 0x1
          , edge_visit   : 0x1
          , expected  : (char**[]) {
                (char *[]) { "A", "B", "C", 0 }
              , (char *[]) { "A", "D", "E", 0 }
              , 0
            }
        }}

      , 0
    }
};
