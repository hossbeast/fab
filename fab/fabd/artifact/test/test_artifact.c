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
#include "xunit.h"
#include "xunit/assert.h"
#include "xlinux/xstdlib.h"

#include "moria/graph.h"
#include "moria/vertex.h"
#include "valyria/list.h"
#include "valyria/map.h"

#include "artifact.h"
#include "generate.h"
#include "ff_node.h"
#include "ff_node_artifact.h"
#include "ff_node_artifact_pattern.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "ff_node_rule.h"
#include "ff_parser.h"
#include "logging.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "node.h"
#include "path.h"

typedef struct artifact_test
{
  xunit_test;

  char * context;
  char * ff;

  char * expected_graph;
  char * expected_artifacts;
} artifact_test;

static xapi artifact_test_unit_setup(xunit_unit * unit)
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
    fatal(path_createw, &(*n)->name, label, len);
    fatal(map_set, node_map, label, len, *n);
  }

  finally : coda;
}

static int artifact_compare(const void * _A, const void * _B, void * arg)
{
  const artifact * A = _A;
  const artifact * B = _B;

  return     strcmp(A->node->name->name, B->node->name->name)
          ?: memncmp(A->variant, A->variant_len, B->variant, B->variant_len);
}

//
// tests
//

static xapi artifact_test_entry(xunit_test * _test)
{
  enter;

  artifact_test * test = (artifact_test *)_test;

  map * node_map = 0;
  narrator * N = 0;
  ff_parser * parser = 0;
  ff_node * ffn = 0;
  list * artifacts_list = 0;
  node * context;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &node_map);
  fatal(list_createx, &artifacts_list, wfree, 0, 0);

  // context node
  fatal(get_node, node_map, MMS(test->context), &context);

  // parse the pattern
  fatal(ff_parser_parse, &parser, MMS(test->ff), 0, &ffn);
  ff_node_artifact_pattern * pattern = ((ff_node_artifact*)ffn)->pattern;

  // act
  fatal(artifact_pattern_generate, pattern, context, artifacts_list);

  // ordered list of edges
  fatal(narrator_xreset, N);
  fatal(graph_say, g_node_graph, 0, N);
  const char * actual_graph = narrator_growing_buffer(N);
  size_t actual_graph_len = narrator_growing_size(N);
  assert_eq_w(test->expected_graph, strlen(test->expected_graph), actual_graph, actual_graph_len);

  // ordered list of nodes
  fatal(narrator_xreset, N);
  list_sort(artifacts_list, artifact_compare, 0);
  int x;
  for(x = 0; x < artifacts_list->l; x++)
  {
    artifact * af = list_get(artifacts_list, x);
    if(x)
      xsays(" ");
    xsays(af->node->name->name);
    xsays("?");
    xsayw(af->variant, af->variant_len);
  }
  const char * actual_artifacts = narrator_growing_buffer(N);
  size_t actual_artifacts_len = narrator_growing_size(N);
  assert_eq_w(test->expected_artifacts, strlen(test->expected_artifacts), actual_artifacts, actual_artifacts_len);

finally:
  fatal(node_cleanup);
  fatal(map_xfree, node_map);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  ffn_free(ffn);
  fatal(list_xfree, artifacts_list);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : artifact_test_unit_setup
  , xu_entry : artifact_test_entry
  , xu_tests : (xunit_test*[]) {
    /* single node */
      (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.{a,b,c}?"
        , expected_graph : "1-C 2-X.a!0x2 3-X.b!0x2 4-X.c!0x2 1:0x1:2 1:0x1:3 1:0x1:4"
        , expected_artifacts : "X.a?a X.b?b X.c?c"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.{[a-c]}?"
        , expected_graph : "1-C 2-X.a!0x2 3-X.b!0x2 4-X.c!0x2 1:0x1:2 1:0x1:3 1:0x1:4"
        , expected_artifacts : "X.a?a X.b?b X.c?c"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.{[a-c]}?d"
        , expected_graph : "1-C 2-X.ad!0x2 3-X.bd!0x2 4-X.cd!0x2 1:0x1:2 1:0x1:3 1:0x1:4"
        , expected_artifacts : "X.ad?a X.bd?b X.cd?c"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.[a-b]?"
        , expected_graph : "1-C 2-X.a!0x2 3-X.b!0x2 1:0x1:2 1:0x1:3"
        , expected_artifacts : "X.a?a X.b?b"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.[a-b]?c"
        , expected_graph : "1-C 2-X.ac!0x2 3-X.bc!0x2 1:0x1:2 1:0x1:3"
        , expected_artifacts : "X.ac?a X.bc?b"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.{a,b}?w"
        , expected_graph : "1-C 2-X.aw!0x2 3-X.bw!0x2 1:0x1:2 1:0x1:3"
        , expected_artifacts : "X.aw?a X.bw?b"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.{{a,b}{.c,}}?"
        , expected_graph : "1-C 2-X.a!0x2 3-X.a.c!0x2 4-X.b!0x2 5-X.b.c!0x2 1:0x1:2 1:0x1:3 1:0x1:4 1:0x1:5"
        , expected_artifacts : "X.a?a X.a.c?a.c X.b?b X.b.c?b.c"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X.{a{,.c},b{,.c}}?"
        , expected_graph : "1-C 2-X.a!0x2 3-X.a.c!0x2 4-X.b!0x2 5-X.b.c!0x2 1:0x1:2 1:0x1:3 1:0x1:4 1:0x1:5"
        , expected_artifacts : "X.a?a X.a.c?a.c X.b?b X.b.c?b.c"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X{Y,Z}.{a,b}?"
        , expected_graph : "1-C 2-XY.a!0x2 3-XY.b!0x2 4-XZ.a!0x2 5-XZ.b!0x2 1:0x1:2 1:0x1:3 1:0x1:4 1:0x1:5"
        , expected_artifacts : "XY.a?a XY.b?b XZ.a?a XZ.b?b"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X{Y}?.{a}"
        , expected_graph : "1-C 2-XY.a!0x2 1:0x1:2"
        , expected_artifacts : "XY.a?Y"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X{Y,}?.{a}"
        , expected_graph : "1-C 2-X.a!0x2 3-XY.a!0x2 1:0x1:2 1:0x1:3"
        , expected_artifacts : "X.a? XY.a?Y"
      }}
    , (artifact_test[]) {{
          context : "C"
        , ff : "artifact X{Y,Z}?.{a,b}"
        , expected_graph : "1-C 2-XY.a!0x2 3-XY.b!0x2 4-XZ.a!0x2 5-XZ.b!0x2 1:0x1:2 1:0x1:3 1:0x1:4 1:0x1:5"
        , expected_artifacts : "XY.a?Y XY.b?Y XZ.a?Z XZ.b?Z"
      }}
    , 0
  }
};
