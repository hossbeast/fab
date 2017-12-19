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

#include "generate.h"
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

static map * attrs_definitions;

typedef struct pattern_generate_test
{
  xunit_test;

  char * context;
  char * stem;
  char * variant;
  char * pattern;

  char * graph;
  char * nodes;
} pattern_generate_test;

static xapi pattern_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_finalize);

  // attrs name map
  fatal(map_create, &attrs_definitions);
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_FS }, sizeof(uint32_t), "FS");
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_STRONG }, sizeof(uint32_t), "ST");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_FILE }, sizeof(uint32_t), "FILE");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_DIR}, sizeof(uint32_t), "DIR");

  finally : coda;
}

static xapi pattern_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(map_xfree, attrs_definitions);

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
  map * scope;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &node_map);
  fatal(list_create, &nodes_list);
  fatal(map_create, &scope);

  // context
  fatal(node_createw, &context, NODE_FSTYPE_DIR, (void*)42, 0, MMS(test->context));

  artifact af = { variant : test->variant };
  if(test->variant)
    af.variant_len = strlen(test->variant);

  // parse the pattern
  fatal(ff_parser_parse, &parser, MMS(test->pattern), 0, &ffn);
  ff_node_pattern * pattern = ((ff_node_rule*)ffn)->antecedent_list->chain;

  // act
  fatal(pattern_generate
    , pattern
    , context
    , scope
    , &af
    , test->stem
    , test->stem ? strlen(test->stem) : 0
    , nodes_list
    , false
  );

  // ordered list of edges
  fatal(narrator_xreset, N);
  fatal(graph_say, g_node_graph, attrs_definitions, N);
  const char * graph = narrator_growing_buffer(N);
  size_t graph_len = narrator_growing_size(N);
  assert_eq_s(test->graph, graph);
  assert_eq_w(test->graph, strlen(test->graph), graph, graph_len);

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
  const char * nodes = narrator_growing_buffer(N);
  size_t nodes_len = narrator_growing_size(N);
  assert_eq_w(test->nodes, strlen(test->nodes), nodes, nodes_len);

