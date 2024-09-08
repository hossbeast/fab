/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/load.h"
#include "bacon/builder.h"
#include "bacon/parser.h"
#include "bacon/load.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "moria/graph.h"
#include "moria/parser.h"
#include "yyutil/parser.h"

#include "xunit.h"
#include "xunit/assert.h"
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
#include "lookup.h"
#include "module.h"
#include "selection.h"
#include "args.h"
#include "zcat.h"
#include "fsindex.h"
#include "fsname.h"
#include "events.h"
#include "fstree.h"
#include "notify.h"

typedef struct node_lookup_test
{
  XUNITTEST;

  char *operations;   // create the starting graph
  char *vars;
  bool shadow;

  // context
  char *pattern;      // lookup pattern
  char *base;         // name for base node

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
  shadow_setup();
  fstree_setup();
  notify_setup();

  graph_setup();
  fsindex_setup();
  events_setup();

  /* suppress rcu-registration checks */
  extern __thread bool rcu_is_registered ; rcu_is_registered = true;
}

static void node_lookup_test_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
  bacon_unload();

  fsent_cleanup();
  graph_cleanup();
  fsname_cleanup();
  fsindex_cleanup();
  fstree_cleanup();
}

//
// tests
//

static void node_lookup_test_entry(xunit_test * _test)
{
  node_lookup_test * test = containerof(_test, node_lookup_test, xu);
  pattern_parser * parser = 0;
  yyu_location loc;
  pattern * pat = 0;
  moria_parser * op_parser = 0;
  bacon_parser * val_parser = 0;
  entlist hits;
  moria_vertex *v;
  bacon_value *vars = 0;
  fsent *base;
  char actual[256];
  size_t z;
  entlist *eln;
  graph_invalidation_context invalidation = { };

  entlist_init(&hits);
  pattern_parser_create(&parser);
  bacon_parser_alloc(&val_parser);

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
    resolve_fragment(MMS(test->base), &base);
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
  pattern_lookup(pat, vars, base, &hits);

  // assert
  z = 0;
  entlist_foreach(&hits, v, eln) {
    if(z) {
      z += zcats(actual + z, sizeof(actual) - z, " ");
    }
    z += fsent_absolute_path_cat(actual + z, sizeof(actual) - z, containerof(v, fsent, vertex));
  }
  actual[z] = 0;

  assert_eq_s(test->expected, actual);

  bacon_value_free(val_parser, vars);
  bacon_parser_free(val_parser);
  pattern_parser_free(parser);
  pattern_free(pat);
  moria_parser_free(op_parser);
  entlist_destroy(&hits);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : node_lookup_test_unit_setup
  , xu_cleanup : node_lookup_test_unit_cleanup
  , xu_entry : node_lookup_test_entry
  , xu_tests : (node_lookup_test*[]) {
    /* indexed */
      (node_lookup_test[]) {{
          operations : "a/b/c/d"
        , base : "a"
        , pattern : (char[]) { "c\0" }
        , expected : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/b/c/d"
        , base : "a"
        , pattern : (char[]) { "b/c\0" }
        , expected : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/b/c/d"
        , base : "a"
        , pattern : (char[]) { "\"\\x63\"\0" }
        , expected : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , base : "a"
        , pattern : (char[]) { "b\"o\"b/c\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/.D/c"
        , base : "a"
        , pattern : (char[]) { "a/.D\0" }
        , expected : "/a/.D"
      }}
    , (node_lookup_test[]) {{
          operations : "a/b.D/c/d"
        , base : "a"
        , pattern : (char[]) { "b.D/c\0" }
        , expected : "/a/b.D/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , base : "a"
        , pattern : (char[]) { "bob/c\0" }
        , expected : "/a/bob/c"
      }}

    /* relative, down */
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d/e"
        , base : "bob"
        , pattern : (char[]) { "./c/d\0" }
        , expected : "/a/bob/c/d"
      }}

    /* relative, up */
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , base : "c"
        , pattern : (char[]) { "../c\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , base : "c"
        , pattern : (char[]) { "..\0" }
        , expected : "/a/bob"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , base : "c"
        , pattern : (char[]) { "../..\0" }
        , expected : "/a"
      }}

    /* absolute */
    , (node_lookup_test[]) {{
          operations : "a"
        , pattern : (char[]) { "/\0" }
        , base : "a"
        , expected : "/"
      }}
    , (node_lookup_test[]) {{
          operations : "a"
        , pattern : (char[]) { "//\0" }
        , base : "a"
        , expected : "//"
      }}

    /* indexed, type qualifiers - match is a file */
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)\0" }
        , expected : "/a/bob/c/d"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)+(.)\0" }
        , expected : "/a/bob/c/d"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d~(.)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(.)~(.)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "c/d+(/)\0" }
        , expected : ""
      }}

    /* indexed, type qualifiers - match is a dir */
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c+(/)\0" }
        , expected : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c~(/)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c+(.)\0" }
        , expected : ""
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c~(.)\0" }
        , expected : "/a/bob/c"
      }}

    /* variables */
    /* classes, alternations */
    , 0
  }
};
XUNIT_UNIT(xunit);
