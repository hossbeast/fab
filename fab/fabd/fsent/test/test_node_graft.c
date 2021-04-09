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

#include "xapi.h"
#include "valyria/load.h"
#include "moria/load.h"

#include "valyria/list.h"
#include "valyria/map.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/parser.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
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

static xapi node_graft_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(filesystem_setup);
  fatal(module_setup);
  fatal(graph_setup);
  fatal(fsent_setup);

  finally : coda;
}

static xapi node_graft_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);
  fatal(module_cleanup);
  fatal(fsent_cleanup);
  fatal(graph_cleanup);

  finally : coda;
}

//
// tests
//

static xapi node_graft_test_entry(xunit_test * _test)
{
  enter;

  node_graft_test * test = (node_graft_test *)_test;
  const char * graph;
  narrator_growing * N = 0;
  graph_parser * parser = 0;
  fsent * node;
  graph_invalidation_context invalidation = { };

  fatal(narrator_growing_create, &N);
  fatal(graph_parser_create, &parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  fatal(graph_invalidation_begin, &invalidation);

  // arrange - setup initial graph
  fatal(graph_parser_operations_parse, parser, MMS(test->operations));

  // act
  fatal(fsent_graft, test->path, &node, &invalidation);
  assert_notnull(node);

  // assert
  fatal(graph_say, &N->base);
  graph = N->s;
  assert_eq_s(test->graph, graph);

finally:
  fatal(narrator_growing_free, N);
  graph_invalidation_end(&invalidation);
  fatal(graph_parser_xfree, parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_graft_test_unit_setup
  , xu_cleanup : node_graft_test_unit_cleanup
  , xu_entry : node_graft_test_entry
  , xu_tests : (node_graft_test*[]) {
      (node_graft_test[]) {{
          operations : ""
        , path : "/a/b/c"
/* graft always creates directories */
        , graph : "1-(root)!dir 2-a!U|dir 3-b!U|dir 4-c!U|dir"
                 " 1:fs:2 2:fs:3 3:fs:4"
      }}
    , 0
  }
};
