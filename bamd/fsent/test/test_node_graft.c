/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/load.h"

#include "valyria/list.h"
#include "moria/graph.h"
#include "moria/parser.h"
#include "bacon/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "rule.h"
#include "fsent.h"
#include "filesystem.internal.h"
#include "module.h"
#include "node_operations_test.h"

typedef struct node_graft_test {
  XUNITTEST;

  char * operations;    // create the starting graph
  char * path;          // context node for applying rules

  char * graph;         // resultant graph
} node_graft_test;

static void node_graft_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();

  filesystem_setup();
  module_setup();
  graph_setup();
  fsent_setup();
}

static void node_graft_test_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
  bacon_unload();

  filesystem_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

//
// tests
//

static void node_graft_test_entry(xunit_test * _test)
{
  node_graft_test * test = (node_graft_test *)_test;
  const char * graph;
  narrator_growing * N = 0;
  moria_parser * parser = 0;
  fsent * node;
  graph_invalidation_context invalidation = { };

  narrator_growing_create(&N);
  moria_parser_create(&parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  graph_invalidation_begin(&invalidation);

  // arrange - setup initial graph
  moria_parser_operations_parse(parser, MMS(test->operations));

  // act
  fsent_graft(test->path, &node, &invalidation);
  assert_notnull(node);

  // assert
  graph_say(&N->base);
  graph = N->s;
  assert_eq_s(test->graph, graph);

  narrator_growing_free(N);
  graph_invalidation_end(&invalidation);
  moria_parser_free(parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : node_graft_test_unit_setup
  , xu_cleanup : node_graft_test_unit_cleanup
  , xu_entry : node_graft_test_entry
  , xu_tests : (node_graft_test*[]) {
      (node_graft_test[]) {{
          operations : ""
        , path : "/a/b/c"
/* graft always creates protected directories */
        , graph : "1-(root)!dir|protect 2-a!dir|protect 3-b!dir|protect 4-c!dir|protect"
                 " 2:fs:1 3:fs:2 4:fs:3"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
