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

#include "moria/graph.h"
#include "moria/vertex.h"
#include "valyria/list.h"
#include "valyria/map.h"

#include "pattern/generate.h"
#include "ff_node.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "ff_node_rule.h"
#include "ff_parser.h"
#include "logging.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "node.h"
#include "path.h"
#include "artifact.h"

typedef struct pattern_generate_test
{
  xunit_test;

  char * context;
  char * stem;
  char * variant;
  char * pattern;

  char * expected_graph;
  char * expected_nodes;
} pattern_generate_test;

static xapi pattern_test_unit_setup(xunit_unit * unit)
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
    fatal(graph_vertex_createw, &v, g_node_graph, 0, label, len);
    *n = vertex_value(v);
    fatal(path_createw, &(*n)->name, label, len);
    fatal(map_set, node_map, label, len, *n);
  }

  finally : coda;
}

static int node_compare(const void * _A, const void * _B, void * arg)
{
  const node * A = _A;
  const node * B = _B;

  return strcmp(A->name->name, B->name->name);
}

//
// tests
//

static xapi pattern_generate_test_entry(xunit_test * _test)
{
  enter;

  pattern_generate_test * test = (pattern_generate_test *)_test;

  map * node_map = 0;
  narrator * N = 0;
  ff_parser * parser = 0;
  ff_node * ffn = 0;
  list * nodes_list = 0;
  node * context;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &node_map);
  fatal(list_create, &nodes_list);

  // context
  fatal(get_node, node_map, MMS(test->context), &context);
  artifact af = { variant : test->variant };
  if(test->variant)
    af.variant_len = strlen(test->variant);

  // parse the pattern
  fatal(ff_parser_parse, &parser, MMS(test->pattern), 0, &ffn);
  ff_node_pattern * pattern = ((ff_node_rule*)ffn)->antecedent_list->chain;

  // act
  fatal(pattern_generate, pattern, context, &af, test->stem, test->stem ? strlen(test->stem) : 0, nodes_list, false);

  // ordered list of edges
  fatal(narrator_xreset, N);
  fatal(graph_say, g_node_graph, NODE_RELATION_FS, N);
  const char * graph = narrator_growing_buffer(N);
  assert_eq_s(test->expected_graph, graph);

  // ordered list of nodes
  fatal(narrator_xreset, N);
  list_sort(nodes_list, node_compare,  0);
  int x;
  for(x = 0; x < nodes_list->l; x++)
  {
    node * n = list_get(nodes_list, x);
    if(x)
      xsays(" ");
    xsays(n->name->name);
  }
  const char * node_list = narrator_growing_buffer(N);
  assert_eq_s(test->expected_nodes, node_list);

