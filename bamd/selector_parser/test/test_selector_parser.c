/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"

#include "yyutil/grammar.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "selector_parser.h"
#include "selector.internal.h"
#include "selector_parser.h"
#include "request.h"
#include "fsent.h"

typedef struct {
  XUNITTEST;

  char * text;
  char * expected;
} selector_parser_test;

static void selector_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void selector_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void selector_parser_test_entry(xunit_test * _test)
{
  selector_parser_test * test = (typeof(test))_test;
  char buf[1024];
  size_t len;
  const char *expected;

  selector_parser * parser = 0;
  selector * A = 0;
  selector * B = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  yyu_location loc;

  // arrange
  selector_parser_create(&parser);

  // act - initial parse
  if(selector_parser_parse_partial(parser, test->text, strlen(test->text) + 2, 0, 0, &loc, &A) != 0) {
    tracef("%.*s", (int)parser->error_len, parser->error);
  }
  assert_eq_u32(strlen(test->text), loc.l);

  narrator_growing_create(&N1);
  selector_say(A, &N1->base);

  // re-parse
  len = N1->l;
  narrator_seek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;

  if(selector_parser_parse_partial(parser, buf, len + 2, 0, 0, &loc, &B) != 0) {
    tracef("%.*s", (int)parser->error_len, parser->error);
  }
  assert_eq_u32(len, loc.l);

  narrator_growing_create(&N2);
  selector_say(B, &N2->base);

  // round-trip textual equivalence
  assert_eq_w(N1->s, N1->l, N2->s, N2->l);

  expected = test->expected ?: test->text;
  assert_eq_w(expected, strlen(expected), N1->s, N1->l);

  selector_parser_free(parser);
  selector_free(A);
  selector_free(B);
  narrator_growing_free(N1);
  narrator_growing_free(N2);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : selector_parser_test_unit_setup
  , xu_cleanup : selector_parser_test_unit_cleanup
  , xu_entry : selector_parser_test_entry
  , xu_tests : (selector_parser_test*[]) {
      (selector_parser_test[]) {{
          text : (char[]) {
            "search : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "search : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "search : {\n"
            "  pattern : //bm/modules/*/anchor\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "search : {\n"
            "  pattern : //bm/modules/*/anchor\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "path : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "  nofollow : false\n"
            "  hidden : false\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "path : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "  nofollow : false\n"
            "  hidden : false\n"
            "  expect : nonzero\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "path : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "  nofollow : false\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "path : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "  nofollow : false\n"
            "  hidden : false\n"
            "  expect : nonzero\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "path : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "  nofollow : false\n"
            "  expect : any\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "path : {\n"
            "  pattern : ./foo/bar/main.o\n"
            "  nofollow : false\n"
            "  hidden : false\n"
            "  expect : any\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "path : {\n"
            "  pattern : /foo/bar/main.o\n"
            "  hidden : false\n"
            "}\0\0"
          }
         , expected : (char[]) {
            "path : {\n"
            "  pattern : /foo/bar/main.o\n"
            "  nofollow : false\n"
            "  hidden : false\n"
            "  expect : nonzero\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "lookup : {\n"
            "  pattern : foo/bar/main.o\n"
            "  namespace : module\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "lookup : {\n"
            "  pattern : foo/bar/main.o\n"
            "  namespace : module\n"
            "  expect : nonzero\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "module\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
            "traverse : {\n"
              "  direction : up\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "traverse : {\n"
            "  direction : up\n"
            "  order : preorder\n"
            "  graph : fs\n"
            "  exhaustive : false\n"
            "  cross-module : true\n"
            "  cross-fstree : true\n"
            "  hidden : false\n"
            "  nofollow : false\n"
            "}\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    direction : up\n"
             "  }\n"
             "  lookup : {\n"
             "    pattern : foo/bar/main.o\n"
             "    namespace : module\n"
             "  }\n"
             "]\0\0"
          }
        , expected : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    direction : up\n"
             "    order : preorder\n"
             "    graph : fs\n"
             "    exhaustive : false\n"
             "    cross-module : true\n"
             "    cross-fstree : true\n"
             "    hidden : false\n"
             "    nofollow : false\n"
             "  }\n"
             "  lookup : {\n"
             "    pattern : foo/bar/main.o\n"
             "    namespace : module\n"
             "    expect : nonzero\n"
             "  }\n"
             "]\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "union : [\n"
             "  lookup : {\n"
             "    pattern : foo/bar/main.o\n"
             "    namespace : module\n"
             "    expect : nonzero\n"
             "  }\n"
             "]\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    min-distance : 1\n"
             "    max-distance : 1\n"
             "    direction : down\n"
             "    graph : depends\n"
             "  }\n"
             "  lookup : {\n"
             "    pattern : foo/bar/main.o\n"
             "    namespace : module\n"
             "  }\n"
             "]\0\0\n"
          }
        /* default min-distance of 1 is omitted */
        , expected :
             "sequence : [\n"
             "  traverse : {\n"
             "    max-distance : 1\n"
             "    direction : down\n"
             "    order : preorder\n"
             "    graph : depends\n"
             "    exhaustive : false\n"
             "    cross-module : true\n"
             "    cross-fstree : true\n"
             "    hidden : false\n"
             "    nofollow : false\n"
             "  }\n"
             "  lookup : {\n"
             "    pattern : foo/bar/main.o\n"
             "    namespace : module\n"
             "    expect : nonzero\n"
             "  }\n"
             "]\0\0\n"
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    min-distance : 1\n"
             "    max-distance : 2\n"
             "    cross-module : false\n"
             "  }\n"
             "  traverse : {\n"
             "    min-distance : 3\n"
             "    max-distance : 4\n"
             "  }\n"
             "  traverse : {\n"
             "    min-distance : 5\n"
             "    max-distance : 6\n"
             "  }\n"
             "  traverse : {\n"
             "    min-distance : 7\n"
             "    max-distance : 8\n"
             "  }\n"
             "]\0\0\n"
          }
        /* default min-distance of 1 is omitted */
        , expected : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    max-distance : 2\n"
             "    direction : down\n"
             "    order : preorder\n"
             "    graph : fs\n"
             "    exhaustive : false\n"
             "    cross-module : false\n"
             "    cross-fstree : true\n"
             "    hidden : false\n"
             "    nofollow : false\n"
             "  }\n"
             "  traverse : {\n"
             "    min-distance : 3\n"
             "    max-distance : 4\n"
             "    direction : down\n"
             "    order : preorder\n"
             "    graph : fs\n"
             "    exhaustive : false\n"
             "    cross-module : true\n"
             "    cross-fstree : true\n"
             "    hidden : false\n"
             "    nofollow : false\n"
             "  }\n"
             "  traverse : {\n"
             "    min-distance : 5\n"
             "    max-distance : 6\n"
             "    direction : down\n"
             "    order : preorder\n"
             "    graph : fs\n"
             "    exhaustive : false\n"
             "    cross-module : true\n"
             "    cross-fstree : true\n"
             "    hidden : false\n"
             "    nofollow : false\n"
             "  }\n"
             "  traverse : {\n"
             "    min-distance : 7\n"
             "    max-distance : 8\n"
             "    direction : down\n"
             "    order : preorder\n"
             "    graph : fs\n"
             "    exhaustive : false\n"
             "    cross-module : true\n"
             "    cross-fstree : true\n"
             "    hidden : false\n"
             "    nofollow : false\n"
             "  }\n"
             "]\0\0\n"
          }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
