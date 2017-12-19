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
#include "ff_node.h"
#include "ff_node_pattern.h"
#include "ff_parser.h"
#include "rule.internal.h"
#include "node.h"
#include "node_operations.h"
#include "artifact.h"
#include "ff_node_require.h"
#include "path.h"
#include "filesystem.internal.h"
#include "module.h"

static map * attrs_definitions;

typedef struct node_graft_test
{
  xunit_test;

  char * operations;          // create the starting graph
  char * path;             // context node for applying rules

  char * graph;
} node_graft_test;

static xapi node_graft_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(filesystem_setup);
  fatal(module_setup);

  // attrs name map for graph_say
  fatal(map_create, &attrs_definitions);
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_FS }, sizeof(uint32_t), "FS");
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_STRONG }, sizeof(uint32_t), "ST");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_FILE }, sizeof(uint32_t), "FILE");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_DIR}, sizeof(uint32_t), "DIR");

  finally : coda;
}

static xapi node_graft_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(map_xfree, attrs_definitions);

  finally : coda;
}

//
// tests
//

static xapi node_graft_test_entry(xunit_test * _test)
{
  enter;

  node_graft_test * test = (node_graft_test *)_test;

  narrator * N = 0;
  list * operations = 0;

  fatal(node_setup);
  fatal(node_root_init);
  fatal(narrator_growing_create, &N);

  // setup initial graph
  fatal(operations_parser_parse, 0, MMS(test->operations), &operations);
  fatal(operations_perform, g_node_graph, node_operations_dispatch, operations);

  node * node;
  fatal(node_graft, test->path, &node);

  assert_notnull(node);

  fatal(graph_say, g_node_graph, attrs_definitions, N);
  const char * graph = narrator_growing_buffer(N);
  assert_eq_s(test->graph, graph);

finally:
  fatal(node_cleanup);
  fatal(narrator_xfree, N);
  fatal(list_xfree, operations);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_graft_test_unit_setup
  , xu_cleanup : node_graft_test_unit_cleanup
  , xu_entry : node_graft_test_entry
  , xu_tests : (xunit_test*[]) {
      (node_graft_test[]) {{
          operations : ""
        , path : "/foo/bar/baz"
        , graph : "1-!DIR 2-bar!DIR 3-baz!DIR 4-foo!DIR"
                 " 1:FS:4 2:FS:3 4:FS:2"
      }}
    , 0
  }
};
