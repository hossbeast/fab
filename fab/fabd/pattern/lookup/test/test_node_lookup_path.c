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
#include "yyutil/parser.h"

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
#include "node_operations_test.h"
#include "lookup.internal.h"

typedef struct node_lookup_test
{
  XUNITTEST;

  char * operations;       // create the starting graph
  char * path;             // lookup path

  char * abspath;
} node_lookup_test;

static xapi node_lookup_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(graph_setup);
  fatal(node_setup);
  fatal(filesystem_setup);

  finally : coda;
}

static xapi node_lookup_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);
  fatal(node_cleanup);
  fatal(graph_cleanup);

  finally : coda;
}

//
// tests
//

static xapi node_lookup_test_entry(xunit_test * _test)
{
  enter;

  node_lookup_test * test = containerof(_test, node_lookup_test, xu);
  char abspath[128];

  list * operations = 0;
  operations_parser * parser = 0;
  node * node = 0;

  // setup initial graph
  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_create, &parser);
  fatal(operations_parser_parse, parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  // act
  fatal(pattern_lookup_fragment, MMS(test->path), 0, 0, 0, 0, 0, 0, &node);

  // assert
  assert_notnull(node);
  node_absolute_path_znload(abspath, sizeof(abspath), node);
  assert_eq_s(test->abspath, abspath);

finally:
  fatal(list_xfree, operations);
  fatal(operations_parser_xfree, parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_lookup_test_unit_setup
  , xu_cleanup : node_lookup_test_unit_cleanup
  , xu_entry : node_lookup_test_entry
  , xu_tests : (node_lookup_test*[]) {
      (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "D"
        , abspath : "/A/B/C/D"
      }}
    , (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "C/D"
        , abspath : "/A/B/C/D"
      }}
    , (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "B/C/D"
        , abspath : "/A/B/C/D"
      }}
    , (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "A/B/C/D"
        , abspath : "/A/B/C/D"
      }}

    , (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "A"
        , abspath : "/A"
      }}
    , (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "B"
        , abspath : "/A/B"
      }}
    , (node_lookup_test[]) {{
          operations : "A/B/C/D"
        , path : "A/B"
        , abspath : "/A/B"
      }}


    , (node_lookup_test[]) {{
          operations : "A/B/C/D A/X"
        , path : "A/B/C"
        , abspath : "/A/B/C"
      }}
    , 0
  }
};
