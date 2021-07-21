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

#include "moria/load.h"
#include "moria.h"
#include "moria/traverse.h"
#include "moria/edge.h"
#include "xlinux/xstdlib.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "valyria/multimap.h"
#include "valyria/array.h"
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "graph.internal.h"
#include "edge.internal.h"
#include "vertex.h"
#include "operations.h"
#include "parser.internal.h"

#include "macros.h"
#include "zbuffer.h"

#define ATTR_WEAK   0x1
#define ATTR_STRONG 0x2

typedef struct graph_test {
  XUNITTEST;

  char * graph;         // starting graph
  char * operations;

  char *    from;
  uint32_t  attrs;
  uint32_t  travel;
  uint32_t  visit;

  char ***   expected;  // expected paths
} graph_test;

typedef struct visit
{
  int position;
  char label[32];
} visit;

static int visitcmp(const void * _A, size_t Asz, const void * _B, size_t Bsz)
{
  const visit * A = _A;
  const visit * B = _B;

  return strcmp(A->label, B->label);
}

static int visit_counter;
static void edge_visit(moria_edge * const restrict e, void * arg, moria_traversal_mode mode, int distance, int * result)
{
  array * visitations = arg;
  visit * visitation;
  size_t z;
  int x;

  array_push(visitations, &visitation);
  visitation->position = ++visit_counter;

  if(e->attrs & MORIA_EDGE_HYPER)
  {
    z = 0;
    for(x = 0; x < e->Alen; x++)
    {
      if(x)
        z += znloads(visitation->label + z, sizeof(visitation->label) - z, ",");
      z += znloadf(visitation->label + z, sizeof(visitation->label) - z, "%s", e->Alist[x].v->label);
    }

    z += znloads(visitation->label + z, sizeof(visitation->label) - z, ":");

    for(x = 0; x < e->Blen; x++)
    {
      if(x)
        z += znloads(visitation->label + z, sizeof(visitation->label) - z, ",");
      z += znloadf(visitation->label + z, sizeof(visitation->label) - z, "%s", e->Blist[x].v->label);
    }
  }
  else
  {
    snprintf(visitation->label, sizeof(visitation->label), "%s:%s", e->A->label, e->B->label);
  }
}

static void graph_unit_setup(xunit_unit * unit)
{
  moria_load();
}

static void graph_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
}

