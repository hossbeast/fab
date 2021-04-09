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
#include "moria/parser.h"
#include "value/load.h"

#include "xunit.h"
#include "lookup.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.h"
#include "fsent.h"
#include "pattern.h"
#include "filesystem.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "variant.h"
#include "node_operations_test.h"
#include "rule_module.h"
#include "rule_system.h"
#include "module_parser.h"
#include "shadow.h"
#include "system_state.h"

typedef struct rule_system_reconcile_test {
  XUNITTEST;

  // inputs
  char * setup_operations;  // create the starting graph
  char * module;            // context node for applying rules

  struct test_rule {
    char * search_pattern;
    char * generate_pattern;
    rule_direction dir;
    rule_cardinality card;
    edge_type relation;
  } **rules;

  char * operations;        // graph mutations to perform

  // outputs
  char * graph;             // expected graph at the end
} rule_system_reconcile_test;

static xapi rule_system_reconcile_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(value_load);
  fatal(filesystem_setup);
  fatal(logging_finalize);

  fatal(graph_setup);
  fatal(variant_setup);
  fatal(fsent_setup);
  fatal(module_setup);
  fatal(rule_system_setup);

/* suppress rcu-registration checks */
extern __thread bool rcu_is_registered ; rcu_is_registered = true;

  finally : coda;
}

static xapi rule_system_reconcile_test_unit_cleanup(xunit_unit * unit)
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

