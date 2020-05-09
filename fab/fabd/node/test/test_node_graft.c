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

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.internal.h"
#include "node.h"
#include "node_operations.h"
#include "path.h"
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
  fatal(node_setup);

  finally : coda;
}

static xapi node_graft_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);
  fatal(module_cleanup);
  fatal(node_cleanup);
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
  narrator * N = 0;
  list * operations = 0;
  operations_parser * parser = 0;
  node * node;
  graph_invalidation_context invalidation;

  fatal(narrator_growing_create, &N);
  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_create, &parser);
  fatal(graph_invalidation_begin, &invalidation);

  // setup initial graph
  fatal(operations_parser_parse, parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  fatal(node_graft, test->path, &node, &invalidation);

  assert_notnull(node);

  fatal(graph_say, g_graph, N);
  graph = narrator_growing_buffer(N);
  assert_eq_s(test->graph, graph);

finally:
  fatal(narrator_xfree, N);
  graph_invalidation_end(&invalidation);
  fatal(list_xfree, operations);
  fatal(operations_parser_xfree, parser);
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
        , graph : "1-(root)!dir 2-a!dir 3-b!dir 4-c!dir"
                 " 1:fs:2 2:fs:3 3:fs:4"
      }}
    , 0
  }
};