finally:
  xapi_infos("pattern", test->pattern);

  fatal(node_cleanup);
  fatal(map_xfree, node_map);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  ffn_free(ffn);
  fatal(list_xfree, nodes_list);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : pattern_test_unit_setup
  , xu_entry : pattern_generate_test_entry
  , xu_tests : (xunit_test*[]) {
    /* single node */
      (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B"
        , expected_graph : "A:B"
        , expected_nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B/C"
        , expected_graph : "A:B B:C"
        , expected_nodes : "C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B/C"
        , expected_graph : "A:B B:C"
        , expected_nodes : "C"
      }}
    , (pattern_generate_test[]) {{
          context : "B"
        , pattern : "rule xunit : C/D"
        , expected_graph : "B:C C:D"
        , expected_nodes : "D"
      }}

    /* alternations */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B}"
        , expected_graph : "A:B"
        , expected_nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,}"
        , expected_graph : "A:B"
        , expected_nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,C}"
        , expected_graph : "A:B A:C"
        , expected_nodes : "B C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,C,D}"
        , expected_graph : "A:B A:C A:D"
        , expected_nodes : "B C D"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B/{C,D}"
        , expected_graph : "A:B B:C B:D"
        , expected_nodes : "C D"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {C,D}/F"
        , expected_graph : "A:C A:D C:F D:F"
        , expected_nodes : "F F"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B/{C,D}/F"
        , expected_graph : "A:B B:C B:D C:F D:F"
        , expected_nodes : "F F"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,C}/{D,E}"
        , expected_graph : "A:B A:C B:D B:E C:D C:E"
        , expected_nodes : "D D E E"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B{C,D}"
        , expected_graph : "A:BC A:BD"
        , expected_nodes : "BC BD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B{C,D,}"
        , expected_graph : "A:B A:BC A:BD"
        , expected_nodes : "B BC BD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {C,D}F"
        , expected_graph : "A:CF A:DF"
        , expected_nodes : "CF DF"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B}{D}"
        , expected_graph : "A:BD"
        , expected_nodes : "BD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,C}{D}"
        , expected_graph : "A:BD A:CD"
        , expected_nodes : "BD CD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B}{D,E}"
        , expected_graph : "A:BD A:BE"
        , expected_nodes : "BD BE"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,C}{D,E}"
        , expected_graph : "A:BD A:BE A:CD A:CE"
        , expected_nodes : "BD BE CD CE"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {B,}{C,}"
        , expected_nodes : "B BC C"
        , expected_graph : "A:B A:BC A:C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{B}}"
        , expected_graph : "A:B"
        , expected_nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{B,C}}"
        , expected_graph : "A:B A:C"
        , expected_nodes : "B C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {X{B,C}}"
        , expected_graph : "A:XB A:XC"
        , expected_nodes : "XB XC"
      }}

    /* classes */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : [BCD]"
        , expected_graph : "A:B A:C A:D"
        , expected_nodes : "B C D"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : X[BCD]"
        , expected_graph : "A:XB A:XC A:XD"
        , expected_nodes : "XB XC XD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : [BCD]Y"
        , expected_graph : "A:BY A:CY A:DY"
        , expected_nodes : "BY CY DY"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : X[BCD]Y"
        , expected_graph : "A:XBY A:XCY A:XDY"
        , expected_nodes : "XBY XCY XDY"
      }}

    /* ranges */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : [B-E]"
        , expected_graph : "A:B A:C A:D A:E"
        , expected_nodes : "B C D E"
      }}

    /* nested alternations */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{C}}"
        , expected_graph : "A:C"
        , expected_nodes : "C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B{{C}}"
        , expected_graph : "A:BC"
        , expected_nodes : "BC"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{C}}D"
        , expected_graph : "A:CD"
        , expected_nodes : "CD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{{C}}}"
        , expected_graph : "A:C"
        , expected_nodes : "C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B{{{C}}}"
        , expected_graph : "A:BC"
        , expected_nodes : "BC"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{{C}}}D"
        , expected_graph : "A:CD"
        , expected_nodes : "CD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{{C}}D}"
        , expected_graph : "A:CD"
        , expected_nodes : "CD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {{{C}D}}"
        , expected_graph : "A:CD"
        , expected_nodes : "CD"
      }}

    /* nested alternations and classes */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {[BCD]}"
        , expected_graph : "A:B A:C A:D"
        , expected_nodes : "B C D"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {X[BCD]}"
        , expected_graph : "A:XB A:XC A:XD"
        , expected_nodes : "XB XC XD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {[BCD]Y}"
        , expected_graph : "A:BY A:CY A:DY"
        , expected_nodes : "BY CY DY"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : X{[BCD]}"
        , expected_graph : "A:XB A:XC A:XD"
        , expected_nodes : "XB XC XD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : {[BCD]}Y"
        , expected_graph : "A:BY A:CY A:DY"
        , expected_nodes : "BY CY DY"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B{[CD]}Y"
        , expected_graph : "A:BCY A:BDY"
        , expected_nodes : "BCY BDY"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B{{,[CD]}}Y"
        , expected_graph : "A:BCY A:BDY A:BY"
        , expected_nodes : "BCY BDY BY"
      }}

    /* stem */
    , (pattern_generate_test[]) {{
          context : "A"
        , stem : "foo"
        , pattern : "rule xunit : %"
        , expected_graph : "A:foo"
        , expected_nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , stem : "bar"
        , pattern : "rule xunit : node.%"
        , expected_graph : "A:node.bar"
        , expected_nodes : "node.bar"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , stem : "bar"
        , pattern : "rule xunit : node/%"
        , expected_graph : "A:node node:bar"
        , expected_nodes : "bar"
      }}

    /* variant */
    , (pattern_generate_test[]) {{
          context : "A"
        , variant : "xapi"
        , pattern : "rule xunit : B.?"
        , expected_graph : "A:B.xapi"
        , expected_nodes : "B.xapi"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , variant : "xapi"
        , pattern : "rule xunit : ?.B"
        , expected_graph : "A:xapi.B"
        , expected_nodes : "xapi.B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , variant : "xapi"
        , pattern : "rule xunit : B?C"
        , expected_graph : "A:BxapiC"
        , expected_nodes : "BxapiC"
      }}

    /* pathological cases */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B/{C,}"
        , expected_graph : "A:B B:C"
        , expected_nodes : "C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule xunit : B/{,C}"
        , expected_graph : "A:B B:C"
        , expected_nodes : "C"
      }}
    , 0
  }
};