static xapi rule_system_reconcile_test_entry(xunit_test * _test)
{
  enter;

  rule_system_reconcile_test * test = (rule_system_reconcile_test *)_test;

  narrator_growing * N = 0;
  pattern_parser * parser = 0;
  graph_parser * op_parser = 0;
  module_parser *mod_parser = 0;

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

  fatal(pattern_parser_create, &parser);
  fatal(module_parser_create, &mod_parser);

  fatal(narrator_growing_create, &N);
  fatal(graph_parser_create, &op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  llist_init_node(&modules);
  fatal(module_alloc, &module);
  rule_ctx.mod = module;
  rule_ctx.modules = &modules;
  fatal(set_create, &variants);
  rule_ctx.variants = variants;

  /* setup - build the initial graph */
  fatal(graph_parser_operations_parse, op_parser, MMS(test->setup_operations));

  fatal(resolve_fragment, MMS(test->module), &module->dir_node);
  module->shadow_imports = module->dir_node;

  /* setup - run the rules */
  for(x = 0; x < sentinel(test->rules); x++)
  {
    typeof(*test->rules) test_rule = test->rules[x];

    // parse the patterns for the rule
    fatal(search_pattern_parse_partial, parser, test_rule->search_pattern, strlen(test_rule->search_pattern) + 2, "-test-", 0, &loc, &match_pat);
    assert_eq_u32(strlen(test_rule->search_pattern), loc.l);

    fatal(generate_pattern_parse_partial, parser, test_rule->generate_pattern, strlen(test_rule->generate_pattern) + 2, "-test-", 0, &loc, &generate_pat);
    assert_eq_u32(strlen(test_rule->generate_pattern), loc.l);

    fatal(rule_mk, &rule, &g_graph, match_pat, generate_pat, 0, 0, 0);
    match_pat = 0;
    generate_pat = 0;

    rule->dir = test_rule->dir;
    rule->card = test_rule->card;
    rule->relation = test_rule->relation;

    fatal(rule_module_connect, &rule_ctx.rme, module, module, rule, variants);

    fatal(rule_run, rule, &rule_ctx);
  }

  /* setup - perform specified mutations if any */
  fatal(graph_parser_operations_parse, op_parser, MMS(test->operations));

  /* act - reconcile rules */
  fatal(rule_system_reconcile, &rule_ctx);
  assert_eq_b(false, system_error);

  // assert
  fatal(graph_say, &N->base);
  graph = N->s;
  assert_eq_s(test->graph, graph);

finally:
  fatal(set_xfree, variants);
  fatal(module_parser_xfree, mod_parser);
  pattern_free(match_pat);
  pattern_free(generate_pat);
  fatal(fsent_cleanup);
  fatal(narrator_growing_free, N);
  fatal(graph_parser_xfree, op_parser);
  fatal(pattern_parser_xfree, parser);
  fatal(rule_run_context_xdestroy, &rule_ctx);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : rule_system_reconcile_test_unit_setup
  , xu_cleanup : rule_system_reconcile_test_unit_cleanup
  , xu_entry : rule_system_reconcile_test_entry
  , xu_tests : (rule_system_reconcile_test*[]) {
      /* no operations - dependency created */
      (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // main.c -> $^D/main.o
                search_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , 0
        }
        , operations : ""
        , graph :  "1-(root)!dir 2-MOD!dir 3-main.c!file 4-main.o!U|file"
                  " 4:depends:3"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4"
      }}
      /* unlinked primary file - deleted, and secondary file remains */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // main.c -> $^D/main.o
                search_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:main.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-main.o!U|file"
                  " 1:fs:2"
                  " 2:fs:3"
      }}
      /* unlinked secondary file - marked unlinked, depends edge still in effect */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // main.c -> $^D/main.o
                search_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:main.o"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-main.c!file 4-main.o!U|file"
                  " 4:depends:3"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4"
      }}
      /* unlinked primary+secondary file - both are deleted along with their dependency */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                search_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:main.c"
                  " =MOD:fs:main.o"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 1:fs:2"
      }}

      /* unlinked tree of primary files */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/src/main.c"
          " MOD/src/sub/args.c"
          " MOD/src/sub/args.h"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                search_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
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
                  " 1:fs:2"
                  " 2:fs:3 3:fs:4 3:fs:6 4:fs:5"
      }}

      /* unlinked tree of all files */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/src/main.c"
          " MOD/src/sub/args.c"
          " MOD/src/sub/args.h"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
                search_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , 0
        }
        , operations : ""
                  " =MOD/src:fs:main.o"
                  " =MOD/src:fs:main.c"
                  " =MOD/src/sub:fs:args.c"
                  " =MOD/src/sub:fs:args.h"
                  " =MOD/src/sub:fs:args.o"
                  " =MOD/src:fs:sub"
                  " =MOD:fs:src"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 1:fs:2"
      }}

      /* new primary files added - new secondary files which are not marked not-exists */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/src/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                search_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , 0
        }
        , operations : ""
                  " MOD/src/sub/args.c"
        , graph :  "1-(root)!dir 2-MOD!dir 3-src!dir 4-sub!dir"
                  " 5-args.c!file 6-args.o!U|file 7-main.c!file 8-main.o!U|file"
                  " 6:depends:5 8:depends:7"
                  " 1:fs:2"
                  " 2:fs:3 3:fs:4 3:fs:7 3:fs:8 4:fs:5 4:fs:6"
      }}

      /* primary file introduced, transitive target edge created */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
          , 0
        }
        , operations : ""
                  " MOD/args.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!U|file 6-main.c!file 7-main.o!U|file"
                  " 4:depends:3 5:depends:4 5:depends:7 7:depends:6"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4 2:fs:5 2:fs:6 2:fs:7"
      }}

      /* primary file introduced, transitive target edge created, consumers invalidated */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
          , 0
        }
        , operations : ""
                  " @exe @main.o"
                  " MOD/args.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!file 6-main.c!file 7-main.o!file"
                  " 4:depends:3 5:depends:4 5:depends:7 7:depends:6"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4 2:fs:5 2:fs:6 2:fs:7"
      }}

      /* primary file unlinked, transitive target edge removed */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
          " MOD/args.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:args.c"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.o!U|file 4-exe!U|file 5-main.c!file 6-main.o!U|file"
                  " 4:depends:3 4:depends:6 6:depends:5"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4 2:fs:5 2:fs:6"
      }}

      /* transitive target unlinked, dependent edges unchanged */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
          " MOD/args.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
          , 0
        }
        , operations : ""
                  " =MOD:fs:exe"
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!U|file 6-main.c!file 7-main.o!U|file"
                  " 4:depends:3 5:depends:4 5:depends:7 7:depends:6"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4 2:fs:5 2:fs:6 2:fs:7"
      }}

      /* secondary file unlinked, transitive edges invalidated */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
          , 0
        }
        , operations : ""
                  " @main.o"
                  " =MOD:fs:main.o"
        /* main.o is uncreated(U) because exe still depends on it */
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-exe!U|file 4-main.c!file 5-main.o!U|file"
                  " 3:depends:5 5:depends:4"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4 2:fs:5"
      }}

      /* secondary file unlinked, transitive edges are invalidated */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
              (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/(*).c\0" }
                , generate_pattern : (char[]) { "$^D/$1.o\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
                , relation : EDGE_DEPENDS
              }}
          , 0
        }
        , operations : ""
                  " @exe @main.o"
                  " =MOD:fs:main.o"
        /* main.o is uncreated(U) because exe still depends on it */
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-exe!I|file 4-main.c!file 5-main.o!U|file"
                  " 3:depends:5 5:depends:4"
                  " 1:fs:2"
                  " 2:fs:3 2:fs:4 2:fs:5"
      }}

      /* unlinked many intermediate files */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
          " MOD/args.c"
          " MOD/args.h"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                search_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
            }}
          , (struct test_rule[]) {{
                search_pattern : (char[]) { "**/*.o\0" }
              , generate_pattern : (char[]) { "exe\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
              , relation : EDGE_DEPENDS
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
                  " 1:fs:2"
                  " 2:fs:3"
      }}
    , 0
  }
};
