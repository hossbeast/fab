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
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "logging.h"
#include "rule.internal.h"
#include "lookup.internal.h"
#include "node.h"
#include "shadow.h"
#include "node_operations.h"
#include "pattern.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "variant.h"
#include "node_operations_test.h"
#include "filesystem.h"

typedef struct rules_test {
  XUNITTEST;

  // inputs
  char * operations;          // create the starting graph
  bool shadow;
  char * module_id;
  char * module;              // context node for applying rules
  char ** variants;
  char * match_pattern;
  char * generate_pattern;
  rule_direction dir;
  rule_cardinality card;
  edge_type relation;

  // outputs
  char * graph;               // expectant result graph
} rules_test;

static xapi rules_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(module_setup);
  fatal(filesystem_setup);
  fatal(graph_setup);
  fatal(node_setup);
  fatal(variant_setup);

  finally : coda;
}

static xapi rules_test_unit_cleanup(xunit_unit * unit)
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

static xapi rules_test_entry(xunit_test * _test)
{
  enter;

  rules_test * test = (rules_test *)_test;

  narrator * N = 0;
  pattern_parser * parser = 0;
  operations_parser * op_parser = 0;
  list * operations = 0;
  yyu_location loc;
  module mod = { 0 };
  set * variants = 0;
  variant *v = 0;
  rule * r;
  int x;
  llist modules;
  pattern * match_pat = 0;
  pattern * generate_pat = 0;
  pattern * fml_pat = 0;
  rule_run_context rule_ctx = { 0 };
  rule_module_association rma;

  fatal(narrator_growing_create, &N);
  fatal(pattern_parser_create, &parser);
  fatal(operations_parser_create, &op_parser);
  fatal(rule_run_context_xinit, &rule_ctx);
  rule_module_association_init(&rma);

  if(test->module_id)
    snprintf(mod.id, sizeof(mod.id), "%s", test->module_id);

  fatal(rule_run_context_begin, &rule_ctx);

  // setup initial graph
  if(test->shadow)
  {
    fatal(shadow_setup);
    fatal(shadow_graft_module, &mod, &rule_ctx.invalidation);
  }

  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_parse, op_parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  // parse the patterns for the rule
  if(test->match_pattern)
  {
    fatal(match_pattern_parse_partial, parser, test->match_pattern, strlen(test->match_pattern) + 2, "-test-", 0, &loc, &match_pat);
    assert_eq_u32(strlen(test->match_pattern), loc.l);
  }

  if(test->generate_pattern)
  {
    fatal(generate_pattern_parse_partial, parser, test->generate_pattern, strlen(test->generate_pattern) + 2, "-test-", 0, &loc, &generate_pat);
    assert_eq_u32(strlen(test->generate_pattern), loc.l);
  }

  fatal(rule_mk, &r, g_graph, match_pat, generate_pat, fml_pat, 0);
  match_pat = 0;
  generate_pat = 0;
  fml_pat = 0;

  r->dir = test->dir;
  r->card = test->card;
  r->relation = test->relation;

  fatal(set_create, &variants);
  if(test->variants)
  {
    for(x = 0; x < sentinel(test->variants); x++)
    {
      fatal(variant_get, test->variants[x], strlen(test->variants[x]), &v);
      fatal(set_put, variants, v, 0);
    }
  }

  llist_init_node(&modules);

  // setup context
  fatal(pattern_lookup_fragment, MMS(test->module), 0, 0, 0, 0, 0, 0, &mod.dir_node);
  mod.shadow_scope = mod.dir_node;

  // act
  rule_ctx.mod = &mod;
  rule_ctx.modules = &modules;
  rule_ctx.variants = variants;
  rule_ctx.rma = &rma;
  fatal(rule_run, r, &rule_ctx);

  // ordered list of edges
  fatal(graph_say, g_graph, N);
  const char * graph = narrator_growing_buffer(N);
  assert_eq_s(test->graph, graph);

finally:
  pattern_free(match_pat);
  pattern_free(generate_pat);
  pattern_free(fml_pat);
  fatal(node_cleanup);
  fatal(set_xfree, variants);
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
        , match_pattern : (char[]) { "a/b\0" }
        , generate_pattern : (char[]) { "$^D/c.o\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_TYPE_STRONG
        , graph :  "1-(root)!dir 2-MOD!dir 3-a!dir"
                  " 4-b!file 5-c.c!file 6-c.o!U|file"
                  " 7-!rule"
                  " 7:rule-dir:2 7:rule-dir:3"             // rule edges
                  " 6:strong:4"                            // strong edges
                  " 1:fs:2 2:fs:3 3:fs:4 3:fs:5 3:fs:6"    // fs edges
      }}
    , (rules_test[]) {{
          operations : ""
          " MOD/a/b.foo"
          " MOD/a/b.bar"
        , module : "MOD"
        , variants : (char*[]) { "foo", "bar", 0 }
// a/b.? -> $^D/c.$?
        , match_pattern : (char[]) { "a/b.?\0" }
        , generate_pattern : (char[]) { "$^D/c.$?\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_TYPE_STRONG
        , graph :  "1-(root)!dir 2-MOD!dir 3-a!dir"
                  " 4-b.bar!file 5-b.foo!file 6-c.bar!U|file 7-c.foo!U|file"
                  " 8-!rule"
                  " 8:rule-dir:2 8:rule-dir:3"
                  " 6:strong:4 7:strong:5"
                  " 1:fs:2 2:fs:3 3:fs:4 3:fs:5 3:fs:6 3:fs:7"
      }}
    , (rules_test[]) {{
          operations : ""
          " MOD/x.c"
        , module : "MOD"
        , variants : (char*[]) { "foo", "bar", 0 }
// (*).c -> $1.?.o
        , match_pattern : (char[]) { "(*).c\0" }
        , generate_pattern : (char[]) { "$1.?.o\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_TYPE_STRONG
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-x.bar.o!U|file 4-x.c!file 5-x.foo.o!U|file"
                  " 6-!rule"
                  " 6:rule-dir:2"
                  " 3:strong:4 5:strong:4"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5"
      }}
    /* shadow */
    , (rules_test[]) {{
          operations : ""
          " MOD/x.c"
        , shadow : true
        , module : "MOD"
        , module_id : "beef"
// //module/tests/* -> //shadow/tests
        , match_pattern : (char[]) { "x.c\0" }
        , generate_pattern : (char[]) { "//tests\0" }
        , dir : RULE_LTR
        , card : RULE_ONE_TO_ONE
        , relation : EDGE_TYPE_STRONG
        , graph :  "1-modules!shadow-modules"
                  " 2-module!shadow-module"
                  " 3-(shadow)!shadow-dir 4-beef!shadow-dir 5-imports!shadow-dir 6-scope!shadow-dir 7-targets!shadow-file"
                  " 8-tests!U|shadow-file"
                  " 9-fs!shadow-link"
                  " 10-(root)!dir 11-MOD!dir"
                  " 12-x.c!file"
                  " 13-!rule"
                  " 13:rule-dir:11"
                  " 8:strong:12"
                  " 1:fs:4 3:fs:1 3:fs:2 3:fs:8 4:fs:5 4:fs:6 4:fs:7 4:fs:9 10:fs:11 11:fs:12"
      }}
    /* zero-to-many */
    , (rules_test[]) {{
          operations : ""
          " MOD"
        , module : "MOD"
// a/b -> $^D/c.o
        , generate_pattern : (char[]) { "vcs.{c,h}\0" }
        , card : RULE_ZERO_TO_MANY
        , relation : EDGE_TYPE_STRONG
        , graph :  "1-(root)!dir 2-MOD!dir"
                  " 3-vcs.c!U|file 4-vcs.h!U|file"
                  " 5-!rule"
                  " 1:fs:2 2:fs:3 2:fs:4"                  // fs edges
                  " :strong:3,4"                           // strong edges (hyper)
      }}
    , 0
  }
};
