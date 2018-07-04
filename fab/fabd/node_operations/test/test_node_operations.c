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

#include "valyria/dictionary.h"
#include "valyria/map.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/edge.h"
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"

#include "node_operations.h"
#include "node.h"

typedef struct node_operations_test
{
  xunit_test;

  char * initial;
  char * sequence;
  char * result;
} node_operations_test;

static xapi node_operations_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  finally : coda;
}

static xapi node_operations_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(valyria_unload);
  fatal(moria_unload);

  finally : coda;
}

static xapi get_node(map * vertices, char * label, node ** n)
{
  enter;

  vertex * v;
  if((v = map_get(vertices, label, 1)) == 0)
  {
    fatal(vertex_createw, &v, g_node_graph, 0, label, 1);
    fatal(map_set, vertices, label, 1, v);
  }

  *n = vertex_value(v);

  finally : coda;
}

//
// tests
//

static xapi node_operations_test_entry(xunit_test * _test)
{
  enter;

  node_operations_test * test = (node_operations_test *)_test;

  map * vertices = 0;
  narrator * N = 0;
  node * A;
  node * B;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &vertices);

  // setup the initial graph
  char * edges = test->initial;
  while(edges && *edges)
  {
    fatal(get_node, vertices, &edges[0], &A);
    fatal(get_node, vertices, &edges[2], &B);

    fatal(graph_connect, g_node_graph, vertex_containerof(A), vertex_containerof(B), NODE_RELATION_FS);

    edges += 3;
    while(*edges == ' ')
      edges++;
  }

  // carry out the test sequence
  char * seq = test->sequence;
  while(*seq)
  {
    int operation = 0;
    if(*seq == '-')
    {
      operation = 1;
      seq++;
    }

    // attach
    if(operation == 0)
    {
      fatal(get_node, vertices, &seq[0], &A);
      fatal(get_node, vertices, &seq[2], &B);
      fatal(node_connect_fs, A, B);
    }

    seq += 3;
    while(*seq == ' ')
      seq++;
  }

  // ordered list of edges
  fatal(graph_say, g_node_graph, 0, N);
  assert_eq_s(test->result ?: "", narrator_growing_buffer(N));

finally:
  fatal(node_cleanup);
  fatal(map_xfree, vertices);
  fatal(narrator_xfree, N);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_operations_test_unit_setup
  , xu_cleanup : node_operations_test_unit_cleanup
  , xu_entry : node_operations_test_entry
  , xu_tests : (xunit_test*[]) {
      (node_operations_test[]) {{        // attach
          initial : "A:B"
        , sequence : "A:C"
        , result : "1-A 2-B 3-C 1:0x1:2 1:0x1:3"
      }}
    , (node_operations_test[]) {{
          initial : "A:B"
        , sequence : "B:C"
        , result : "1-A 2-B 3-C 1:0x1:2 2:0x1:3"
      }}
    , (node_operations_test[]) {{
          initial : "A:B"
        , sequence : "A:C C:D"
        , result : "1-A 2-B 3-C 4-D 1:0x1:2 1:0x1:3 3:0x1:4"
      }}
    , (node_operations_test[]) {{
          initial : "A:B"
        , sequence : "B:C B:D B:E D:F"
        , result : "1-A 2-B 3-C 4-D 5-E 6-F 1:0x1:2 2:0x1:3 2:0x1:4 2:0x1:5 4:0x1:6"
      }}
    , 0
  }
};
