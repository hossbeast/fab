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
#include "lookup.h"
#include "pattern.h"
#include "fsent.h"
#include "shadow.h"
#include "module.h"
#include "filesystem.h"
#include "node_operations_test.h"
#include "strht.h"

#include "common/hash.h"
#include "zcat.h"

typedef struct pattern_search_test {
  XUNITTEST;

  // setup
  char * operations;
  char * vars;

  // context
  char * pattern;
  char * module;        // name for module node

  // results
  char ** matches;      // space separated, 1, 2, etc
} pattern_search_test;

static void pattern_search_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
  graph_setup();
  filesystem_setup();
  fsent_setup();
  shadow_setup();
}

static void pattern_search_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
  filesystem_cleanup();
  fsent_cleanup();
  shadow_cleanup();
}

//
// tests
//

static void pattern_search_test_entry(xunit_test * _test)
{
  pattern_search_test * test = (pattern_search_test *)_test;

  pattern_parser * parser = 0;
  pattern * pattern = 0;
  moria_parser * op_parser = 0;
  bacon_parser * val_parser = 0;
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
  bacon_value *vars;

  // prepare the graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(op_parser, MMS(test->operations));

  // prepare the module
  resolve_fragment(MMS(test->module), &mods[0].dir_node);
  mod = &mods[0];
  bacon_parser_alloc(&val_parser);
  bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  mod->vars = vars;

  // parse the pattern
  pattern_parser_create(&parser);
  if(search_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern) != 0) {
    tracef("%.*s", (int)parser->search_yyu.error_len, parser->search_yyu.error_str);
  }
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  pattern_search_matches_init(&matches);
  pattern_search(pattern, mod, mod->dir_node, &matches, 0, 0);

  // assert
  hashtable_initx(&expected, 0, strht_hash, strht_cmp, 0);
  for(x = 0; x < sentinel(test->matches); x++) {
    hashtable_put(&expected, test->matches[x]);
  }

  hashtable_initx(&actual, 0, strht_hash, strht_cmp, free);
  for(x = 0; x < matches.table_size; x++)
  {
    pattern_search_node * m;
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
  pattern_free(pattern);
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
    xu_setup : pattern_search_test_unit_setup
  , xu_cleanup : pattern_search_test_unit_cleanup
  , xu_entry : pattern_search_test_entry
  , xu_tests : (pattern_search_test*[]) {
    /* single word */
      (pattern_search_test[]) {{
          operations : "dir/bar"
        , vars : ""
        , module : "dir"
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
        , module : "mod"
        , pattern : (char[]) { "foo.a\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.a"
        , vars : ""
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "foo.b.bar\0" }
        , matches : (char*[]) {
              "/mod/foo.b.bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "A.B/C/D"
        , vars : ""
        , module : "A.B"
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
        , module : "dir"
        , pattern : (char[]) { "foo/bar\0" }
        , matches : (char*[]) {
              "/dir/foo/bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "mod/A/B"
        , vars : ""
        , module : "A"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "{A,B}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/ab"
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "{A}B\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AB"
        , vars : ""
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "{A}{B}\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AC"
        , vars : ""
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "{A.B,X}/E\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
        , vars : ""
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "[ABC]\0" }
        , matches : (char*[]) { "/mod/A", 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/b"
        , vars : ""
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
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
        , module : "mod"
        , pattern : (char[]) { "scar+(/)\0" }  // directory negative test
        , matches : (char*[]) {
            0
          }
      }}
    , (pattern_search_test[]) {{
          operations :
            " mod/scar/foo"
            " mod/carf/bar"
        , module : "mod"
        , vars : ""
        , pattern : (char[]) { "scar+(/)\0" }  // directory positive test
        , matches : (char*[]) {
              "/mod/scar"
            , 0
          }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
