/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/load.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria/graph.h"
#include "moria/parser.h"
#include "bacon/load.h"

#include "xunit.h"
#include "lookup.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "rule.h"
#include "fsent.h"
#include "pattern.h"
#include "fstree.h"
#include "pattern_parser.h"
#include "module.h"
#include "node_operations_test.h"
#include "rule_module_association.h"
#include "rule_system.h"
#include "ruleset_parser.h"
#include "shadow.h"
#include "system.h"
#include "buildplan_entity.h"
#include "vertex.h"
#include "entlist.h"
#include "fsedge.h"
#include "dependency.h"
#include "dir_rma_edge.h"

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
  } **rules;

  char * operations;        // graph mutations to perform

  // outputs
  char * graph;             // expected graph at the end
} rule_system_reconcile_test;

static void rule_system_reconcile_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();

  graph_setup();
  fsent_root_setup();
  shadow_setup();
  fstree_setup();
  module_setup();
  rule_system_setup();
  buildplan_entity_setup();

  /* suppress rcu-registration checks */
  extern __thread bool rcu_is_registered ; rcu_is_registered = true;
}

static void rule_system_reconcile_test_unit_cleanup(xunit_unit * unit)
{
  graph_cleanup();
  module_cleanup();
  fstree_cleanup();
  buildplan_entity_cleanup();
  entlist_cleanup();
  shadow_cleanup();

  moria_unload();
}

//
// tests
//

static void rule_system_reconcile_test_entry(xunit_test * _test)
{
  rule_system_reconcile_test * test = (rule_system_reconcile_test *)_test;

  narrator_growing * N = 0;
  pattern_parser * parser = 0;
  moria_parser * op_parser = 0;
  ruleset_parser *mod_parser = 0;

  yyu_location loc;
  module *mod;
  rule * rule;
  int x;
  pattern * search_pat = 0;
  pattern * generate_pat = 0;
  rule_run_context rule_ctx = { };
  const char * graph;
  rule_module_association rmas[8];
  struct rule_bacon rb = { };

  rule_run_context_init(&rule_ctx);
  pattern_parser_create(&parser);
  ruleset_parser_alloc(&mod_parser);

  narrator_growing_create(&N);
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);

  /* setup - build the initial graph */
  moria_parser_operations_parse(op_parser, MMS(test->setup_operations));

  module_alloc(&mod);
  resolve_fragment(MMS(test->module), &mod->dir_node);
  mod->anchor_node = mod->dir_node;

  /* setup - run each rule */
  for(x = 0; x < sentinel(test->rules); x++)
  {
    typeof(*test->rules) test_rule = test->rules[x];

    // parse the patterns for the rule
    search_pattern_parse_partial(parser, test_rule->search_pattern, strlen(test_rule->search_pattern) + 2, "-test-", 0, &loc, &search_pat);
    assert_eq_u32(strlen(test_rule->search_pattern), loc.l);

    generate_pattern_parse_partial(parser, test_rule->generate_pattern, strlen(test_rule->generate_pattern) + 2, "-test-", 0, &loc, &generate_pat);
    assert_eq_u32(strlen(test_rule->generate_pattern), loc.l);

    rule = rule_mk(&g_graph, 0, 0, search_pat, generate_pat, 0, 0, 0, 0, &rb, 0, &loc);
    search_pat = 0;
    generate_pat = 0;

    rule->dir = test_rule->dir;
    rule->card = test_rule->card;

    /* when the generate pattern changes the set of child nodes in the directory
     * which the match pattern visited, the rule is enqueued to be run again */
    rule_module_association_init(&rmas[x]);
    rmas[x].mod = mod;
    rmas[x].rule = rule;
    rule_ctx.rma = &rmas[x];

    rule_run(rule, &rule_ctx);
  }

  /* setup - perform specified mutations if any */
  moria_parser_operations_parse(op_parser, MMS(test->operations));

  /* act - reconcile rules - run enqueued rules to quiescence */
  rule_system_reconcile(&rule_ctx);
  assert_eq_b(false, system_error);

  // assert
  graph_say(&N->base);
  graph = N->s;
  assert_eq_s(test->graph, graph);

  ruleset_parser_free(mod_parser);
  pattern_free(search_pat);
  pattern_free(generate_pat);
  fsent_cleanup();
  narrator_growing_free(N);
  moria_parser_free(op_parser);
  pattern_parser_free(parser);
  rule_run_context_destroy(&rule_ctx);
}

//
// manifest
//

static xunit_unit xunit = {
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
              // rule main.c -> $^D/main.o
                search_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , 0
        }
        , operations : ""
        , graph :  "1-(root)!dir|protect 2-MOD!dir 3-main.c!file 4-main.o!U|file 5-!bpe"
                  " 5:bpe-tgt:4 3:src-bpe:5"
                  " 2:fs:1 3:fs:2 4:fs:2"
      }}

      /* unlinked primary file - deleted, along with the secondary file */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // rule main.c -> $^D/main.o
                search_pattern : (char[]) { "main.c\0" }
              , generate_pattern : (char[]) { "$^D/main.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , 0
        }
        , operations : ""
                  " =main.c:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 2:fs:1"

