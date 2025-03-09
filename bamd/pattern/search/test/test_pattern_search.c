/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"

#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/parser.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/hashtable.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "bacon/parser.h"
#include "bacon/load.h"
#include "bacon/builder.h"

#include "search.internal.h"
#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern.h"
#include "fsent.h"
#include "shadow.h"
#include "module.h"
#include "filesystem.h"
#include "node_operations_test.h"
#include "strht.h"
#include "fstree.h"
#include "args.h"

#include "zcat.h"

typedef struct pattern_search_test {
  XUNITTEST;

  // setup
  char *operations;
  char *vars;

  // context
  char *pattern;
  char *base;        // module base dir
  char *anchor;      // module anchor dir

  // results
  char **matches;      // space separated, 1, 2, etc
} pattern_search_test;

static void pattern_test_unit_setup(xunit_unit * unit)
{
  bacon_load();

  args_init();
  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  graph_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void pattern_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();

  shadow_cleanup();
  fstree_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

//
// tests
//

static void search_hit(void *ctx, const pattern_search_hit *hitp)
{
  hashtable *matches = ctx;
  pattern_search_hit *hit;

  xmalloc(&hit, sizeof(*hit));
  memcpy(hit, hitp, sizeof(*hit));
  hashtable_put(matches, hit);
}

static void pattern_search_test_entry(xunit_test * _test)
{
  pattern_search_test * test = (pattern_search_test *)_test;

  pattern_parser *parser = 0;
  search_pattern *pattern = 0;
  moria_parser *op_parser = 0;
  bacon_parser *val_parser = 0;
  hashtable matches;
  module mods[4] = { 0 };
  module *mod;
  hashtable expected;
  hashtable actual;
  int x;
  int y;
  char path[PATH_MAX];
  char * actual_match = 0;
  yyu_location loc;
  graph_invalidation_context invalidation = { 0 };
  size_t pz;
  size_t sz;
  size_t z;
  bacon_value *vars = 0;

  // prepare the graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(op_parser, MMS(test->operations));

  // prepare the module
  mods[0].build_dir = resolve_fragment(MMS(test->base));
  mods[0].anchor_dir = mods[0].build_dir;
  if(test->anchor) {
    mods[0].anchor_dir = resolve_fragment(MMS(test->anchor));
  }
  mod = &mods[0];
  bacon_parser_alloc(&val_parser);
  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }
  mod->vars = vars;

  // parse the pattern
  pattern_parser_create(&parser);
  if(search_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern) != 0) {
    tracef("%.*s", (int)parser->search_yyu.error_len, parser->search_yyu.error_str);
  }
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  pattern_search_matches_init(&matches);
  pattern_search(pattern, mod, search_hit, &matches, 0, 0);

  // assert
  hashtable_initx(&expected, 0, strht_hash, strht_cmp, 0);
  for(x = 0; x < sentinel(test->matches); x++) {
    hashtable_put(&expected, test->matches[x]);
  }

  hashtable_initx(&actual, 0, strht_hash, strht_cmp, free);
  for(x = 0; x < matches.table_size; x++)
  {
    pattern_search_hit * m;
    if(!(m = hashtable_table_entry(&matches, x))) {
      continue;
    }

    pz = fsent_absolute_path_cat(path, sizeof(path), m->node);
    sz = pz;

    for(y = 0; y <= m->group_max; y++)
    {
      if(!m->groups[y].start) {
        continue;
      }

      if(y <= 9) { sz += 3; }
      else { sz += 4; }

      sz += m->groups[y].len;
    }

    sz += 1;
    xmalloc(&actual_match, sz);

    z = 0;
    z += zcatw(actual_match + z, sz - z, path, pz);

    for(y = 1; y <= m->group_max; y++)
    {
      if(!m->groups[y].start) {
        continue;
      }

      z += zcatf(actual_match + z, sz - z, " %d ", y);
      z += zcatw(actual_match + z, sz - z, m->groups[y].start, m->groups[y].len);
    }

    hashtable_put(&actual, actual_match);
    actual_match = 0;
  }

  assert_eq_strht(&expected, &actual);

  bacon_value_free(val_parser, vars);
  hashtable_destroy(&matches);
  pattern_parser_free(parser);
  pattern_free(&pattern->pattern);
  moria_parser_free(op_parser);
  hashtable_destroy(&expected);
  hashtable_destroy(&actual);
  free(actual_match);
  graph_invalidation_end(&invalidation);
  bacon_parser_free(val_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : pattern_test_unit_setup
  , xu_cleanup : pattern_test_unit_cleanup
  , xu_entry : pattern_search_test_entry
  , xu_tests : (pattern_search_test*[]) {
    /* single word */
      (pattern_search_test[]) {{
          operations : "dir/bar"
        , vars : ""
        , base : "dir"
        , pattern : (char[]) { "bar\0" }
        , matches : (char*[]) {
              "/dir/bar"
            , 0
          }
      }}

    /* words, characters */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "foo.a\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.a"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "foo\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.a"
            " mod/foo.b"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "foo.b\0" }
        , matches : (char*[]) {
              "/mod/foo.b"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.b"
            " mod/foo.b.bar"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "foo.b.bar\0" }
        , matches : (char*[]) {
              "/mod/foo.b.bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "A.B/C/D"
        , vars : ""
        , base : "A.B"
        , pattern : (char[]) { "C/D\0" }
        , matches : (char*[]) {
              "/A.B/C/D"
            , 0
          }
      }}

    /* multi directory */
    , (pattern_search_test[]) {{
          operations : "dir/foo/bar"
        , vars : ""
        , base : "dir"
        , pattern : (char[]) { "foo/bar\0" }
        , matches : (char*[]) {
              "/dir/foo/bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "mod/A/B"
        , vars : ""
        , base : "A"
        , pattern : (char[]) { "main.o\0" }
        , matches : (char*[]) { 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/abcd/bar/a"
            " mod/foo/abcd/bar/gondor/valyria/b"
            " mod/foo/abcd/baz"
            " mod/foo/abcd/oof/a"
            " mod/foo/abcd/oof/b"
            " mod/foo/abcd/qux/a"
            " mod/foo/abcd/qux/b"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "foo/abcd/oof/b\0" }
        , matches : (char*[]) {
              "/mod/foo/abcd/oof/b"
            , 0
          }
      }}

    /* dentry section with consecutive words */
    , (pattern_search_test[]) {{
          operations : "mod/BCD"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "B\"C\"D\0" }
        , matches : (char*[]) {
              "/mod/BCD"
            , 0
          }
      }}

    /* alternations */
    , (pattern_search_test[]) {{
          operations :
            " mod/ab"
        , base : "mod"
        , vars : ""
        , pattern : (char[]) { "{ab}\0" }
        , matches : (char*[]) {
              "/mod/ab"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A,B}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/B"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/C"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A,B}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/ab"
        , base : "mod"
        , vars : ""
        , pattern : (char[]) { "{a,ab}\0" }
        , matches : (char*[]) {
              "/mod/ab"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/ab"
        , base : "mod"
        , vars : ""
        , pattern : (char[]) { "{ab,a}\0" }
        , matches : (char*[]) {
              "/mod/ab"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/ab"
        , base : "mod"
        , vars : ""
        , pattern : (char[]) { "{a,ab,b}\0" }
        , matches : (char*[]) {
              "/mod/ab"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AC"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A,B}{C,D}\0" }
        , matches : (char*[]) {
              "/mod/AC"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A}B\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AB"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A}B\0" }
        , matches : (char*[]) {
              "/mod/AB"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AB"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A}{B}\0" }
        , matches : (char*[]) {
              "/mod/AB"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A}{B}\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AC"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A,B,C}{E,F}\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
            " mod/C"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A,{B}}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/B"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "A/B{X,C,Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
            " mod/A/BXD/E"
            " mod/A/BYD/E"
            " mod/A/B"
            " mod/A/BX"
            " mod/A/BC"
            " mod/BCD/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "A/B{X,C,Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , "/mod/A/BXD/E"
            , "/mod/A/BYD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "A/B{{X,C},Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
            " mod/A/BYD/E"
            " mod/A/BCD/e"
            " mod/A/aBCD/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "A/B{{X,C},Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , "/mod/A/BYD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "A/B{X,{C,Y}}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
            " mod/X/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A.B,X}/E\0" }
        , matches : (char*[]) {
              "/mod/X/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
            " mod/X/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A.B,{X}}/E\0" }
        , matches : (char*[]) {
              "/mod/X/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{A.B,X}/E\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "{{A.B,X}}/E\0" }
        , matches : (char*[]) {
              0
          }
      }}

    /* alternations + epsilon */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AB/E"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "AB{,X}/E\0" }
        , matches : (char*[]) {
              "/mod/AB/E"
            , 0
          }
      }}

    /* classes */
    , (pattern_search_test[]) {{
          operations : "mod/A"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "[ABC]\0" }
        , matches : (char*[]) { "/mod/A", 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/b"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "[A-Za-z]\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/b"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "[A-Z]\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/B"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/Bb"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "[A-Z][a-z]\0" }
        , matches : (char*[]) {
              "/mod/Bb"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/bB"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "[a-z][A-Z]\0" }
        , matches : (char*[]) {
              "/mod/bB"
            , 0
          }
      }}

    /* star */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "ab*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*cd\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "a*d\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "ab*cd\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "a*c*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*c*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}

    /* star + alternation */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bcd/e"
            " mod/bd/e"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*{d,cd}/e\0" }
        , matches : (char*[]) {
              "/mod/bcd/e"
            , "/mod/bd/e"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*{lm,ab}*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd/efgh"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*{lm,ab}*/[a-z][a-z][a-z][a-z]\0" }
        , matches : (char*[]) {
              "/mod/abcd/efgh"
            , 0
          }
      }}

    /* star / multi directory */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd/x/abcd"
            " mod/xyzw/x/abcd"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*/*/abcd\0" }
        , matches : (char*[]) {
              "/mod/abcd/x/abcd"
            , "/mod/xyzw/x/abcd"
            , 0
          }
      }}

    /* starstar */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**\0" } // starstar only matches directories
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**+(/)\0" } // can match directories with qualifiers
        , matches : (char*[]) {
              "/mod"
            , "/mod/foo"
            , "/mod/foo/bar"
            , "/mod/foo/bar/baz"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/*\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , "/mod/foo/bar/baz/qux"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/**\0" }  // only matches directories
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/**/*\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , "/mod/foo/bar/baz/qux"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/qux\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , "/mod/foo/bar/baz/qux"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/foo/B"
            " mod/foo/bar/C"
            " mod/foo/bar/baz/D"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/*\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/foo/B"
            , "/mod/foo/bar/C"
            , "/mod/foo/bar/baz/D"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/foo/B"
            " mod/foo/bar/C"
            " mod/foo/bar/baz/D"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/**/*\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/foo/B"
            , "/mod/foo/bar/C"
            , "/mod/foo/bar/baz/D"
            , 0
          }
      }}

    /* starstar+conjunctions */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**~baz/qux\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , 0
          }
      }}

    /* star+conjunctions */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*~baz\0" }
        , matches : (char*[]) {
              "/mod/bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*+baz\0" }
        , matches : (char*[]) {
              "/mod/baz"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/integ/a"
            " mod/integ/a/a"
            " mod/integ/a/a/a"
            " mod/src/a"
            " mod/src/b/a"
            " mod/src/c/d/a"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*~integ/**/a\0" }
        , matches : (char*[]) {
              "/mod/src/a"
            , "/mod/src/b/a"
            , "/mod/src/c/d/a"
            , 0
          }
      }}

    /* qualifier-list */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo"
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*~bar\0" }
        , matches : (char*[]) {
              "/mod/foo"
            , "/mod/baz"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo"
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*~bar~baz\0" }
        , matches : (char*[]) {
              "/mod/foo"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo"
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*~baz~bar\0" }
        , matches : (char*[]) {
               "/mod/foo"
            , 0
          }
      }}

    /* groups */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "ba([rz])\0" }
        , matches : (char*[]) {
              "/mod/bar 1 r"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "ba({r,z})\0" }
        , matches : (char*[]) {
              "/mod/bar 1 r"
            , "/mod/baz 1 z"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "*ba(r)\0" }
        , matches : (char*[]) {
              "/mod/bar 1 r"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/foo/bar"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "(f)oo/ba(r)\0" }
        , matches : (char*[]) {
              "/mods/mod/foo/bar 1 f 2 r"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/A"
            " mods/mod/foo/B"
            " mods/mod/foo/bar/C"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "**/(*)\0" }
        , matches : (char*[]) {
              "/mods/mod/A 1 A"
            , "/mods/mod/foo/B 1 B"
            , "/mods/mod/foo/bar/C 1 C"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/abc"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "(*)bc\0" }
        , matches : (char*[]) {
              "/mods/mod/abc 1 a"
            , 0
          }
      }}

    /* nested groups */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/car"
            " mod/carf"
            " mod/carff"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "c((ar))f\0" }
        , matches : (char*[]) {
              "/mod/carf 1 ar 2 ar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/c"
            " mod/ca"
            " mod/car"
            " mod/carf"
            " mod/carff"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "c(((ar)))f\0" }
        , matches : (char*[]) {
              "/mod/carf 1 ar 2 ar 3 ar"
            , 0
          }
      }}

    /* conjunctions + groups */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/barf"
            " mod/barb"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "(([bc]ar)+bar)f\0" }
        , matches : (char*[]) {
              "/mod/barf 1 bar 2 bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/barf"
            " mod/carb"
            " mod/carf"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "([bc]ar~bar)f\0" }
        , matches : (char*[]) {
              "/mod/carf 1 car"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/barf"
            " mod/carf"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "(*~b)arf\0" }
        , matches : (char*[]) {
              "/mod/carf 1 c"
            , 0
          }
      }}

    /* axes, nodesets */
    , (pattern_search_test[]) {{
          operations :
            " mod/scar"
            " mod/carf"
        , base : "mod"
        , vars : ""
        , pattern : (char[]) { ".\0" }  // nodeset-self, but directories arent matched without qualifiers
        , matches : (char*[]) {
              0
          }
      }}

    /* variables */
    , (pattern_search_test[]) {{
          operations :
            " mod/barf"
            " mod/carb"
            " mod/carf"
        , vars : "v : carb"
        , base : "mod"
        , pattern : (char[]) { "$v\0" }
        , matches : (char*[]) {
              "/mod/carb"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/barf"
            " mod/carb"
            " mod/carf"
        , vars : "v : car"
        , base : "mod"
        , pattern : (char[]) { "${v}b\0" }
        , matches : (char*[]) {
              "/mod/carb"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/a/car/b"
            " mod/a/bar/c"
        , vars : "v : car"
        , base : "mod"
        , pattern : (char[]) { "a/$v/b\0" }
        , matches : (char*[]) {
              "/mod/a/car/b"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/scar"
            " mod/carb"
            " mod/carf"
        , vars : "v : car"
        , base : "mod"
        , pattern : (char[]) { "s${v}\0" }
        , matches : (char*[]) {
              "/mod/scar"
            , 0
          }
      }}

    /* qualifiers */
    , (pattern_search_test[]) {{
          operations :
            " mod/scar"
            " mod/carf"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "scar+(.)\0" }  // regular-file positive test
        , matches : (char*[]) {
              "/mod/scar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/scar/bar"
            " mod/carf"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "scar+(.)\0" }  // regular-file negative test
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/scar"
            " mod/carf"
        , vars : ""
        , base : "mod"
        , pattern : (char[]) { "scar+(/)\0" }  // directory negative test
        , matches : (char*[]) {
            0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/scar/foo"
            " mod/carf/bar"
        , base : "mod"
        , vars : ""
        , pattern : (char[]) { "scar+(/)\0" }  // directory positive test
        , matches : (char*[]) {
              "/mod/scar"
            , 0
          }
      }}

    /* different initial sections */
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "//\0" }        // just the shadow node
        , matches : (char*[]) {
              "//"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "//a\0" }        // shadow node, no match
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "/\0" }         // just the root node
        , matches : (char*[]) {
              "/"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "/a\0" }         // root node, no match
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "/mod+(/)\0" }   // root node, match
        , matches : (char*[]) {
              "/mod"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "$^D\0" }        // just the module base dir
        , matches : (char*[]) {
              "/mod"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/foo/bar"
        , base : "mod"
        , pattern : (char[]) { "$^D/foo/bar\0" }    // off the module base dir
        , matches : (char*[]) {
              "/mod/foo/bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " base/foo/bar"
            " anchor/baz/qux"
        , base : "base"
        , anchor : "anchor"
        , pattern : (char[]) { "$^A\0" }        // just the module anchor dir
        , matches : (char*[]) {
              "/anchor"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " base/foo/bar"
            " anchor/baz/qux"
        , base : "base"
        , anchor : "anchor"
        , pattern : (char[]) { "$^A/baz/qux\0" } // off the anchor dir
        , matches : (char*[]) {
              "/anchor/baz/qux"
            , 0
          }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
