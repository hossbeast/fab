/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"
#include "bacon/value.h"
#include "bacon/parser.h"

#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "valyria/list.h"
#include "valyria/hashtable.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "render.internal.h"
#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern.h"
#include "strht.h"

#include "common/hash.h"

typedef struct pattern_render_test {
  XUNITTEST;

  // context
  char * pattern;
  char * vars;

  // results
  char ** paths;
} pattern_render_test;

//
// tests
//

static void pattern_render_test_entry(xunit_test * _test)
{
  pattern_render_test * test = (typeof(test))_test;

  pattern_parser * parser = 0;
  pattern *pat;
  pattern_render_result * result = 0;
  pattern_render_fragment * fragment;
  yyu_location loc;
  hashtable expected;
  hashtable actual;
  uint16_t expected_size;
  int x;
  bacon_value *vars;
  bacon_parser * val_parser = 0;

  hashtable_initx(&expected, 0, strht_hash, strht_cmp, 0);
  hashtable_initx(&actual, 0, strht_hash, strht_cmp, 0);
  expected_size = sentinel(test->paths);

  bacon_parser_alloc(&val_parser);
  bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);

  // parse the pattern
  pattern_parser_create(&parser);
  if(lookup_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pat) != 0) {
    tracef("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
    RTA;
  }
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  pattern_render(pat, vars, &result, 0);

  // expected
  for(x = 0; x < expected_size; x++)
    hashtable_put(&expected, test->paths[x]);

  // actual
  fragment = result->fragments;
  for(x = 0; x < result->size; x++)
  {
    hashtable_put(&actual, fragment->text);
    fragment = pattern_render_fragment_next(fragment);
  }

  assert_eq_strht(&expected, &actual);
  assert_eq_d(expected_size, result->size); // no duplicates rendered

  bacon_value_free(val_parser, vars);
  pattern_parser_free(parser);
  pattern_free(pat);
  hashtable_destroy(&expected);
  hashtable_destroy(&actual);
  free(result);
  bacon_parser_free(val_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_entry : pattern_render_test_entry
  , xu_tests : (pattern_render_test*[]) {
      /* single word */
      (pattern_render_test[]) {{
          pattern : (char[]) { "bar\0" }
        , vars : ""
        , paths : (char*[]) {
              "bar"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "\"b\\x42r\"\0" }
        , vars : ""
        , paths : (char*[]) {
              "bBr"
            , 0
          }
      }}

    /* alternations */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "lib{void,xlinux,xunit,logger,narrator}\0" }
        , vars : ""
        , paths : (char*[]) {
              "libvoid"
            , "libxlinux"
            , "libxunit"
            , "liblogger"
            , "libnarrator"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a}\0" }
        , vars : ""
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a,}\0" }
        , vars : ""
        , paths : (char*[]) {
              "a"
            , ""
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{,a}\0" }
        , vars : ""
        , paths : (char*[]) {
              "a"
            , ""
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{a}}\0" }
        , vars : ""
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}}}\0" }
        , vars : ""
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}}}b\0" }
        , vars : ""
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}}b}\0" }
        , vars : ""
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}b}}\0" }
        , vars : ""
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab{c,d}\0" }
        , vars : ""
        , paths : (char*[]) {
              "abc"
            , "abd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab{c,d}e\0" }
        , vars : ""
        , paths : (char*[]) {
              "abce"
            , "abde"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{b{1,2}}\0" }
        , vars : ""
        , paths : (char*[]) {
              "ab1"
            , "ab2"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{b{c,d}e}f\0" }
        , vars : ""
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{b{c,d}}ef\0" }
        , vars : ""
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{{b,c}d}ef\0" }
        , vars : ""
        , paths : (char*[]) {
              "abdef"
            , "acdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "x{a,b}{c,d}\0" }
        , vars : ""
        , paths : (char*[]) {
              "xac"
            , "xad"
            , "xbc"
            , "xbd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a,b}{c,d}z\0" }
        , vars : ""
        , paths : (char*[]) {
              "acz"
            , "adz"
            , "bcz"
            , "bdz"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a,b}y{c,d}\0" }
        , vars : ""
        , paths : (char*[]) {
              "ayc"
            , "ayd"
            , "byc"
            , "byd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "x{a,b}y{c,d}z\0" }
        , vars : ""
        , paths : (char*[]) {
              "xaycz"
            , "xaydz"
            , "xbycz"
            , "xbydz"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a,b}{c,d}\0" }
        , vars : ""
        , paths : (char*[]) {
              "ac"
            , "ad"
            , "bc"
            , "bd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a{b,c}}\0" }
        , vars : ""
        , paths : (char*[]) {
              "ab"
            , "ac"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{foo,bar}.void.{debug,devel,final}\0" }
        , vars : ""
        , paths : (char*[]) {
              "foo.void.debug"
            , "foo.void.devel"
            , "foo.void.final"
            , "bar.void.debug"
            , "bar.void.devel"
            , "bar.void.final"
            , 0
          }
      }}

    /* classes */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "[cd]\0" }
        , vars : ""
        , paths : (char*[]) {
              "c"
            , "d"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab[cd]\0" }
        , vars : ""
        , paths : (char*[]) {
              "abc"
            , "abd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "[ab]cd\0" }
        , vars : ""
        , paths : (char*[]) {
              "acd"
            , "bcd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab[cd]ef\0" }
        , vars : ""
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab[c-f]\0" }
        , vars : ""
        , paths : (char*[]) {
              "abc"
            , "abd"
            , "abe"
            , "abf"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{[c-d]}\0" }
        , vars : ""
        , paths : (char*[]) {
              "c"
            , "d"
            , 0
          }
      }}

    /* variables */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "$a\0" }
        , vars : "a : b"
        , paths : (char*[]) {
              "b"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a$a\0" }
        , vars : "a : b"
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "${a}a\0" }
        , vars : "a : b"
        , paths : (char*[]) {
              "ba"
            , 0
          }
      }}

    /* directories */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "fab/common\0" }
        , vars : ""
        , paths : (char*[]) {
              "fab/common"
            , 0
          }
      }}

    /* NODESET_SELF */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "./fab/common\0" }
        , vars : ""
        , paths : (char*[]) {
              "./fab/common"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { ".\0" }
        , vars : ""
        , paths : (char*[]) {
              "."
            , 0
          }
      }}

    /* NODESET_SHADOW */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "//foo/bar\0" }
        , vars : ""
        , paths : (char*[]) {
              "//foo/bar"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "//{foo,bar}\0" }
        , vars : ""
        , paths : (char*[]) {
              "//foo"
            , "//bar"
            , 0
          }
      }}
    , NULL
  }
};
XUNIT_UNIT(xunit);
