/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/map.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "moria/load.h"

#include "moria.h"
#include "graph.internal.h"
#include "parser.internal.h"
#include "vertex.h"
#include "lookup.h"
#include "connect.h"
#include "dispatch.h"

typedef struct graph_lookup_test
{
  XUNITTEST;

  // setup the graph
  char * operations;

  // labels to lookup
  char ** labels;

  // expected results
  char * expected_label;
  uint32_t expected_attrs;
} graph_lookup_test;

static void graph_unit_setup(xunit_unit * unit)
{
  moria_load();
}

static void graph_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
}

static void *index_objs[1024];
static uint32_t index_objs_len;

static void graph_index_lookup(moria_graph * restrict g, moria_vertex * restrict index, moria_vertex * restrict v)
{
  moria_vertex *base;
  moria_edge *e;
  int __attribute__((unused)) r;
  moria_connect_context ctx;

  if(!(base = moria_vertex_downw(index, v->label, v->label_len))) {
    xmalloc(&base, sizeof(*base));
    index_objs[index_objs_len++] = base;
    moria_vertex_init(base, g, 0);
    base->label = v->label;
    base->label_len = v->label_len;
    r = moria_preconnect(&ctx, g, base, index, MORIA_EDGE_IDENTITY, 0);
    RUNTIME_ASSERT(r == MORIA_NOEDGE);
    xmalloc(&e, sizeof(*e));
    index_objs[index_objs_len++] = e;
    moria_connect(&ctx, g, e, base, index, MORIA_EDGE_IDENTITY);
  }

  r = moria_preconnect(&ctx, g, v, base, 0, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  xmalloc(&e, sizeof(*e));
  index_objs[index_objs_len++] = e;
  moria_connect(&ctx, g, e, v, base, 0);
}

static void graph_test_entry(xunit_test * _test)
{
  moria_graph g;
  moria_vertex *index;
  moria_parser * p = 0;
  moria_vertex *v;
  const moria_vertex * actual[2];
  int r;

  graph_lookup_test * test = (void*)_test;
  moria_graph_init(&g);

  // arrange
  moria_parser_create(&p, &g, 0, 0, moria_operations_dispatch, 0, 0);
  if(test->operations) {
    moria_parser_operations_parse(p, MMS(test->operations));
  }

  /* setup the index */
  xmalloc(&index, sizeof(*index));
  index_objs[index_objs_len++] = index;
  moria_vertex_init(index, &g, 0);

  llist_foreach(&p->vertices, v, owner) {
    graph_index_lookup(&g, index, v);
  }

  // act
  moria_graph_lookup(
      &g
    , index
    , moria_graph_lookup_sentinel
    , 0
    , (moria_graph_lookup_sentinel_context[]) {{ labels : test->labels }}
    , actual
    , &r
  );

  if(test->expected_label || test->expected_attrs)
  {
    assert_eq_d(1, r);
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

  moria_graph_destroy(&g);
  moria_parser_free(p);

  int x;
  for(x = 0; x < index_objs_len; x++) {
    free(index_objs[x]);
  }
}

static xunit_unit xunit = {
    .xu_setup = graph_unit_setup
  , .xu_cleanup = graph_unit_cleanup
  , .xu_entry = graph_test_entry
  , .xu_tests = (void*)(graph_lookup_test*[]) {
      // graph lookup on the identity subgraph
        (graph_lookup_test[]){{
            operations     : " +x:0x40000000:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "x", 0 }
          , expected_label : "x"
        }}
      , (graph_lookup_test[]){{
            operations     : " +x:0x40000000:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "y", 0 }
          , expected_label : "y"
        }}
      , (graph_lookup_test[]){{
            operations     : " +y:0x40000000:x" // aka x/y
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "y", "x", 0 }
          , expected_label : "y"
        }}
      , (graph_lookup_test[]){{
            operations     : " +x:0x40000000:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", 0 }
          , expected_label : "B"
        }}
      , (graph_lookup_test[]){{
            operations     : " +x:0x40000000:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", "y", 0 }
          , expected_label : "B"
        }}
      , (graph_lookup_test[]){{
            operations     : " +y:0x40000000:x"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", "y", "x", 0 }
          , expected_label : "B"
        }}
      , 0
  }
};
XUNIT_UNIT(xunit);