finally:
  xapi_infos("pattern", test->pattern);

  fatal(node_cleanup);
  fatal(map_xfree, node_map);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  ffn_free(ffn);
  fatal(list_xfree, nodes_list);
  fatal(map_xfree, scope);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : pattern_test_unit_setup
  , xu_cleanup : pattern_test_unit_cleanup
  , xu_entry : pattern_generate_test_entry
  , xu_tests : (xunit_test*[]) {
    /* single node */
      (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B"
        , graph :  "1-A!DIR 2-B!FILE"
                  " 1:FS:2"
        , nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B/C"
        , graph :  "1-A!DIR 2-B!DIR 3-C!FILE"
                  " 1:FS:2 2:FS:3"
        , nodes : "C"
      }}

    /* alternations */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B}"
        , graph :  "1-A!DIR 2-B!FILE"
                  " 1:FS:2"
        , nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,}"
        , graph :  "1-A!DIR 2-B!FILE"
                  " 1:FS:2"
        , nodes : "B"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,C}"
        , graph :  "1-A!DIR 2-B!FILE 3-C!FILE"
                  " 1:FS:2 1:FS:3"
        , nodes : "B C"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,C,D}"
        , graph :  "1-A!DIR 2-B!FILE 3-C!FILE 4-D!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
        , nodes : "B C D"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B/{C,D}"
        , graph :  "1-A!DIR 2-B!DIR 3-C!FILE 4-D!FILE"
                  " 1:FS:2 2:FS:3 2:FS:4"
        , nodes : "C D"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {C,D}/F"
        , graph :  "1-A!DIR 2-C!DIR 3-D!DIR 4-F!FILE 5-F!FILE"
                  " 1:FS:2 1:FS:3 2:FS:4 3:FS:5"
        , nodes : "F F"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B/{C,D}/F"
        , graph :  "1-A!DIR 2-B!DIR 3-C!DIR 4-D!DIR 5-F!FILE 6-F!FILE"
                  " 1:FS:2 2:FS:3 2:FS:4 3:FS:5 4:FS:6"
        , nodes : "F F"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,C}/{D,E}"
        , graph :  "1-A!DIR 2-B!DIR 3-C!DIR 4-D!FILE 5-D!FILE 6-E!FILE 7-E!FILE"
                  " 1:FS:2 1:FS:3 2:FS:4 2:FS:6 3:FS:5 3:FS:7"
        , nodes : "D D E E"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B{C,D}"
        , graph :  "1-A!DIR 2-BC!FILE 3-BD!FILE"
                  " 1:FS:2 1:FS:3"
        , nodes : "BC BD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B{C,D,}"
        , graph :  "1-A!DIR 2-B!FILE 3-BC!FILE 4-BD!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
        , nodes : "B BC BD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {C,D}F"
        , graph :  "1-A!DIR 2-CF!FILE 3-DF!FILE"
                  " 1:FS:2 1:FS:3"
        , nodes : "CF DF"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B}{D}"
        , graph : "A:BD"
        , graph :  "1-A!DIR 2-BD!FILE"
                  " 1:FS:2"
        , nodes : "BD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,C}{D}"
        , graph :  "1-A!DIR 2-BD!FILE 3-CD!FILE"
                  " 1:FS:2 1:FS:3"
        , nodes : "BD CD"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B}{D,E}"
        , graph :  "1-A!DIR 2-BD!FILE 3-BE!FILE"
                  " 1:FS:2 1:FS:3"
        , nodes : "BD BE"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,C}{D,E}"
        , graph :  "1-A!DIR 2-BD!FILE 3-BE!FILE 4-CD!FILE 5-CE!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4 1:FS:5"
        , nodes : "BD BE CD CE"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {B,}{C,}"
        , nodes : "B BC C"
        , graph :  "1-A!DIR 2-B!FILE 3-BC!FILE 4-C!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{B}}"
        , nodes : "B"
        , graph :  "1-A!DIR 2-B!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{B,C}}"
        , nodes : "B C"
        , graph :  "1-A!DIR 2-B!FILE 3-C!FILE"
                  " 1:FS:2 1:FS:3"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {X{B,C}}"
        , nodes : "XB XC"
        , graph :  "1-A!DIR 2-XB!FILE 3-XC!FILE"
                  " 1:FS:2 1:FS:3"
      }}

    /* classes */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : [BCD]"
        , nodes : "B C D"
        , graph :  "1-A!DIR 2-B!FILE 3-C!FILE 4-D!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : X[BCD]"
        , nodes : "XB XC XD"
        , graph :  "1-A!DIR 2-XB!FILE 3-XC!FILE 4-XD!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : [BCD]Y"
        , nodes : "BY CY DY"
        , graph :  "1-A!DIR 2-BY!FILE 3-CY!FILE 4-DY!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : X[BCD]Y"
        , nodes : "XBY XCY XDY"
        , graph :  "1-A!DIR 2-XBY!FILE 3-XCY!FILE 4-XDY!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}

    /* ranges */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : [B-E]"
        , nodes : "B C D E"
        , graph :  "1-A!DIR 2-B!FILE 3-C!FILE 4-D!FILE 5-E!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4 1:FS:5"
      }}

    /* nested alternations */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{C}}"
        , nodes : "C"
        , graph :  "1-A!DIR 2-C!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B{{C}}"
        , nodes : "BC"
        , graph :  "1-A!DIR 2-BC!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{C}}D"
        , nodes : "CD"
        , graph :  "1-A!DIR 2-CD!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{{C}}}"
        , nodes : "C"
        , graph :  "1-A!DIR 2-C!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B{{{C}}}"
        , nodes : "BC"
        , graph :  "1-A!DIR 2-BC!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{{C}}}D"
        , nodes : "CD"
        , graph :  "1-A!DIR 2-CD!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{{C}}D}"
        , nodes : "CD"
        , graph :  "1-A!DIR 2-CD!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {{{C}D}}"
        , nodes : "CD"
        , graph :  "1-A!DIR 2-CD!FILE"
                  " 1:FS:2"
      }}

    /* nested alternations and classes */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {[BCD]}"
        , nodes : "B C D"
        , graph :  "1-A!DIR 2-B!FILE 3-C!FILE 4-D!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {X[BCD]}"
        , nodes : "XB XC XD"
        , graph :  "1-A!DIR 2-XB!FILE 3-XC!FILE 4-XD!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {[BCD]Y}"
        , nodes : "BY CY DY"
        , graph :  "1-A!DIR 2-BY!FILE 3-CY!FILE 4-DY!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : X{[BCD]}"
        , nodes : "XB XC XD"
        , graph :  "1-A!DIR 2-XB!FILE 3-XC!FILE 4-XD!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : {[BCD]}Y"
        , nodes : "BY CY DY"
        , graph :  "1-A!DIR 2-BY!FILE 3-CY!FILE 4-DY!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B{[CD]}Y"
        , graph : "A:BCY A:BDY"
        , nodes : "BCY BDY"
        , graph :  "1-A!DIR 2-BCY!FILE 3-BDY!FILE"
                  " 1:FS:2 1:FS:3"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B{{,[CD]}}Y"
        , nodes : "BCY BDY BY"
        , graph :  "1-A!DIR 2-BCY!FILE 3-BDY!FILE 4-BY!FILE"
                  " 1:FS:2 1:FS:3 1:FS:4"
      }}

    /* stem */
    , (pattern_generate_test[]) {{
          context : "A"
        , stem : "foo"
        , pattern : "rule consequent-ignored : %"
        , nodes : "foo"
        , graph :  "1-A!DIR 2-foo!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , stem : "bar"
        , pattern : "rule consequent-ignored : node.%"
        , nodes : "node.bar"
        , graph :  "1-A!DIR 2-node.bar!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , stem : "bar"
        , pattern : "rule consequent-ignored : node/%"
        , nodes : "bar"
        , graph :  "1-A!DIR 2-bar!FILE 3-node!DIR"
                  " 1:FS:3 3:FS:2"
      }}

    /* variant */
    , (pattern_generate_test[]) {{
          context : "A"
        , variant : "xapi"
        , pattern : "rule consequent-ignored : B.?"
        , nodes : "B.xapi"
        , graph :  "1-A!DIR 2-B.xapi!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , variant : "xapi"
        , pattern : "rule consequent-ignored : ?.B"
        , nodes : "xapi.B"
        , graph :  "1-A!DIR 2-xapi.B!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , variant : "xapi"
        , pattern : "rule consequent-ignored : B?C"
        , nodes : "BxapiC"
        , graph :  "1-A!DIR 2-BxapiC!FILE"
                  " 1:FS:2"
      }}

    /* pathological cases */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B/{C,}"
        , nodes : "C"
        , graph :  "1-A!DIR 2-B!DIR 3-C!FILE"
                  " 1:FS:2 2:FS:3"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : B/{,C}"
        , nodes : "C"
        , graph :  "1-A!DIR 2-B!DIR 3-C!FILE"
                  " 1:FS:2 2:FS:3"
      }}

    /* path variations */
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : ./B"
        , nodes : "B"
        , graph :  "1-A!DIR 2-B!FILE"
                  " 1:FS:2"
      }}
    , (pattern_generate_test[]) {{
          context : "A"
        , pattern : "rule consequent-ignored : ././B"
        , nodes : "B"
        , graph :  "1-A!DIR 2-B!FILE"
                  " 1:FS:2"
      }}
    , 0
  }
};
