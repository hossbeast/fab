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
#include "node_operations_test.h"
#include "filesystem.h"
#include "path.h"

typedef struct node_operations_test {
  XUNITTEST;

  /* starting graph */
  char * graph;

  /* operations to perform */
  char * operations;

  /* expected graph */
  char * expected;
} node_operations_test;

static xapi node_operations_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(graph_setup);
  fatal(filesystem_setup);
  fatal(node_setup_minimal);

  finally : coda;
}

static xapi node_operations_test_unit_cleanup(xunit_unit * unit)
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

static xapi node_operations_test_entry(xunit_test * _test)
{
  enter;

  node_operations_test * test = (node_operations_test *)_test;

  narrator_growing * N = 0;
  list * operations = 0;
  operations_parser * parser = 0;
  const char *actual;

  fatal(narrator_growing_create, &N);
  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_create, &parser);

  // setup the initial graph
  fatal(operations_parser_parse, parser, g_graph, MMS(test->graph), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  // perform the operations
  fatal(operations_parser_parse, parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  fatal(narrator_xreset, &N->base);
  fatal(graph_say, g_graph, &N->base);
  actual = N->s;
  assert_eq_s(test->expected, actual);

finally:
  fatal(narrator_growing_free, N);
  fatal(list_xfree, operations);
  fatal(operations_parser_xfree, parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_operations_test_unit_setup
  , xu_cleanup : node_operations_test_unit_cleanup
  , xu_entry : node_operations_test_entry
  , xu_tests : (node_operations_test*[]) {
    /* single-node, remove strong edge only */
      (node_operations_test[]) {{
          graph :     "+A/B:strong:A/C"
        , operations : "=A/B:strong:A/C"
        , expected :  "1-A!dir 2-B!I|file 3-C!file"
                     " 1:fs:2 1:fs:3"
      }}
    /* single-node, remove fs edge only */
    , (node_operations_test[]) {{
          graph :     "+A/B:strong:A/C"
        , operations : " =A:fs:C"
        , expected :  "1-A!dir 2-B!I|file 3-C!X|file" // marked as unlinked, not deleted
                     " 2:strong:3"
                     " 1:fs:2 1:fs:3"
      }}
    /* single-node, remove fs, then strong edge */
    , (node_operations_test[]) {{
          graph :     "+A/B:strong:A/C"
        , operations : "=A:fs:C"
                      " =A/B:strong:A/C"
        , expected :  "1-A!dir 2-B!I|file"
                     " 1:fs:2"
      }}
    /* single-node, remove strong, then fs edge */
    , (node_operations_test[]) {{
          graph :     "+A/B:strong:A/C"
        , operations : "=A/B:strong:A/C"
                       " =A:fs:C"
        , expected :  "1-A!dir 2-B!I|file"
                     " 1:fs:2"
      }}
    /* tree, remove fs edges bottom-up */
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=B:fs:C"
        , expected :  "1-A!dir 2-B!dir"
                     " 1:fs:2"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=B:fs:C"
                      " =A:fs:B"
        , expected :  "1-A!dir"
      }}
    /* tree, remove fs edges top-down */
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=A:fs:B"
        , expected :  "1-A!dir 2-B!X|dir 3-C!file" // marked not-exists only
                     " 1:fs:2 2:fs:3"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=A:fs:B"
                      " =B:fs:C"
        , expected :  "1-A!dir"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C/D/E"
        , operations : "=A:fs:B"
                      " =B:fs:C"
                      " =C:fs:D"
                      " =D:fs:E"
        , expected :  "1-A!dir"
      }}
    , 0
  }
};
