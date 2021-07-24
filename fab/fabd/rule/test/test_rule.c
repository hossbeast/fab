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

#include "valyria/load.h"
#include "moria/load.h"
#include "xlinux/xstdlib.h"
#include "value/load.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "rule.h"
#include "lookup.h"
#include "fsent.h"
#include "shadow.h"
#include "pattern.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "variant.h"
#include "node_operations_test.h"
#include "filesystem.h"
#include "rule_module.h"
#include "selection.h"
#include "rule_system.h"
#include "rule_module.h"
#include "fsedge.h"
#include "dependency.h"
#include "channel.h"

typedef struct rules_test {
  XUNITTEST;

  // inputs
  char * operations;          // create the starting graph
  bool shadow;
  char * module_id;
  char * module;              // context node for applying rules
  char ** variants;
  char * search_pattern;
  char * generate_pattern;
  rule_direction dir;
  rule_cardinality card;
  edge_type relation;

  // outputs
  char * graph;               // expectant result graph
} rules_test;

static void rules_test_unit_setup(xunit_unit * unit)
{
  valyria_load();
  moria_load();
  value_load();
  logging_finalize();

  module_setup();
  filesystem_setup();
  graph_setup();
  fsent_setup();
  variant_setup();
  shadow_setup();
  rule_system_setup();
}

static void rules_test_unit_cleanup(xunit_unit * unit)
{
  graph_cleanup();
  variant_cleanup();
  module_cleanup();
  filesystem_cleanup();

  valyria_unload();
  moria_unload();
  value_unload();
}

//
// tests
//

