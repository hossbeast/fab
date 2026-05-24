/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"

#include "moria/parser.h"
#include "bacon/parser.h"
#include "bacon/load.h"

#include "match.internal.h"
#include "pattern_parser.h"
#include "pattern.h"
#include "fsent.h"
#include "shadow.h"
#include "module.h"
#include "fstree.h"
#include "node_operations_test.h"


typedef struct pattern_match_test {
  XUNITTEST;

  // setup
  char *operations;

  // context
  char *pattern;
  char *node;
  char *vars;

  // results
  bool expected;
  char **matches;  // space separated, 1, 2, etc
} pattern_match_test;

static void pattern_match_test_unit_setup(xunit_unit * unit)
{
  bacon_load();

  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  graph_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void pattern_match_test_unit_cleanup(xunit_unit * unit)
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

static void pattern_match_test_entry(xunit_test * _test)
{
  pattern_match_test *test = (pattern_match_test *)_test;

  pattern_parser *parser = 0;
  match_pattern *pattern = 0;
  moria_parser *op_parser = 0;
  yyu_location loc;
  graph_invalidation_context invalidation = { 0 };
  bool matched;
  fsent *fse;
  fsent *n = 0;
  int r;
  bacon_value *vars = 0;
  bacon_parser *val_parser = 0;

  // setup initial graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  r = moria_parser_operations_parse(op_parser, MMS(test->operations));
  RUNTIME_ASSERT(r == 0);

  // parse the pattern
  pattern_parser_create(&parser);
  match_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern);
  assert_eq_u32(strlen(test->pattern), loc.l);

  bacon_parser_alloc(&val_parser);
  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }

  /* locate node to match */
  n = 0;
  llist_foreach(&fsent_list, fse, vertex.owner) {
    if(memncmp(fse->vertex.label, fse->vertex.label_len, test->node, strlen(test->node)) == 0) {
      n = fse;
      break;
    }
  }

  RUNTIME_ASSERT(n);

  // act
  matched = pattern_match(pattern, vars, n);

  // assert
  assert_eq_b(test->expected, matched);

  pattern_parser_free(parser);
  pattern_free(&pattern->pattern);
  moria_parser_free(op_parser);
  graph_invalidation_end(&invalidation);
  bacon_value_free(val_parser, vars);
  bacon_parser_free(val_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : pattern_match_test_unit_setup
  , xu_cleanup : pattern_match_test_unit_cleanup
  , xu_entry : pattern_match_test_entry
  , xu_tests : (pattern_match_test*[]) {
    /* single word */
      (pattern_match_test[]) {{
          operations : "dir/bar"
        , node : "dir"
        , pattern : (char[]) { "dir\0" }
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : "dir/bar"
        , node : "dir"
        , pattern : (char[]) { "bar\0" }
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : "dir/bar"
        , node : "bar"
        , pattern : (char[]) { "bar\0" }
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : "dir/bar"
        , node : "bar"
        , pattern : (char[]) { "dir\0" }
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : "dir/bar"
        , node : "bar"
        , pattern : (char[]) { "dir/bar\0" }
        , expected : true
      }}

    /* words, characters */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo"
        , pattern : (char[]) { "foo.a\0" }
        , node : "foo"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo.a"
        , node : "foo.a"
        , pattern : (char[]) { "foo.a\0" }
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo.b"
            " mod/foo.b.bar"
        , node : "foo.b.bar"
        , pattern : (char[]) { "foo.b.bar\0" }
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : "A.B/C/D"
        , pattern : (char[]) { "C/D\0" }
        , node : "D"
        , expected : true
      }}

    /* multi directory */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/abcd/baz"
            " mod/foo/abcd/oof/a"
            " mod/foo/abcd/oof/b"
        , pattern : (char[]) { "foo/abcd/oof/b\0" }
        , node : "b"
        , expected : true
      }}

    /* dentry section with consecutive words */
    , (pattern_match_test[]) {{
          operations : "mod/BCD"
        , pattern : (char[]) { "B\"C\"D\0" }
        , node : "BCD"
        , expected : true
      }}

    /* alternations */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
        , pattern : (char[]) { "{A,B}\0" }
        , node : "A"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
        , pattern : (char[]) { "{A,B}\0" }
        , node : "B"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AD"
            " mod/AE"
            " mod/AF"
            " mod/BD"
            " mod/BE"
            " mod/BF"
            " mod/CD"
            " mod/CE"
            " mod/CF"
        , pattern : (char[]) { "{A,B}{D,E}\0" }
        , node : "AD"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AD"
            " mod/AE"
            " mod/AF"
            " mod/BD"
            " mod/BE"
            " mod/BF"
            " mod/CD"
            " mod/CE"
            " mod/CF"
        , pattern : (char[]) { "{A,B}{D,E}\0" }
        , node : "AE"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AD"
            " mod/AE"
            " mod/AF"
            " mod/BD"
            " mod/BE"
            " mod/BF"
            " mod/CD"
            " mod/CE"
            " mod/CF"
        , pattern : (char[]) { "{A,B}{D,E}\0" }
        , node : "AF"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AD"
            " mod/AE"
            " mod/AF"
            " mod/BD"
            " mod/BE"
            " mod/BF"
            " mod/CD"
            " mod/CE"
            " mod/CF"
        , pattern : (char[]) { "{A,B}{D,E}\0" }
        , node : "BD"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AD"
            " mod/AE"
            " mod/AF"
            " mod/BD"
            " mod/BE"
            " mod/BF"
            " mod/CD"
            " mod/CE"
            " mod/CF"
        , pattern : (char[]) { "{A,B}{D,E}\0" }
        , node : "BE"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AD"
            " mod/AE"
            " mod/AF"
            " mod/BD"
            " mod/BE"
            " mod/BF"
            " mod/CD"
            " mod/CE"
            " mod/CF"
        , pattern : (char[]) { "{A,B}{D,E}\0" }
        , node : "BF"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
            " mod/C"
        , pattern : (char[]) { "{A,{B}}\0" }
        , node : "A"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
            " mod/C"
        , pattern : (char[]) { "{A,{B}}\0" }
        , node : "B"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
            " mod/C"
        , pattern : (char[]) { "{A,{B}}\0" }
        , node : "C"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , pattern : (char[]) { "A/B{X,C,Y}D/E\0" }
        , node : "E"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , pattern : (char[]) { "{X,C,Y}D/E\0" }
        , node : "E"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , pattern : (char[]) { "B{X,C,Y}D/E\0" }
        , node : "E"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , pattern : (char[]) { "A/B{X,{C,Y}}D/E\0" }
        , node : "E"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A/E"
        , pattern : (char[]) { "{A.B,X}/E\0" }
        , node : "E"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A/E"
        , pattern : (char[]) { "{{A.B,X}}/E\0" }
        , node : "E"
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/AB/E"
        , pattern : (char[]) { "AB{,X}/E\0" }
        , node : "E"
        , expected : true
      }}

    /* alternations + epsilon */

    /* classes */
    , (pattern_match_test[]) {{
          operations : "mod/A"
        , pattern : (char[]) { "[ABC]\0" }
        , matches : (char*[]) { "/mod/A", 0 }
        , node : "A"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/b"
        , pattern : (char[]) { "[A-Za-z]\0" }
        , node : "A"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/b"
        , pattern : (char[]) { "[A-Za-z]\0" }
        , node : "b"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/A"
            " mod/Bb"
        , pattern : (char[]) { "[A-Z][a-z]\0" }
        , node : "Bb"
        , expected : true
      }}

    /* star */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd"
        , pattern : (char[]) { "ab*\0" }
        , node : "abcd"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd"
        , pattern : (char[]) { "*cd\0" }
        , node : "abcd"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd"
        , pattern : (char[]) { "a*d\0" }
        , node : "abcd"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd"
        , pattern : (char[]) { "ab*cd\0" }
        , node : "abcd"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd"
        , pattern : (char[]) { "a*c*\0" }
        , node : "abcd"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd"
        , pattern : (char[]) { "*c*\0" }
        , node : "abcd"
        , expected : true
      }}

    /* star + alternation */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/bcd/e"
            " mod/bd/f"
        , pattern : (char[]) { "*{d,cd}/e\0" }
        , node : "e"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/bcd/e"
            " mod/bd/f"
        , pattern : (char[]) { "*{d,cd}/f\0" }
        , node : "f"
        , expected : true
      }}

    /* star / multi directory */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd/x/abc"
            " mod/xyzw/x/bcd"
        , pattern : (char[]) { "*/*/abc\0" }
        , node : "abc"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/abcd/x/abc"
            " mod/xyzw/x/bcd"
        , pattern : (char[]) { "*/*/bcd\0" }
        , node : "bcd"
        , expected : true
      }}

    /* starstar */
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/a"
            " mod/foo/bar/b"
        , pattern : (char[]) { "**/*\0" }
        , node : "a"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/a"
            " mod/foo/bar/b"
        , pattern : (char[]) { "**\0" }
        , node : "foo"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/a"
            " mod/foo/bar/b"
        , pattern : (char[]) { "**\0" }
        , node : "b"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/a"
            " mod/foo/bar/b"
        , pattern : (char[]) { "**\0" }
        , node : "bar"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/*\0" }
        , node : "qux"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/baz"
        , pattern : (char[]) { "**/*\0" }
        , node : "foo"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/baz"
        , pattern : (char[]) { "**/*\0" }
        , node : "baz"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/baz"
        , pattern : (char[]) { "**/*\0" }   // starstar only matches directories - but this includes the root
        , node : "mod"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/**/*\0" }
        , node : "qux"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/**/*\0" }
        , node : "bar"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/*\0" }  // starstar matches zero or more directories away
        , node : "foo"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/**/*\0" }  // starstar matches zero or more directories away
        , node : "foo"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/**/**/*\0" }  // starstar matches zero or more directories away
        , node : "foo"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " mod/foo/bar/baz/qux"
        , pattern : (char[]) { "**/qux\0" }
        , node : "qux"
        , expected : true
      }}

    /* starstar+conjunctions */
    , (pattern_match_test[]) {{
          operations : ""
            " foo/bar/A"
            " foo/bar/baz/B"
/* in a match pattern, qualifiers on a starstar are only evaluated against the immediate parent */
        , pattern : (char[]) { "**~baz/B\0" }
        , node : "B"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " foo/bar/A"
            " foo/bar/baz/B"
        , pattern : (char[]) { "**+baz/B\0" }
        , node : "B"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " foo/bar/A"
            " foo/bar/baz/B"
        , pattern : (char[]) { "**~baz/A\0" }
        , node : "A"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " foo/bar/A"
            " foo/bar/baz/B"
        , pattern : (char[]) { "**+baz/A\0" }
        , node : "A"
        , expected : false
      }}

    /* multi-starstar */
    , (pattern_match_test[]) {{
          operations : ""
            " X/Y/bar/baz/A"
            " X/Z/bar/baz/qux/B"
        , pattern : (char[]) { "Y/**/[AB]\0" }
        , node : "A"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " X/Y/bar/baz/A"
            " X/Z/bar/baz/qux/B"
        , pattern : (char[]) { "Y/**/[AB]\0" }
        , node : "B"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " X/Z/bar/baz/A"
            " X/Z/bar/baz/qux/B"
        , pattern : (char[]) { "Z/**+qux/[AB]\0" }
        , node : "A"
        , expected : false
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " X/Z/bar/baz/A"
            " X/Z/bar/baz/qux/B"
        , pattern : (char[]) { "Z/**+qux/[AB]\0" }
        , node : "B"
        , expected : true
      }}

    /* match against the root node */
    , (pattern_match_test[]) {{
          operations : ""
            " X/Y/bar"
        , pattern : (char[]) { "/X/Y/bar\0" }
        , node : "bar"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " X"
        , pattern : (char[]) { "/X\0" }
        , node : "X"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " fab/\"builtin-modules+devel\""
        , pattern : (char[]) { "fab/\"builtin-modules+devel\"\0" }
        , node : "builtin-modules+devel"
        , expected : true
      }}
    /* match with segment sysvars */
    , (pattern_match_test[]) {{
          operations : ""
            " A/B"
        , pattern : (char[]) { "A/$bm_module_id\0" }
        , vars : (char[]) { "bm_module_id : B\0" }
        , node : "B"
        , expected : true
      }}
    , (pattern_match_test[]) {{
          operations : ""
            " A/BCD"
        , pattern : (char[]) { "A/B${bm_bamd_id}D\0" }
        , vars : (char[]) { "bm_bamd_id : C\0" }
        , node : "BCD"
        , expected : true
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
