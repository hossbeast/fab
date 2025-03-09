/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/builder.h"
#include "bacon/load.h"
#include "bacon/parser.h"
#include "moria/graph.h"
#include "moria/load.h"
#include "moria/parser.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "yyutil/parser.h"

#include "narrator.h"
#include "narrator/growing.h"
#include "rule.h"
#include "fsent.h"
#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern.h"
#include "shadow.h"
#include "node_operations_test.h"
#include "fstree.h"
#include "module.h"
#include "selection.h"
#include "args.h"
#include "zcat.h"
#include "namespace.h"
#include "fsname.h"
#include "events/events.h"
#include "notify.h"
#include "configset_parser.h"
#include "configset.h"
#include "config.h"
#include "config_system.h"
#include "ns_edge.h"
#include "ref_edge.h"
#include "fsedge.h"

typedef struct node_lookup_test
{
  XUNITTEST;

  char *config;       // filesystems config
  char *operations;   // create the starting graph
  char *vars;

  // context
  char *pattern;      // lookup pattern

  // expected
  char *expected;
} node_lookup_test;

static void node_lookup_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();

  g_args.default_filesystem_index = true;

  args_init();
  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();

  fsent_base_setup();
  module_setup();
  namespace_setup();
  graph_setup();
  namespace_setup();
  notify_thread_setup();
  notify_setup();
  config_system_setup();
  events_setup();

  /* suppress rcu-registration checks */
  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void node_lookup_test_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
  bacon_unload();

  fstree_cleanup();
  fsname_cleanup();
  namespace_cleanup();
  shadow_cleanup();
  fsent_cleanup();
  ns_edge_cleanup();
  ref_edge_cleanup();
  fsedge_cleanup();
  graph_cleanup();
}

//
// tests
//

static void node_lookup_test_entry(xunit_test * _test)
{
  node_lookup_test * test = containerof(_test, node_lookup_test, xu);
  pattern_parser * parser = 0;
  yyu_location loc;
  lookup_pattern *ref = 0;
  configset_parser *cfg_parser = 0;
  moria_parser *op_parser = 0;
  bacon_parser *val_parser = 0;
  bacon_value *vars = 0;
  char actual[256];
  size_t z;
  graph_invalidation_context invalidation = { };
  config *cfg;
  struct lookup_context ctx;
  int r;
  int x;
  ctx = (typeof(ctx)) { };

  pattern_parser_create(&parser);
  bacon_parser_alloc(&val_parser);
  graph_invalidation_begin(&invalidation);
  namespace_modules = namespace_get(MMS("modules"), &invalidation);
  graph_invalidation_end(&invalidation);

  // setup filesystems
  configset_parser_create(&cfg_parser);
  config_alloc(&cfg);
  if(test->config) {
    if(configset_parser_parse(cfg_parser, test->config, strlen(test->config) + 2, 0, 0, cfg) != 0) {
      tracef("%.*s", cfg_parser->yyu.error_len, cfg_parser->yyu.error_str);
      RTA;
    }
  }

  graph_invalidation_begin(&invalidation);
  namespace_reconfigure(cfg, false, &invalidation);
  fstree_reconfigure(cfg, false);
  namespace_reconfigure(cfg, true, &invalidation);
  fstree_reconfigure(cfg, true);
  graph_invalidation_end(&invalidation);

  // setup initial graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  if(test->operations) {
    if(moria_parser_operations_parse(op_parser, MMS(test->operations)) != 0) {
      RTA;
    }
  }

  /* index fsents */
  graph_invalidation_begin(&invalidation);
  fsent_fstree_refresh(&invalidation);
  namespace_reconcile(&invalidation);
  graph_invalidation_end(&invalidation);

  if(lookup_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &ref) != 0) {
    tracef("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
    RTA;
  }
  assert_eq_u32(strlen(test->pattern), loc.l);

  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }

  // act
  ctx = (typeof(ctx)) {
      .ns = namespace_modules
    , .ns_name = namespace_modules->name
    , .ns_name_len = namespace_modules->name_len
    , .rctx.vars = vars
  };
  r = namespace_resolve(&ctx, ref);

  // assert
  z = 0;
  for(x = 0; x < r; x++) {
    if(z) {
      z += zcats(actual + z, sizeof(actual) - z, " ");
    }
    z += fsent_absolute_path_cat(actual + z, sizeof(actual) - z, containerof(ctx.rctx.vertices[x], fsent, vertex));
  }

  if(ctx.miss) {
    z += zcats(actual + z, sizeof(actual) - z, ":");
    z += zcatw(actual + z, sizeof(actual) - z, ctx.miss->name, ctx.miss->name_len);
  }
  actual[z] = 0;

  assert_eq_s(test->expected, actual);

  bacon_value_free(val_parser, vars);
  bacon_parser_free(val_parser);
  pattern_parser_free(parser);
  pattern_free(&ref->pattern);
  moria_parser_free(op_parser);
  config_free(cfg, cfg_parser);
  configset_parser_free(cfg_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : node_lookup_test_unit_setup
  , xu_cleanup : node_lookup_test_unit_cleanup
  , xu_entry : node_lookup_test_entry
  , xu_tests : (node_lookup_test*[]) {
    /* basic */
      (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/b/c/d"
        , pattern : (char[]) { "c\0" }
        , expected : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/b/c/d"
        , pattern : (char[]) { "b/c\0" }
        , expected : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/b/c/d"
        , pattern : (char[]) { "\"\\x63\"\0" }
        , expected : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "b\"o\"b/c\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/.D/c"
        , pattern : (char[]) { "a/.D\0" }
        , expected : "/a/.D"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/b.D/c/d"
        , pattern : (char[]) { "b.D/c\0" }
        , expected : "/a/b.D/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c\0" }
        , expected : "/a/bob/c"
      }}

    /* type-file qualifiers in last section */
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)\0" }
        , expected : "/a/bob/c/d"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        ,  operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)+(.)\0" }
        , expected : "/a/bob/c/d"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        ,  operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d~(.)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)~(.)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)~(/)\0" }
        , expected : "/a/bob/c/d"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(/)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c+(.)\0" }
        , expected : ""
      }}

    /* type-dir qualifiers in last section */
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c+(/)\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c~(/)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c~(.)\0" }
        , expected : "/a/bob/c"
      }}

    /* variables */
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , vars : "dir : a"
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "$dir/bob/c\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , vars : "dir : bob"
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "a/$dir/c\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , vars : "dir : c"
        , operations : "a/bob/c/d"
        , pattern : (char[]) { "a/bob/$dir\0" }
        , expected : "/a/bob/c"
      }}
      /* symlinks */
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations :
           " B/b/c"
           " +B:ref-symlink:A"  /* A -> symlink -> B */
        , pattern : (char[]) { "B\0" }
        , expected : "/B"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations :
           " B/b/c"
           " +B:ref-symlink:A"  /* A -> symlink -> B */
        , pattern : (char[]) { "B/b\0" }
        , expected : "/B/b"
      }}
      /* lookup via A -> B symlink */
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations :
           " B/b/c"
           " +B:ref-symlink:A"  /* A -> symlink -> B */
        , pattern : (char[]) { "A\0" }
        , expected : "/A"
      }}
    , (node_lookup_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        modules\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations :
           " B/b/c"
           " +B:ref-symlink:A"  /* A -> symlink -> B */
        , pattern : (char[]) { "A/b\0" }
        , expected : "/B/b"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