/*
 * The rule is run again, doesn't match, and the rma-bpe edge is deleted, which cascades
 * to the edges created by the bpe
 */
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
            }}
          , 0
        }
        , operations : ""
                  " =main.o:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-main.c!file 4-main.o!U|file"
                  " 5-!bpe"
                  " 5:bpe-tgt:4 3:src-bpe:5"
                  " 2:fs:1 3:fs:2 4:fs:2"
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
            }}
          , 0
        }
        , operations : ""
                  " =main.c:fs:MOD"
                  " =main.o:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 2:fs:1"
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
            }}
          , 0
        }
        , operations : ""
                  /* This refresh operation promotes these secondary files from UNCREATED -> OK
                   * which is what happens when a formula is executed that generates them
                   * hence they are not subsequently deleted when the rule that originally created
                   * them no longer matches. */
                  " @args.o @main.o"
                  /* Now the primary files which the secondary files have dependencies on are unlinked.
                   * which causes them to become invalidated (I) */
                  " =main.c:fs:src"
                  " =args.c:fs:sub"
                  " =args.h:fs:sub"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-src!dir 4-sub!dir"
                  " 5-args.o!I|file 6-main.o!I|file"
                  " 2:fs:1 3:fs:2 4:fs:3 6:fs:3 5:fs:4"
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
            }}
          , 0
        }
        , operations : ""
                  " =main.o:fs:MOD/src"
                  " =main.c:fs:MOD/src"
                  " =args.c:fs:MOD/src/sub"
                  " =args.h:fs:MOD/src/sub"
                  " =args.o:fs:MOD/src/sub"
                  " =sub:fs:MOD/src"
                  " =src:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 2:fs:1"
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
            }}
          , 0
        }
        , operations : ""
                  " MOD/src/sub/args.c"
        , graph :  "1-(root)!dir|protect 2-MOD!dir 3-src!dir 4-sub!dir"
                  " 5-args.c!file 6-args.o!U|file 7-main.c!file 8-main.o!U|file"
                  " 9-!bpe 10-!bpe"
                  " 10:bpe-tgt:6 9:bpe-tgt:8 7:src-bpe:9 5:src-bpe:10"
                  " 2:fs:1"
                  " 3:fs:2 4:fs:3 7:fs:3 8:fs:3 5:fs:4 6:fs:4"
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
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
              }}
          , 0
        }
        , operations : ""
                  " MOD/args.c"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!U|file 6-main.c!file 7-main.o!U|file"
                  " 8-!bpe 9-!bpe 10-!bpe 11-!bpe"
                  " 10:bpe-tgt:4 9:bpe-tgt:5 11:bpe-tgt:5 8:bpe-tgt:7 6:src-bpe:8 7:src-bpe:9 3:src-bpe:10 4:src-bpe:11"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2 6:fs:2 7:fs:2"
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
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
              }}
          , 0
        }
        , operations : ""
                  /* exe and main.o are invalid(I) rather than uncreated (U) because the exists bit
                   * was set when those nodes were refreshed */
                  " @exe @main.o"
                  /* This adds another file which is matched by the first rule, which creates a file
                   * that is matched by the second rule. These attachments invalidate the nodes */
                  " MOD/args.c"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!I|file 6-main.c!file 7-main.o!I|file"
                  " 8-!bpe 9-!bpe 10-!bpe 11-!bpe"
                  " 10:bpe-tgt:4 9:bpe-tgt:5 11:bpe-tgt:5 8:bpe-tgt:7 6:src-bpe:8 7:src-bpe:9 3:src-bpe:10 4:src-bpe:11"
                  " 2:fs:1"
                  " 3:fs:2 4:fs:2 5:fs:2 6:fs:2 7:fs:2"
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
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
              }}
          , 0
        }
        , operations : ""
                  " =args.c:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-args.o!U|file 4-exe!U|file 5-main.c!file 6-main.o!U|file"
                  " 7-!bpe 8-!bpe 9-!bpe"
                  " 8:bpe-tgt:4 9:bpe-tgt:4 7:bpe-tgt:6 5:src-bpe:7 3:src-bpe:8 6:src-bpe:9"
                  " 2:fs:1"
                  " 3:fs:2 4:fs:2 5:fs:2 6:fs:2"
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
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
              }}
          , 0
        }
        , operations : ""
                  " =exe:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-args.c!file 4-args.o!U|file 5-exe!U|file 6-main.c!file 7-main.o!U|file"
                  " 8-!bpe 9-!bpe 10-!bpe 11-!bpe"
                  " 9:bpe-tgt:4 10:bpe-tgt:5 11:bpe-tgt:5 8:bpe-tgt:7 6:src-bpe:8 3:src-bpe:9 4:src-bpe:10 7:src-bpe:11"
                  " 2:fs:1"
                  " 3:fs:2 4:fs:2 5:fs:2 6:fs:2 7:fs:2"

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
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
              }}
          , 0
        }
        , operations : ""
                  " @main.o"
                  " =main.o:fs:MOD"
        /* main.o is uncreated(U) because exe still depends on it */
        /* exe is uncreated(U) because it was never refreshed */
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-exe!U|file 4-main.c!file 5-main.o!U|file"
                  " 6-!bpe 7-!bpe"
                  " 7:bpe-tgt:3 6:bpe-tgt:5 4:src-bpe:6 5:src-bpe:7"
                  " 2:fs:1"
                  " 3:fs:2 4:fs:2 5:fs:2"
      }}

      /* intermediate file refreshed, then unlinked */
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
              }}
            , (struct test_rule[]) {{
                  search_pattern : (char[]) { "**/*.o\0" }
                , generate_pattern : (char[]) { "exe\0" }
                , dir : RULE_LTR
                , card : RULE_ONE_TO_ONE
              }}
          , 0
        }
        , operations : ""
                  " @exe @main.o"
                  " =main.o:fs:MOD"
        /* main.o is uncreated(U) instead of deleted because exe still depends on it */
        /* exe was marked invalid(I) - transitively - when main.c was unlinked */
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-exe!I|file 4-main.c!file 5-main.o!U|file"
                  " 6-!bpe 7-!bpe"
                  " 7:bpe-tgt:3 6:bpe-tgt:5 4:src-bpe:6 5:src-bpe:7"
                  " 2:fs:1"
                  " 3:fs:2 4:fs:2 5:fs:2"
      }}

      /* intermediate file refreshed, then primary file unlinked */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                search_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , (struct test_rule[]) {{
                search_pattern : (char[]) { "**/*.o\0" }
              , generate_pattern : (char[]) { "exe\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , 0
        }

        , operations : ""
                  " @exe @main.o"               // VERTEX_OK
                  " =main.c:fs:MOD"
        /* main.o is marked invalid(I) when main.c was unlinked */
        /* exe was marked invalid(I) - transitively - when main.c was unlinked */
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-exe!I|file 4-main.o!I|file"
                  " 5-!bpe"
                  " 5:bpe-tgt:3 4:src-bpe:5"
                  " 2:fs:1 3:fs:2 4:fs:2"
      }}

      /* intermediate file refreshed, then primary and secondary file unlinked */
    , (rule_system_reconcile_test[]) {{
          setup_operations : ""
          " MOD/main.c"
        , module : "MOD"
        , rules : (struct test_rule*[]) {
            (struct test_rule[]) {{
              // a/b -> $^D/c.o
                search_pattern : (char[]) { "**/(*).c\0" }
              , generate_pattern : (char[]) { "$^D/$1.o\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , (struct test_rule[]) {{
                search_pattern : (char[]) { "**/*.o\0" }
              , generate_pattern : (char[]) { "exe\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , 0
        }
        , operations : ""
                  " @exe @main.o"               // VERTEX_OK
                  " =main.c:fs:MOD"
                  " =main.o:fs:MOD"
        /* exe was marked invalid(I) - transitively - when main.c was unlinked */
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-exe!I|file"
                  " 2:fs:1 3:fs:2"
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
            }}
          , (struct test_rule[]) {{
                search_pattern : (char[]) { "**/*.o\0" }
              , generate_pattern : (char[]) { "exe\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , 0
        }
        , operations : ""
                  " @exe @main.o @args.o"     // VERTEX_OK
                  " =main.c:fs:MOD"
        /* main.o is marked invalid(I) when main.c is unlinked */
        /* exe is marked invalid(I) when the main.o edge is removed */
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-args.c!file 4-args.h!file 5-args.o!file 6-exe!I|file 7-main.o!I|file"
                  " 8-!bpe 9-!bpe 10-!bpe"
                  " 8:bpe-tgt:5 9:bpe-tgt:6 10:bpe-tgt:6 3:src-bpe:8 5:src-bpe:9 7:src-bpe:10"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2 6:fs:2 7:fs:2"
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
            }}
          , (struct test_rule[]) {{
                search_pattern : (char[]) { "**/*.o\0" }
              , generate_pattern : (char[]) { "exe\0" }
              , dir : RULE_LTR
              , card : RULE_ONE_TO_ONE
            }}
          , 0
        }
        , operations : ""
                  " @exe @main.o @args.o"     // VERTEX_OK
                  " =main.c:fs:MOD"
                  " =main.o:fs:MOD"
                  " =args.o:fs:MOD"
                  " =args.c:fs:MOD"
                  " =args.h:fs:MOD"
        , graph :  "1-(root)!dir|protect 2-MOD!dir"
                  " 3-exe!I|file"
                  " 2:fs:1"
                  " 3:fs:2"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