static void graph_test_entry(xunit_test * _test)
{
  graph_test * test = containerof(_test, graph_test, xu);

  moria_graph g;
  graph_parser * p = 0;
  list * operations = 0;
  array * visitations = 0;
  const char ** keys = 0;
  int x;
  moria_vertex * v;
  const char * spec;

  array_create(&visitations, sizeof(visit));

  moria_graph_init(&g);
  graph_parser_create(&p, &g, 0, graph_operations_dispatch, 0, 0);
  if(test->graph) {
    graph_parser_parse(p, MMS(test->graph));
  }

  if(test->operations) {
    graph_parser_operations_parse(p, MMS(test->operations));
  }

  // find the starting edge
  moria_vertex * fromA[4];
  size_t fromA_len = 0;
  moria_vertex * fromB[4];
  size_t fromB_len = 0;
  const char * froms = test->from;
  bool colon = false;
  while(*froms)
  {
    if(*froms == ':')
    {
      colon = true;
    }
    else
    {
      llist_foreach(&p->vertices, v, owner) {
        if(v->label[0] == *froms)
        {
          if(!colon)
          {
            fromA[fromA_len++] = v;
          }
          else
          {
            fromB[fromB_len++] = v;
          }
          break;
        }
      }
    }
    froms++;
  }

  moria_edge * from = moria_edge_of(fromA, fromA_len, fromB, fromB_len);
  assert_notnull(from);

  // perform the traversal
  moria_traverse_edges(
      &g
    , from
    , edge_visit
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : test->travel
        , edge_visit : test->visit
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , test->attrs
    , visitations
  );

  array_sort(visitations, visitcmp);

  char *** expected_sets = test->expected;
  while(*expected_sets)
  {
    char ** expected[2] = { [1] = *expected_sets };
    while(*expected[1])
    {
      // these edges were visited
      if(!expected[0])
      {
        char * p = *expected[1];
        while(*p)
        {
          spec = p;
          while(*p && *p != ' ')
            p++;

          visit * visited;
          visit key;
          snprintf(key.label, sizeof(key.label), "%.*s", (int)(p - spec), spec);
          array_search(visitations, &key, visitcmp, &visited, 0, 0);

          xunit_info_pushf("edge", "%.*s", (int)(p - spec), spec);
          assert_true(visited);
          xunit_info_unstage();

          if(*p == ' ')
            p++;
        }
      }

      // these edges were visited after those in the preceeding group
      else
      {
        char * e = *expected[1];
        while(*e)
        {
          spec = e;
          while(*e && *e != ' ')
            e++;

          visit * evisit;
          visit key;
          snprintf(key.label, sizeof(key.label), "%.*s", (int)(e - spec), spec);
          array_search(visitations, &key, visitcmp, &evisit, 0, 0);
          assert_true(evisit);

          char * p = *expected[0];
          while(*p)
          {
            spec = p;
            while(*p && *p != ' ')
              p++;

            visit * pvisit;
            visit key;
            snprintf(key.label, sizeof(key.label), "%.*s", (int)(p - spec), spec);
            array_search(visitations, &key, visitcmp, &pvisit, 0, 0);

            assert_gt_d(pvisit->position, evisit->position);
            if(*p == ' ')
              p++;
          }

          if(*e == ' ')
            e++;
        }
      }

      expected[0] = expected[1];
      expected[1]++;
    }
    expected_sets++;
  }

  // all observed visitations were expected
  for(x = 0; x < visitations->size; x++)
  {
    visit * visitation = array_get(visitations, x);

    char *** expected_sets = test->expected;
    while(*expected_sets)
    {
      char ** expected = *expected_sets;
      while(*expected)
      {
        const char * label = *expected;
        while(*label)
        {
          spec = label;
          while(*spec && *spec != ' ')
            spec ++;

          if(memncmp(MMS(visitation->label), label, (int)(spec - label)) == 0)
            break;

          label = spec;
          if(*label == ' ')
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
    xunit_info_pushf("label", "%s", visitation->label);
    assert_true(observed);
    xunit_info_unstage();
  }

  moria_graph_destroy(&g);
  list_xfree(operations);
  array_xfree(visitations);
  wfree(keys);
  graph_parser_xfree(p);
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
          , from       : "B:C"
          , expected   : (char**[]) {
                (char*[]) { "B:C C:D D:E", 0 }
              , 0
            }
        }}
        // downward, preorder, hyperedges
      , (graph_test[]){{
/*
  A
 B C
   D
   E
*/
            operations : "+A:B,C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B,C"
          , expected   : (char**[]) {
                (char*[]) { "A:B,C C:D D:E", 0 }
              , 0
            }
        }}
        // downward, preorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "B:C"
          , expected   : (char**[]) {
                (char*[]) { "B:C C:D D:E", 0 }
              , 0
            }
        }}
        // downward, postorder
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from       : "A:B"
          , expected   : (char**[]) {
                (char*[]) { "B:C C:D D:E", "A:B", 0 }
              , 0
            }
        }}
        /* downward, postorder, hyper-edges */
      , (graph_test[]){{
/*
 A
 B
C D
  E
*/
            operations : "+A:B +B:C,D +D:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from       : "A:B"
          , expected   : (char**[]) {
                (char*[]) { "B:C,D D:E", "A:B", 0 }
              , 0
            }
        }}

        // downward, postorder, hyper-edges
      , (graph_test[]){{
            operations : "+A:B +B:C,D +C:E"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
          , from       : "A:B"
          , expected   : (char**[]) {
                (char*[]) { "C:E", "B:C,D A:B", 0 }
              , 0
            }
        }}

        // upward, postorder, from another node
      , (graph_test[]){{
            operations : "+A:B +B:C +C:D +D:E"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST
          , from       : "C:D"
          , expected   : (char**[]) {
               (char*[]) { "A:B B:C", "C:D", 0 }
              , 0
            }
        }}
        // upward, postorder, hyper-edges
      , (graph_test[]){{
/*
 A
B C
 D E
   F
*/
            operations : "+A:B,C +C:D,E +E:F"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST
          , from       : "E:F"
          , expected   : (char**[]) {
               (char*[]) { "A:B,C", "C:D,E", "E:F", 0 }
              , 0
            }
        }}

        // neither visit nor travel (stop) on weak edges, down
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +D:2:E +B:1:D"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , travel     : ATTR_STRONG
          , visit      : ATTR_STRONG
          , expected   : (char**[]) {
                (char*[]) { "A:B B:C", 0 }
              , 0
            }
        }}
        // neither visit nor travel (stop) on weak edges, up
      , (graph_test[]){{
            operations : "+A:2:B +B:2:C +D:2:E +B:1:D"
          , attrs      : MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
          , from       : "D:E"
          , travel     : ATTR_STRONG
          , visit      : ATTR_STRONG
          , expected   : (char**[]) {
                (char*[]) { "D:E", 0 }
              , 0
            }
        }}

        // multi-hop cycle
      , (graph_test[]){{
/*
A -> B -> C -> D -> E -> F -> G ==>> A
*/
            operations : "+A:B +B:C +C:D +D:E +E:F +F:G +G:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , expected   : (char**[]) {
              (char*[]) { "A:B", "B:C", "C:D", "D:E", "E:F", "F:G", "G:A", 0 }
            , 0
          }
        }}
        // single-hop cycle
      , (graph_test[]){{
            operations : "+A:B +B:A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , expected   : (char**[]) {
              (char*[]) { "A:B", "B:A", 0 }
            , 0
          }
        }}
        // cycle, hyper-edges
      , (graph_test[]){{
/*
 A
 B
C D
 E A

 */
            operations : "+A:B +B:C,D +D:E,A"
          , attrs      : MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
          , from       : "A:B"
          , expected   : (char**[]) {
              (char*[]) { "A:B", "B:C,D", "D:A,E", 0 }
            , 0
          }
        }}

      , 0
  }
};
