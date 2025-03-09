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
#include "ref_edge.h"
#include "fsedge.h"
#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern.h"
#include "shadow.h"
#include "node_operations_test.h"
#include "fstree.h"
#include "lookup.h"
#include "module.h"
#include "selection.h"
#include "args.h"
#include "zcat.h"
#include "namespace.h"
#include "fsname.h"
#include "events/events.h"
#include "fstree.h"
#include "notify.h"
#include "configset_parser.h"
#include "configset.h"
#include "config.h"
#include "config_system.h"

typedef struct pattern_resolve_test
{
  XUNITTEST;

  char *config;       // filesystems config
  char *operations;   // create the starting graph
  char *vars;
  bool shadow;

  // context
  char *pattern;      // lookup pattern
  char *namespace;    // lookup namespace
  char *base;         // name for base node
  bool nofollow;
  bool hidden;

  // expected
  int result;
  char *expected;
} pattern_resolve_test;

static void pattern_resolve_test_unit_setup(xunit_unit * unit)
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
  graph_setup();
  fsent_base_setup();

  notify_setup();
  config_system_setup();

  /* suppress rcu-registration checks */
  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void pattern_resolve_test_unit_cleanup(xunit_unit *unit)
{
  moria_unload();
  bacon_unload();

  fstree_cleanup();
  fsname_cleanup();
  fsent_cleanup();
  ref_edge_cleanup();
  fsedge_cleanup();
  namespace_cleanup();
  shadow_cleanup();
  events_cleanup();
  graph_cleanup();
  config_system_cleanup();
}

//
// tests
//

static void pattern_resolve_test_entry(xunit_test *_test)
{
  pattern_resolve_test *test = containerof(_test, pattern_resolve_test, xu);
  pattern_parser *parser = 0;
  yyu_location loc;
  lookup_pattern *pat = 0;
  configset_parser *cfg_parser = 0;
  moria_parser *op_parser = 0;
  bacon_parser *val_parser = 0;
  bacon_value *vars = 0;
  fsent *base;
  char actual[256];
  size_t z;
  graph_invalidation_context invalidation = { };
  config *cfg;
  struct resolve_context ctx;
  int r;
  struct module mod;
  struct fsent mod_shadow;

  memset(&mod, 0, sizeof(mod));
  mod.shadow = &mod_shadow;

  pattern_parser_create(&parser);
  bacon_parser_alloc(&val_parser);

  // setup filesystems
  configset_parser_create(&cfg_parser);
  config_alloc(&cfg);
  if(test->config) {
    if(configset_parser_parse(cfg_parser, test->config, strlen(test->config) + 2, 0, 0, cfg) != 0) {
      tracef("%.*s", cfg_parser->yyu.error_len, cfg_parser->yyu.error_str);
      RTA;
    }
  }

  fstree_reconfigure(cfg, true);
  fstree_reconfigure(cfg, false);

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
  graph_invalidation_end(&invalidation);

  base = g_root;
  if(test->base) {
    base = resolve_fragment(MMS(test->base));
  }
  if(lookup_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pat) != 0) {
    tracef("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
    RTA;
  }
  assert_eq_u32(strlen(test->pattern), loc.l);

  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }

  // act
  memset(&ctx, 0, sizeof(ctx));
  ctx.mod = &mod;
  ctx.vars = vars;
  ctx.base = base;
  ctx.nofollow = test->nofollow;
  ctx.hidden = test->hidden;

  r = pattern_resolve_lookup(&ctx, pat);
  if(test->expected) {
    assert_eq_d(1, r);
  } else {
    assert_eq_d(0, r);
  }

  // assert
  z = 0;
  if(r == 1) {
    z += fsent_absolute_path_cat(actual + z, sizeof(actual) - z, containerof(ctx.vertices[0], fsent, vertex));
  }
  actual[z] = 0;

  assert_eq_s(test->expected, actual);

  bacon_value_free(val_parser, vars);
  bacon_parser_free(val_parser);
  pattern_parser_free(parser);
  pattern_free(&pat->pattern);
  moria_parser_free(op_parser);
  config_free(cfg, cfg_parser);
  configset_parser_free(cfg_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : pattern_resolve_test_unit_setup
  , xu_cleanup : pattern_resolve_test_unit_cleanup
  , xu_entry : pattern_resolve_test_entry
  , xu_tests : (pattern_resolve_test*[]) {
    /* dot-relative, down */
      (pattern_resolve_test[]) {{
          operations : "a/bob/c/d/e"
        , base : "bob"
        , pattern : (char[]) { "./c/d\0" }
        , expected : "/a/bob/c/d"
      }}
    /* dot-relative, up */
    , (pattern_resolve_test[]) {{
          operations : "a/bob/c/d"
        , base : "c"
        , pattern : (char[]) { "../c\0" }
        , expected : "/a/bob/c"
      }}
    , (pattern_resolve_test[]) {{
          operations : "a/bob/c/d"
        , base : "c"
        , pattern : (char[]) { "..\0" }
        , expected : "/a/bob"
      }}
    , (pattern_resolve_test[]) {{
          operations : "a/bob/c/d"
        , base : "c"
        , pattern : (char[]) { "../..\0" }
        , expected : "/a"
      }}

    /* absolute, root */
    , (pattern_resolve_test[]) {{
          operations : "a"
        , pattern : (char[]) { "/\0" }
        , expected : "/"
      }}
    , (pattern_resolve_test[]) {{
          operations : "a/bob/c"
        , pattern : (char[]) { "/a/bob\0" }
        , expected : "/a/bob"
      }}

    /* absolute, shadow */
    , (pattern_resolve_test[]) {{
          operations : "a"
        , pattern : (char[]) { "//\0" }
        , expected : "//"
      }}
    , (pattern_resolve_test[]) {{
          operations : "a"
        , pattern : (char[]) { "//bm/namespaces\0" }
        , expected : "//bm/namespaces"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
