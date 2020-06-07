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
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "moria/graph.h"
#include "moria/operations.h"

#include "xunit.h"
#include "lookup.internal.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.internal.h"
#include "node.h"
#include "node_operations.h"
#include "pattern.h"
#include "filesystem.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "variant.h"
#include "node_operations_test.h"

typedef struct graph_refresh_test {
  XUNITTEST;

  // inputs
  char * setup_operations;  // create the starting graph
  char * module;            // context node for applying rules

  struct test_rule {
    char * match_pattern;
    char * generate_pattern;
    rule_direction dir;
    rule_cardinality card;
    edge_type relation;
  } **rules;

  char * operations;        // graph mutations to perform

  // outputs
  char * graph;             // expected graph at the end
} graph_refresh_test;

static xapi graph_refresh_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(filesystem_setup);
  fatal(logging_finalize);

  fatal(module_setup);
  fatal(graph_setup);
  fatal(variant_setup);
  fatal(node_setup);

  finally : coda;
}

static xapi graph_refresh_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(graph_cleanup);
  fatal(variant_cleanup);
  fatal(module_cleanup);
  fatal(filesystem_cleanup);

  fatal(valyria_unload);
  fatal(moria_unload);

  finally : coda;
}

//
// tests
//

static xapi graph_refresh_test_entry(xunit_test * _test)
{
  enter;

  graph_refresh_test * test = (graph_refresh_test *)_test;

  narrator * N = 0;
  pattern_parser * parser = 0;
  operations_parser * op_parser = 0;
  list * operations = 0;
  yyu_location loc;
  module *module;
  rule * rule;
  int x;
  llist modules;
  pattern * match_pat = 0;
  pattern * generate_pat = 0;
  rule_run_context rule_ctx = { 0 };
  const char * graph;
  set *variants = 0;

  fatal(rule_run_context_xinit, &rule_ctx);
  fatal(rule_run_context_begin, &rule_ctx);

  fatal(narrator_growing_create, &N);
  fatal(pattern_parser_create, &parser);
  fatal(operations_parser_create, &op_parser);
  llist_init_node(&modules);
  fatal(module_create, &module);
  rule_ctx.mod = module;
  rule_ctx.modules = &modules;
  fatal(set_create, &variants);
  rule_ctx.variants = variants;

  /* setup - build the initial graph */
  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_parse, op_parser, g_graph, MMS(test->setup_operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  fatal(pattern_lookup_fragment, MMS(test->module), 0, 0, 0, 0, 0, 0, &module->dir_node);
  module->shadow_scope = module->dir_node;

  /* setup - run the rules */
  for(x = 0; x < sentinel(test->rules); x++)
  {
    typeof(*test->rules) test_rule = test->rules[x];

    // parse the patterns for the rule
    fatal(match_pattern_parse_partial, parser, test_rule->match_pattern, strlen(test_rule->match_pattern) + 2, "-test-", 0, &loc, &match_pat);
    assert_eq_u32(strlen(test_rule->match_pattern), loc.l);

    fatal(generate_pattern_parse_partial, parser, test_rule->generate_pattern, strlen(test_rule->generate_pattern) + 2, "-test-", 0, &loc, &generate_pat);
    assert_eq_u32(strlen(test_rule->generate_pattern), loc.l);

    fatal(rule_mk, &rule, g_graph, match_pat, generate_pat, 0, 0);
    match_pat = 0;
    generate_pat = 0;

    rule->dir = test_rule->dir;
    rule->card = test_rule->card;
    rule->relation = test_rule->relation;

    fatal(module_rule_associate, module, module, rule, variants, &rule_ctx.rma);

    fatal(rule_run, rule, &rule_ctx);
  }

  /* setup - replay the mutation operations */
  fatal(operations_parser_parse, op_parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  /* act - graph refresh */
  fatal(graph_full_refresh, &rule_ctx);

  // assert
  fatal(graph_say, g_graph, N);
  graph = narrator_growing_buffer(N);
  assert_eq_s(test->graph, graph);

finally:
  fatal(set_xfree, variants);
  fatal(module_xfree, module);
  pattern_free(match_pat);
  pattern_free(generate_pat);
  fatal(node_cleanup);
  fatal(narrator_xfree, N);
  fatal(operations_parser_xfree, op_parser);
  fatal(pattern_parser_xfree, parser);
  fatal(list_xfree, operations);
  fatal(rule_run_context_xdestroy, &rule_ctx);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : graph_refresh_test_unit_setup
  , xu_cleanup : graph_refresh_test_unit_cleanup
  , xu_entry : graph_refresh_test_entry
  , xu_tests : (graph_refresh_test*[]) {
      /* no operations */
      (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
        , graph :  "1-(root)!dir 2-MOD!dir 3-main.c!file 4-main.o!U|file"
                  " 5-!rule"
                  " 5:rule-dir:2"                 // rule edges
                  " 4:strong:3"                   // strong edges
                  " 2:mod-rule:5"
                  " 1:fs:2 2:fs:3 2:fs:4"         // fs edges
      }}
      /* unlinked primary file - deleted, its secondary file remains */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:main.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-main.o!U|file"
                  " 4-!rule"
                  " 4:rule-dir:2"                 // rule edges
                  " 2:mod-rule:4"
                  " 1:fs:2 2:fs:3"                // fs edges
      }}
      /* unlinked secondary file - marked notexists, strong edge still in effect */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:main.o"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-main.c!file 4-main.o!U|file"
                  " 5-!rule"
                  " 5:rule-dir:2"                 // rule edges
                  " 4:strong:3"                   // strong edges
                  " 2:mod-rule:5"
                  " 1:fs:2 2:fs:3 2:fs:4"         // fs edges
      }}
      /* unlinked primary+secondary file - both are deleted */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:main.c"
                  " =MOD:fs:main.o"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-!rule"
                  " 3:rule-dir:2"
                  " 2:mod-rule:3"
                  " 1:fs:2"
      }}

      /* unlinked tree of primary files */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/src/main.c"
          " MOD/src/sub/args.c"
          " MOD/src/sub/args.h"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " @args.o @main.o"
                  " =src:fs:main.c"
                  " =sub:fs:args.c"
                  " =sub:fs:args.h"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-src!dir 4-sub!dir"
                  " 5-args.o!I|file 6-main.o!I|file"
                  " 7-!rule"
                  " 7:rule-dir:2 7:rule-dir:3 7:rule-dir:4"
                  " 2:mod-rule:7"
                  " 1:fs:2 2:fs:3 3:fs:4 3:fs:6 4:fs:5"
      }}

      /* unlinked tree of all files */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/src/main.c"
          " MOD/src/sub/args.c"
          " MOD/src/sub/args.h"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " =MOD/src:fs:main.c"
                  " =MOD/src:fs:main.o"
                  " =MOD/src/sub:fs:args.c"
                  " =MOD/src/sub:fs:args.h"
                  " =MOD/src/sub:fs:args.o"
                  " =MOD/src:fs:sub"
                  " =MOD:fs:src"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-!rule"
                  " 3:rule-dir:2"
                  " 2:mod-rule:3"
                  " 1:fs:2"
      }}

      /* new primary files added - new secondary files which are not marked not-exists */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/src/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " MOD/src/sub/args.c"
        , graph :  "1-(root)!dir 2-MOD!dir 3-src!dir 4-sub!dir"
                  " 5-args.c!file 6-args.o!U|file 7-main.c!file 8-main.o!U|file"
                  " 9-!rule"
                  " 9:rule-dir:2 9:rule-dir:3 9:rule-dir:4"
                  " 6:strong:5 8:strong:7"
                  " 2:mod-rule:9"
                  " 1:fs:2 2:fs:3 3:fs:4 3:fs:7 3:fs:8 4:fs:5 4:fs:6"
      }}

      /* primary file introduced, transitive target edge created */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
            , (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
          , 0
        }
        , operations : ""
                  " MOD/args.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!U|file 6-main.c!file 7-main.o!U|file"
                  " 8-!rule 9-!rule"
                  " 8:rule-dir:2 9:rule-dir:2"
                  " 4:strong:3 5:strong:4 5:strong:7 7:strong:6"
                  " 2:mod-rule:8 2:mod-rule:9"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5 2:fs:6 2:fs:7"
      }}

      /* primary file introduced, transitive target edge created, consumers invalidated */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
            , (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
          , 0
        }
        , operations : ""
                  " @exe @main.o"
                  " MOD/args.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!I|file 6-main.c!file 7-main.o!file"
                  " 8-!rule 9-!rule"
                  " 8:rule-dir:2 9:rule-dir:2"
                  " 4:strong:3 5:strong:4 5:strong:7 7:strong:6"
                  " 2:mod-rule:8 2:mod-rule:9"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5 2:fs:6 2:fs:7"
      }}

      /* primary file unlinked, transitive target edge removed */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
          " MOD/args.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
            , (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:args.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.o!U|file 4-exe!U|file 5-main.c!file 6-main.o!U|file"
                  " 7-!rule 8-!rule"
                  " 7:rule-dir:2 8:rule-dir:2"
                  " 4:strong:3 4:strong:6 6:strong:5"
                  " 2:mod-rule:7 2:mod-rule:8"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5 2:fs:6"
      }}

      /* transitive target unlinked, dependent edges unchanged */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
          " MOD/args.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
            , (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:exe"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!U|file 6-main.c!file 7-main.o!U|file"
                  " 8-!rule 9-!rule"
                  " 8:rule-dir:2 9:rule-dir:2"
                  " 4:strong:3 5:strong:4 5:strong:7 7:strong:6"
                  " 2:mod-rule:8 2:mod-rule:9"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5 2:fs:6 2:fs:7"
      }}

      /* secondary file unlinked, transitive edges invalidated */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
            , (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
          , 0
        }
        , operations : ""
                  " @main.o"
                  " =MOD:fs:main.o"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-exe!U|file 4-main.c!file 5-main.o!X|file"
                  " 6-!rule 7-!rule"
                  " 6:rule-dir:2 7:rule-dir:2"
                  " 5:strong:4"
                  " 2:mod-rule:6 2:mod-rule:7"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5"
      }}

      /* secondary file unlinked, transitive edges are invalidated */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
            , (struct test_rule[]) {{
                  match_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_TYPE_STRONG
              }}
          , 0
        }
        , operations : ""
                  " @exe @main.o"
                  " =MOD:fs:main.o"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-exe!I|file 4-main.c!file 5-main.o!X|file"
                  " 6-!rule 7-!rule"
                  " 6:rule-dir:2 7:rule-dir:2"
                  " 5:strong:4"
                  " 2:mod-rule:6 2:mod-rule:7"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5"
      }}

      /* unlinked many intermediate files */
    , (graph_refresh_test[]) {{
          setup_operations : ""
          " MOD/main.c"
          " MOD/args.c"
          " MOD/args.h"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                match_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , (struct test_rule[]) {{
                match_pattern : (char[]) { "**/*.o\0" }
              , generate_pattern : (char[]) { "exe\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_TYPE_STRONG
            }}
          , 0
        }
        , operations : ""
                  " @exe @main.o @args.o"
                  " =MOD:fs:main.c"
                  " =MOD:fs:main.o"
                  " =MOD:fs:args.c"
                  " =MOD:fs:args.o"
                  " =MOD:fs:args.h"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-exe!I|file"
                  " 4-!rule 5-!rule"
                  " 4:rule-dir:2 5:rule-dir:2"
                  " 2:mod-rule:4 2:mod-rule:5"
                  " 1:fs:2 2:fs:3"
      }}
    , 0
  }
};
