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
#include "valyria/list.h"
#include "valyria/map.h"
#include "moria/edge.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "ff_node.h"
#include "ff_parser.h"
#include "rule.internal.h"
#include "node.h"
#include "path.h"
#include "artifact.h"

typedef struct rules_test
{
  xunit_test;

  char * nodes;
  char * base;
  char * artifact;
  char * artifact_variant;
  char ** rules;
  char * result_graph;
} rules_test;

static xapi rules_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_finalize);

  finally : coda;
}

static xapi get_node(map * node_map, char * label, size_t len, node ** n)
{
  enter;

  if((*n = map_get(node_map, label, len)) == 0)
  {
    vertex * v;
    fatal(vertex_createw, &v, g_node_graph, 0, label, len);
    *n = vertex_value(v);
    (*n)->fstype = NODE_FSTYPE_FILE;
    fatal(path_createw, &(*n)->name, label, len);
    fatal(map_set, node_map, label, len, *n);
  }

  finally : coda;
}

//
// tests
//

static xapi rules_test_entry(xunit_test * _test)
{
  enter;

  rules_test * test = (rules_test *)_test;

  map * node_map = 0;
  narrator * N = 0;
  ff_parser * parser = 0;
  ff_node * ffn = 0;
  node * artifact_node;
  node * base;
  list * rules_list = 0;
  node * A = 0;
  node * B;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &node_map);

  // setup initial nodes
  char * nodes = test->nodes;
  while(*nodes)
  {
    while(*nodes && *nodes == ' ')
      nodes++;

    while(*nodes && *nodes != ' ')
    {
      char * label = nodes;
      while(*nodes && *nodes != ' ' && *nodes != '/')
        nodes++;

      fatal(get_node, node_map, label, nodes - label, &B);

      if(*nodes == '/')
        nodes++;

      if(A)
      {
        A->fstype = NODE_FSTYPE_DIR;
        fatal(graph_connect, g_node_graph, vertex_containerof(A), vertex_containerof(B), NODE_RELATION_FS);
      }
      
      A = B;
    }

    A = 0;
  }

  // parse the rules
  fatal(list_createx, &rules_list, ffn_free, 0, 0);
  char ** rules = test->rules;
  while(*rules)
  {
    fatal(ff_parser_parse, &parser, MMS(*rules), 0, &ffn);
    fatal(list_push, rules_list, ffn);
    ffn = 0;

    rules++;
  }

  // apply rules, starting with the artifact
  base = map_get(node_map, MMS(test->base));
  artifact_node = map_get(node_map, MMS(test->artifact));

  artifact af = {
      node : artifact_node
    , variant : test->artifact_variant
    , variant_len : test->artifact_variant ? strlen(test->artifact_variant) : 0
  };

  int traversal_id = graph_traversal_begin(g_node_graph);
  fatal(rules_apply, rules_list, base, &af, traversal_id);

  // ordered list of edges
  fatal(graph_say, g_node_graph, 0, N);
  const char * graph = narrator_growing_buffer(N);
  assert_eq_s(test->result_graph, graph);

finally:
  fatal(node_cleanup);
  fatal(map_xfree, node_map);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  ffn_free(ffn);
  fatal(list_xfree, rules_list);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : rules_test_unit_setup
  , xu_entry : rules_test_entry
  , xu_tests : (xunit_test*[]) {
      (rules_test[]) {{
          nodes : "A/B A/C.c"
        , base : "A"
        , artifact : "B"
        , rules : (char*[]) {
              "rule B : C.o"
            , "rule C.o : C.c"
            , 0
          }
        , result_graph : "1-A 2-B 3-C.c 4-C.c!0x2 5-C.o!0x2 1:0x1:2 1:0x1:3 1:0x1:4 1:0x1:5 2:0x6:5 5:0x6:4"
      }}
    , (rules_test[]) {{
          nodes : "A/B.xapi"
        , base : "A"
        , artifact : "B.xapi"
        , artifact_variant : "xapi"
        , rules : (char*[]) {
              "rule B.? : C.?.o"
            , 0
          }
        , result_graph : "1-A 2-B.xapi 3-C.xapi.o!0x2 1:0x1:2 1:0x1:3 2:0x6:3"
      }}
    , (rules_test[]) {{
          nodes : "A/B A/C.c A/D.c"
        , base : "A"
        , artifact : "B"
        , rules : (char*[]) {
              "rule B : {C,D}.o"
            , "rule %.o : %.c"
            , 0
          }
        , result_graph : "1-A 2-B 3-C.c 4-C.c!0x2 5-C.o!0x2 6-D.c 7-D.c!0x2 8-D.o!0x2 1:0x1:2 1:0x1:3 1:0x1:4 1:0x1:5 1:0x1:6 1:0x1:7 1:0x1:8 2:0x6:5 2:0x6:8 5:0x6:4 8:0x6:7"
      }}
    , 0
  }
};
