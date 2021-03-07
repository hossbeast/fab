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
#include "value/load.h"

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
#include "node_operations.h"
#include "node_operations_test.h"
#include "filesystem.h"

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
  fatal(value_load);
  fatal(logging_finalize);

  fatal(graph_setup);
  fatal(filesystem_setup);
  fatal(fsent_setup);

  finally : coda;
}

static xapi node_operations_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);
  fatal(fsent_cleanup);
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
  graph_parser * parser = 0;
  const char *actual;

  fatal(narrator_growing_create, &N);
  fatal(graph_parser_create, &parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);

  // setup the initial graph
  fatal(graph_parser_operations_parse, parser, MMS(test->graph));

  // perform the operations
  fatal(graph_parser_operations_parse, parser, MMS(test->operations));

  fatal(narrator_xreset, &N->base);
  fatal(graph_say, &N->base);
  actual = N->s;
  assert_eq_s(test->expected, actual);

finally:
  fatal(narrator_growing_free, N);
  fatal(graph_parser_xfree, parser);
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
          graph :     "+A/B:depends:A/C"
        , operations : "=A/B:depends:A/C"
        , expected :  "1-(root)!dir 2-A!dir 3-B!I|file 4-C!file"
                     " 1:fs:2 2:fs:3 2:fs:4"
      }}
    /* single-node, remove fs edge only */
    , (node_operations_test[]) {{
          graph :     "+A/B:depends:A/C"
        , operations : " =A:fs:C"
        , expected :  "1-(root)!dir 2-A!dir 3-B!I|file 4-C!X|file" // fsent still exists but is marked as unlinked
                     " 3:depends:4"
                     " 1:fs:2 2:fs:3 2:fs:4"
      }}
    /* single-node, remove fs, then strong edge */
    , (node_operations_test[]) {{
          graph :     "+A/B:depends:A/C"
        , operations : "=A:fs:C"
                      " =A/B:depends:A/C"
        , expected :  "1-(root)!dir 2-A!dir 3-B!I|file"
                     " 1:fs:2 2:fs:3"
      }}
    /* single-node, remove strong, then fs edge */
    , (node_operations_test[]) {{
          graph :     "+A/B:depends:A/C"
        , operations : "=A/B:depends:A/C"
                       " =A:fs:C"
        , expected :  "1-(root)!dir 2-A!dir 3-B!I|file"
                     " 1:fs:2 2:fs:3"
      }}
    /* tree, remove fs edges bottom-up */
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=B:fs:C"
        , expected :  "1-(root)!dir 2-A!dir 3-B!dir"
                     " 1:fs:2 2:fs:3"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=B:fs:C"
                      " =A:fs:B"
        , expected :  "1-(root)!dir 2-A!dir 1:fs:2"
      }}
    /* tree, remove fs edges top-down */
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=A:fs:B"
        , expected :  "1-(root)!dir 2-A!dir 3-B!X|dir 4-C!file" // marked not-exists but not deleted
                     " 1:fs:2 2:fs:3 3:fs:4"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=A:fs:B"
                      " =B:fs:C"
        , expected :  "1-(root)!dir 2-A!dir 1:fs:2"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C"
        , operations : "=B:fs:C"
                      " =A:fs:B"
        , expected :  "1-(root)!dir 2-A!dir 1:fs:2"
      }}
    , (node_operations_test[]) {{
          graph :      "A/B/C/D/E"
        , operations : "=A:fs:B"
                      " =B:fs:C"
                      " =C:fs:D"
                      " =D:fs:E"
        , expected :  "1-(root)!dir 2-A!dir 1:fs:2"
      }}
    , 0
  }
};
