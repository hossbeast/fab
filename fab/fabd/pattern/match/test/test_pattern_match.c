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
#include "xunit.h"
#include "xunit/assert.h"

#include "xlinux/xstdlib.h"
#include "yyutil/load.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "match.internal.h"
#include "pattern_parser.h"
#include "lookup.h"
#include "pattern.h"
#include "logging.h"
#include "fsent.h"
#include "shadow.h"
#include "module.internal.h"
#include "variant.h"
#include "filesystem.h"
#include "node_operations_test.h"

#include "common/hash.h"
#include "zbuffer.h"

typedef struct pattern_match_test {
  XUNITTEST;

  // setup
  char * operations;

  // context
  char * pattern;
  char * node;

  // results
  bool expected;
  char ** matches;  // space separated, 1, 2, etc
} pattern_match_test;

static xapi pattern_match_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(logging_finalize);
  fatal(filesystem_setup);

  // arrange
  fatal(graph_setup);
  fatal(fsent_setup);
  fatal(shadow_setup);
  fatal(variant_setup);

  finally : coda;
}

static xapi pattern_match_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);

  fatal(fsent_cleanup);
  fatal(variant_cleanup);

  finally : coda;
}

//
// tests
//

static xapi pattern_match_test_entry(xunit_test * _test)
{
  enter;

  pattern_match_test * test = (pattern_match_test *)_test;

  pattern_parser * parser = 0;
  pattern * pattern = 0;
  graph_parser * op_parser = 0;
  yyu_location loc;
  graph_invalidation_context invalidation = { 0 };
  bool matched;
  fsent *n = 0;

  // setup initial graph
  fatal(graph_parser_create, &op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  fatal(graph_parser_operations_parse, op_parser, MMS(test->operations));

fatal(graph_say, g_narrator_stdout);
printf("\n");

  // parse the pattern
  fatal(pattern_parser_create, &parser);
  fatal(match_pattern_parse_partial, parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern);
  assert_eq_u32(strlen(test->pattern), loc.l);

  llist_foreach(&fsent_list, n, vertex.owner) {
    if(memncmp(n->vertex.label, n->vertex.label_len, test->node, strlen(test->node)) == 0) {
      break;
    }
  }

  RUNTIME_ASSERT(n);

  // act
  fatal(pattern_match, pattern, fsent_parent(n), n->vertex.label, n->vertex.label_len, &matched);

  // assert
  assert_eq_b(test->expected, matched);

finally:
  fatal(pattern_parser_xfree, parser);
  pattern_free(pattern);
  fatal(graph_parser_xfree, op_parser);
  graph_invalidation_end(&invalidation);
coda;
}

//
// manifest
//

xunit_unit xunit = {
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
        , pattern : (char[]) { "**/*\0" }   // starstar only matches directories
        , node : "mod"
        , expected : false
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
        , pattern : (char[]) { "**/**/*\0" }
        , node : "foo"
        , expected : false
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
            " X/Y/bar/baz/A"
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

    , 0
  }
};
