/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/load.h"
#include "moria.h"
#include "moria/foreach.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "valyria/list.h"

#include "graph.internal.h"
#include "parser.internal.h"
#include "dispatch.h"
#include "linear_search.h"

#include "zcat.h"

#define ATTR_WEAK   0x1
#define ATTR_STRONG 0x2

#define UP 1
#define DOWN 2

typedef struct graph_test {
  XUNITTEST;

  char * graph;         // starting graph
  char * operations;
  char * from;
  int direction;
  char * expected;
} graph_test;

static void graph_unit_setup(xunit_unit * unit)
{
  moria_load();
}

static void graph_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
}

static size_t edge_render(char * dst, size_t sz, moria_edge * const restrict e)
{
  size_t z;
  int x;

  z = 0;
  if(e->attrs & MORIA_EDGE_HYPER)
  {
    for(x = 0; x < e->Blen; x++)
    {
      if(x) {
        z += zcats(dst + z, sizeof(dst) - z, ",");
      }
      z += zcatf(dst + z, sizeof(dst) - z, "%s", e->Blist[x].v->label);
    }
    z += zcats(dst + z, sizeof(dst) - z, ":");
    for(x = 0; x < e->Alen; x++)
    {
      if(x) {
        z += zcats(dst + z, sizeof(dst) - z, ",");
      }
      z += zcatf(dst + z, sizeof(dst) - z, "%s", e->Alist[x].v->label);
    }
  }
  else
  {
    z += zcatf(dst + z, sizeof(dst) - z, "%s:%s", e->B->label, e->A->label);
  }

  return z;
}

static size_t test_up(char * dst, size_t sz, moria_vertex * v)
{
  moria_foreach_state st;
  moria_edge *e;
  size_t z;

  z = 0;
  moria_foreach_edge_up(&st, v, e) {
    if(z) {
      z += zcats(dst + z, sz - z, " ");
    }
    z += edge_render(dst + z, sz - z, e);
  }

  return z;
}

static size_t test_down(char * dst, size_t sz, moria_vertex * v)
{
  moria_foreach_state st;
  moria_edge *e;
  size_t z;

  z = 0;
  moria_foreach_edge_down(&st, v, e) {
    if(z) {
      z += zcats(dst + z, sz - z, " ");
    }
    z += edge_render(dst + z, sz - z, e);
  }

  return z;
}

static void graph_test_entry(xunit_test * _test)
{
  graph_test * test = containerof(_test, graph_test, xu);

  moria_graph g;
  moria_parser * p = 0;
  list * operations = 0;
  moria_vertex * v;
  char record[128];
  size_t z;

  moria_graph_init(&g);
  moria_parser_create(&p, &g, 0, 0, moria_operations_dispatch, 0, 0);
  if(test->graph) {
    moria_parser_graph_parse(p, MMS(test->graph));
  }
  if(test->operations) {
    moria_parser_operations_parse(p, MMS(test->operations));
  }

  // find the starting vertex
  moria_graph_linear_search(&p->vertices, test->from, strlen(test->from), 0, 0, 0, &v);
  assert_notnull(v);

  // act
  if(test->direction == UP)
  {
    z = test_up(record, sizeof(record), v);
  }
  else if(test->direction == DOWN)
  {
    z = test_down(record, sizeof(record), v);
  }

  if(test->expected) {
    record[z] = 0;
    assert_eq_s(test->expected, record);
  } else {
    assert_eq_zu(0, z);
  }

  moria_graph_destroy(&g);
  list_free(operations);
  moria_parser_free(p);
}

static xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_cleanup = graph_unit_cleanup
  , .xu_entry = graph_test_entry
  , .xu_tests = (void*)(graph_test*[]) {
/*
 A
 B
 C
*/
      /* normal edges */
        (graph_test[]){{
            operations : "+A:B +B:C"
          , from       : "C"
          , direction  : UP
        }}
      , (graph_test[]){{
            operations : "+A:B +B:C"
          , from       : "A"
          , direction  : UP
          , expected   : "A:B"
        }}
      , (graph_test[]){{
            operations : "+A:B +B:C"
          , from       : "B"
          , direction  : UP
          , expected   : "B:C"
        }}
      , (graph_test[]){{
            operations : "+A:B +B:C"
          , from       : "A"
          , direction  : DOWN
        }}
      , (graph_test[]){{
            operations : "+A:B +B:C"
          , from       : "C"
          , direction  : DOWN
          , expected   : "B:C"
        }}
      , (graph_test[]){{
            operations : "+A:B +B:C"
          , from       : "B"
          , direction  : DOWN
          , expected   : "A:B"
        }}

/*

A
BC CD
   EF
*/

      /* hyper edges */
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "B"
          , direction  : UP
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "A"
          , direction  : UP
          , expected   : "A:B,C"
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "A"
          , direction  : UP
          , expected   : "A:B,C"
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "E"
          , direction  : UP
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "C"
          , direction  : UP
          , expected   : "C,D:E,F"
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "B"
          , direction  : DOWN
          , expected   : "A:B,C"
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "A"
          , direction  : DOWN
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "F"
          , direction  : DOWN
          , expected   : "C,D:E,F"
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "E"
          , direction  : DOWN
          , expected   : "C,D:E,F"
        }}
      , (graph_test[]){{
            operations : "+A:B,C +C,D:E,F"
          , from       : "D"
          , direction  : DOWN
        }}

/*
    D
 A /
 B
 C

*/

      /* identity edges */
      , (graph_test[]){{
            operations : "+B:0x40000000:A +C:0x40000000:B +B:D"
          , from       : "B"
          , direction  : UP
          , expected   : "B:D B:A"
        }}
      , 0
  }
};
XUNIT_UNIT(xunit);