static void rules_test_entry(xunit_test * _test)
{
  rules_test * test = (rules_test *)_test;

  narrator_growing * N = 0;
  pattern_parser * parser = 0;
  graph_parser * op_parser = 0;
  yyu_location loc;
  module mod = { };
  set * variants = 0;
  variant *v = 0;
  rule * r;
  int x;
  llist modules;
  pattern * search_pat = 0;
  pattern * generate_pat = 0;
  rule_run_context rule_ctx = { 0 };
  rule_module_edge *rme;
  const char *graph;
  llist *vertex_lists[2];
  llist *edge_lists[3];
  channel *chan = 0;

  narrator_growing_create(&N);
  pattern_parser_create(&parser);
  rule_run_context_xinit(&rule_ctx);
  rule_module_edge_alloc(&rme);
  xmalloc(&chan, sizeof(*chan));

  // setup the initial graph
  graph_parser_create(&op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  graph_parser_operations_parse(op_parser, MMS(test->operations));

  // setup the module
  snprintf(mod.id, sizeof(mod.id), "%s", test->module_id);
  resolve_fragment(MMS(test->module), &mod.dir_node);
  shadow_module_init(&mod, &rule_ctx.invalidation);

  // parse the patterns for the rule
  if(test->search_pattern)
  {
    search_pattern_parse_partial(parser, test->search_pattern, strlen(test->search_pattern) + 2, "-test-", 0, &loc, &search_pat);
    assert_eq_u32(strlen(test->search_pattern), loc.l);
  }

  if(test->generate_pattern)
  {
    generate_pattern_parse_partial(parser, test->generate_pattern, strlen(test->generate_pattern) + 2, "-test-", 0, &loc, &generate_pat);
    assert_eq_u32(strlen(test->generate_pattern), loc.l);
  }

  rule_mk(&r, &g_graph, search_pat, generate_pat, 0, 0, 0);
  search_pat = 0;
  generate_pat = 0;

  r->dir = test->dir;
  r->card = test->card;
  r->relation = test->relation;

  set_create(&variants);
  if(test->variants)
  {
    for(x = 0; x < sentinel(test->variants); x++)
    {
      variant_get(test->variants[x], strlen(test->variants[x]), &v);
      set_put(variants, v, 0);
    }
  }

  // setup context
  llist_init_node(&modules);
  rule_ctx.mod = &mod;
  rule_ctx.modules = &modules;
  rule_ctx.variants = variants;
  rule_ctx.rme = rme;
  rme->rule = r;
  rule_ctx.chan = chan;

  // act
  rule_run(r, &rule_ctx);
  assert_eq_b(false, chan->error);

  vertex_lists[0] = &fsent_list;
  vertex_lists[1] = &rule_list;
  edge_lists[0] = &fsedge_list;
  edge_lists[1] = &dependency_list;
  edge_lists[2] = &rde_list;

  graph_say_lists(&N->base, vertex_lists, sizeof(vertex_lists) / sizeof(*vertex_lists), edge_lists, sizeof(edge_lists) / sizeof(*edge_lists));
  graph = N->s;
  assert_eq_s(test->graph, graph);

finally:
  pattern_free(search_pat);
  pattern_free(generate_pat);
  fsent_cleanup();
  set_xfree(variants);
  narrator_growing_free(N);
  graph_parser_xfree(op_parser);
  pattern_parser_xfree(parser);
  rule_run_context_xdestroy(&rule_ctx);
  wfree(chan);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : rules_test_unit_setup
  , xu_cleanup : rules_test_unit_cleanup
  , xu_entry : rules_test_entry
  , xu_tests : (rules_test*[]) {
      (rules_test[]) {{
          operations : ""
          " MOD/a/b"
          " MOD/a/c.c"
        , module : "MOD"
        , variants : (char*[]) { "foo", 0 }
        // a/b -> $^D/c.o
        , search_pattern : (char[]) { "a/b\0" }
        , generate_pattern : (char[]) { "$^D/c.o\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_DEPENDS
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module 4-(null)!shadow-dir 5-(shadow)!shadow-dir 6-imports!shadow-dir 7-requires!shadow-dir 8-uses!shadow-dir 9-targets!shadow-file"
                  " 10-(root)!dir 11-MOD!dir 12-a!dir"
                  " 13-b!file 14-c.c!file 15-c.o!U|file"
                  " 16-!rule"
                  " 16:rule-dir:11 16:rule-dir:12"
                  " 15:depends:13"
                  " 2:fs:4 4:fs:1 4:fs:6 4:fs:7 4:fs:8 4:fs:9 5:fs:2 5:fs:3"
                  " 10:fs:11 11:fs:12 12:fs:13 12:fs:14 12:fs:15"
      }}
    , (rules_test[]) {{
          operations : ""
          " MOD/a/b.foo"
          " MOD/a/b.bar"
        , module : "MOD"
        , variants : (char*[]) { "foo", "bar", 0 }
        // a/b.? -> $^D/c.$?
        , search_pattern : (char[]) { "a/b.?\0" }
        , generate_pattern : (char[]) { "$^D/c.$?\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_DEPENDS
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module 4-(null)!shadow-dir 5-(shadow)!shadow-dir 6-imports!shadow-dir 7-requires!shadow-dir 8-uses!shadow-dir 9-targets!shadow-file"
                  " 10-(root)!dir 11-MOD!dir 12-a!dir"
                  " 13-b.bar!file 14-b.foo!file 15-c.bar!U|file 16-c.foo!U|file"
                  " 17-!rule"
                  " 17:rule-dir:11 17:rule-dir:12"
                  " 15:depends:13 16:depends:14"
                  " 2:fs:4 4:fs:1 4:fs:6 4:fs:7 4:fs:8 4:fs:9 5:fs:2 5:fs:3"
                  " 10:fs:11 11:fs:12 12:fs:13 12:fs:14 12:fs:15 12:fs:16"
      }}
    , (rules_test[]) {{
          operations : ""
          " MOD/x.c"
        , module : "MOD"
        , variants : (char*[]) { "foo", "bar", 0 }
        // (*).c -> $1.?.o
        , search_pattern : (char[]) { "(*).c\0" }
        , generate_pattern : (char[]) { "$1.?.o\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_DEPENDS
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module 4-(null)!shadow-dir 5-(shadow)!shadow-dir 6-imports!shadow-dir 7-requires!shadow-dir 8-uses!shadow-dir 9-targets!shadow-file"
                  " 10-(root)!dir 11-MOD!dir"
                  " 12-x.bar.o!U|file 13-x.c!file 14-x.foo.o!U|file"
                  " 15-!rule 15:rule-dir:11"
                  " 12:depends:13 14:depends:13"
                  " 2:fs:4 4:fs:1 4:fs:6 4:fs:7 4:fs:8 4:fs:9 5:fs:2 5:fs:3"
                  " 10:fs:11 11:fs:12 11:fs:13 11:fs:14"
      }}
    /* shadow */
    , (rules_test[]) {{
          operations : ""
          " MOD/x.c"
        , shadow : true
        , module : "MOD"
        , module_id : "beef"
        // x.c -> //tests
        , search_pattern : (char[]) { "x.c\0" }
        , generate_pattern : (char[]) { "//tests\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_DEPENDS
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module 4-(shadow)!shadow-dir 5-beef!shadow-dir 6-imports!shadow-dir 7-requires!shadow-dir 8-uses!shadow-dir 9-targets!shadow-file 10-tests!shadow-file"
                  " 11-(root)!dir 12-MOD!dir"
                  " 13-x.c!file"
                  " 14-!rule"
                  " 14:rule-dir:12"
                  " 10:depends:13"
                  " 2:fs:5 4:fs:2 4:fs:3 4:fs:10 5:fs:1 5:fs:6 5:fs:7 5:fs:8 5:fs:9"
                  " 11:fs:12 12:fs:13"
      }}
    , 0
  }
};
