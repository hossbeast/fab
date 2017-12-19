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
#include "ff_parser.h"
#include "rule.internal.h"
#include "node.h"
#include "node_operations.h"
#include "artifact.h"

static map * attrs_definitions;

typedef struct rules_test
{
  xunit_test;

  char * operations;          // create the starting graph
  char * base;                // context node for applying rules
  char * artifact;            // artifact node to apply rules for
  char * artifact_variant;    // artifact variant to apply rules for
  char ** rules;              // rules to apply
  char * graph;               // expectant result graph
} rules_test;

static xapi rules_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  // attrs name map for graph_say
  fatal(map_create, &attrs_definitions);
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_FS }, sizeof(uint32_t), "FS");
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_STRONG }, sizeof(uint32_t), "ST");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_FILE }, sizeof(uint32_t), "FILE");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_DIR }, sizeof(uint32_t), "DIR");

  finally : coda;
}

static xapi rules_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(map_xfree, attrs_definitions);

  finally : coda;
}

//
// tests
//

static xapi rules_test_entry(xunit_test * _test)
{
  enter;

  rules_test * test = (rules_test *)_test;

  narrator * N = 0;
  ff_parser * parser = 0;
  ff_node * ffn = 0;
  node * artifact_node;
  node * base = 0;
  list * rules_list = 0;
  list * rules_lists = 0;
  list * operations = 0;
  list * antecedents = 0;
  list * consequents = 0;
  map * scope = 0;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(list_create, &antecedents);
  fatal(list_create, &consequents);

  // setup initial graph
  fatal(operations_parser_parse, 0, MMS(test->operations), &operations);
  fatal(operations_perform, g_node_graph, node_operations_dispatch, operations);

  // parse the rules
  fatal(map_create, &scope);
  fatal(list_create, &rules_lists);
  fatal(list_createx, &rules_list, ffn_free, 0, 0);
  char ** rules = test->rules;
  while(*rules)
  {
    fatal(ff_parser_parse, &parser, MMS(*rules), 0, &ffn);
    fatal(list_push, rules_list, ffn);
    ffn = 0;

    rules++;
  }
  fatal(list_push, rules_lists, rules_list);

  // apply rules, starting with the artifact
  fatal(node_lookup_path, 0, test->base, &base);
  fatal(node_lookup_path, 0, test->artifact, &artifact_node);

  artifact af = {
      node : artifact_node
    , variant : test->artifact_variant
    , variant_len : test->artifact_variant ? strlen(test->artifact_variant) : 0
  };

  fatal(rules_apply_rules, af.node, rules_lists, base, scope, &af, antecedents, consequents);

  // ordered list of edges
  fatal(graph_say, g_node_graph, attrs_definitions, N);
  const char * graph = narrator_growing_buffer(N);
  assert_eq_s(test->graph, graph);

finally:
  fatal(node_cleanup);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  ffn_free(ffn);
  fatal(list_xfree, rules_list);
  fatal(list_xfree, rules_lists);
  fatal(list_xfree, consequents);
  fatal(list_xfree, antecedents);
  fatal(list_xfree, operations);
  fatal(map_xfree, scope);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : rules_test_unit_setup
  , xu_cleanup : rules_test_unit_cleanup
  , xu_entry : rules_test_entry
  , xu_tests : (xunit_test*[]) {
      (rules_test[]) {{
          operations : "A/B A/C.c"
        , base : "A"
        , artifact : "B"
        , rules : (char*[]) {
              "rule B : C.o"
            , "rule C.o : C.c"
            , 0
          }
        , graph :  "1-A!DIR 2-B!FILE 3-C.c!FILE 4-C.o!FILE"
                     " 1:FS:2 1:FS:3 1:FS:4"
                     " 2:ST:4"
      }}
    , (rules_test[]) {{
          operations : "A/B.xapi"
        , base : "A"
        , artifact : "B.xapi"
        , artifact_variant : "xapi"
        , rules : (char*[]) {
              "rule B.? : C.?.o"
            , 0
          }
        , graph : "1-A!DIR 2-B.xapi!FILE 3-C.xapi.o!FILE"
                    " 1:FS:2 1:FS:3"
                    " 2:ST:3"
      }}
    , (rules_test[]) {{
          operations : "A/B A/C.c A/D.c"
        , base : "A"
        , artifact : "B"
        , rules : (char*[]) {
              "rule B : {C,D}.o"
            , "rule %.o : %.c"
            , 0
          }
        , graph : "1-A!DIR 2-B!FILE 3-C.c!FILE 4-C.o!FILE 5-D.c!FILE 6-D.o!FILE"
                    " 1:FS:2 1:FS:3 1:FS:4 1:FS:5 1:FS:6"
                    " 2:ST:4 2:ST:6"
      }}
    , 0
  }
};
