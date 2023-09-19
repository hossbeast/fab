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


#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/map.h"
#include "valyria/multimap.h"
#include "valyria/list.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "moria/load.h"

#include "moria.h"
#include "graph.internal.h"
#include "operations.h"
#include "parser.internal.h"
#include "vertex.h"

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

static void graph_hashtable_index(hashtable * restrict mm, moria_vertex * restrict v)
{
  moria_vertex_entry *entry, key;

  key.label = v->label;
  key.label_len = v->label_len;
  entry = hashtable_search(mm, &key, sizeof(key), moria_vertex_entry_hash, moria_vertex_entry_key_cmp);

  if(!entry)
  {
    /* entry will use this nodes label */
    rbtree_init(&key.rbt);
    hashtable_put(mm, &key);
    entry = hashtable_get(mm, &key);
  }
  rbtree_put(&entry->rbt, v, rbn_lookup, (void*)ptrcmp);
}

static void graph_test_entry(xunit_test * _test)
{
  moria_graph g;
  hashtable *mm = 0;
  graph_parser * p = 0;
  moria_vertex *v;
  moria_vertex * actual[2];
  int r;

  graph_lookup_test * test = (void*)_test;
  moria_graph_init(&g);

  // arrange
  hashtable_createx(
      &mm
    , sizeof(moria_vertex_entry)
    , 0
    , moria_vertex_entry_hash
    , moria_vertex_entry_cmp
    , 0
  );
  graph_parser_create(&p, &g, 0, graph_operations_dispatch, 0, 0);
  if(test->operations) {
    graph_parser_operations_parse(p, MMS(test->operations));
  }

  llist_foreach(&p->vertices, v, owner) {
    graph_hashtable_index(mm, v);
  }

  // act
  moria_graph_lookup(
      &g
    , mm
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
  graph_parser_xfree(p);
  hashtable_xfree(mm);
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
            operations     : " +x:0x40000000:y" // aka x/y
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
            operations     : " +x:0x40000000:y"
                             " +y/A:y/B +y/B:y/C +y/C:y/D +y/C:y/E"
          , labels         : (char*[]) { "B", "y", "x", 0 }
          , expected_label : "B"
        }}
      , 0
  }
};
XUNIT_UNIT(xunit);
