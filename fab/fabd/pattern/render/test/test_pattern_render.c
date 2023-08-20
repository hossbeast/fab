/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"

#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "render.internal.h"
#include "pattern_parser.h"
#include "pattern.h"

#include "common/hash.h"

typedef struct pattern_render_test {
  XUNITTEST;

  // context
  char * pattern;

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
  pattern * pattern = 0;
  pattern_render_result * result = 0;
  pattern_render_fragment * fragment;
  yyu_location loc;
  set * expected = 0;
  set * actual = 0;
  uint16_t expected_size;
  int x;

  set_createx(&expected, 0, hash32, memncmp, 0);
  set_createx(&actual, 0, hash32, memncmp, 0);
  expected_size = sentinel(test->paths);

  // parse the pattern
  pattern_parser_create(&parser);
  lookup_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern);
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  pattern_render(pattern, &result);

  // expected
  for(x = 0; x < expected_size; x++)
    set_put(expected, MMS(test->paths[x]));

  // actual
  fragment = result->fragments;
  for(x = 0; x < result->size; x++)
  {
    set_put(actual, fragment->text, fragment->len);
    fragment = pattern_render_fragment_next(fragment);
  }

  assert_eq_set(expected, actual);
  assert_eq_d(expected_size, result->size); // no duplicates rendered

  pattern_parser_xfree(parser);
  pattern_free(pattern);
  set_xfree(expected);
  set_xfree(actual);
  wfree(result);
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
        , paths : (char*[]) {
              "bar"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "\"b\\x42r\"\0" }
        , paths : (char*[]) {
              "bBr"
            , 0
          }
      }}

    /* alternations */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "lib{void,xlinux,xunit,logger,narrator}\0" }
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
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{a,}\0" }
        , paths : (char*[]) {
              "a"
            , ""
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{,a}\0" }
        , paths : (char*[]) {
              "a"
            , ""
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{a}}\0" }
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}}}\0" }
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}}}b\0" }
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}}b}\0" }
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{{{a}b}}\0" }
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab{c,d}\0" }
        , paths : (char*[]) {
              "abc"
            , "abd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab{c,d}e\0" }
        , paths : (char*[]) {
              "abce"
            , "abde"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{b{1,2}}\0" }
        , paths : (char*[]) {
              "ab1"
            , "ab2"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{b{c,d}e}f\0" }
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{b{c,d}}ef\0" }
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "a{{b,c}d}ef\0" }
        , paths : (char*[]) {
              "abdef"
            , "acdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "x{a,b}{c,d}\0" }
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
        , paths : (char*[]) {
              "ab"
            , "ac"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "{foo,bar}.void.{debug,devel,final}\0" }
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
        , paths : (char*[]) {
              "c"
            , "d"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab[cd]\0" }
        , paths : (char*[]) {
              "abc"
            , "abd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "[ab]cd\0" }
        , paths : (char*[]) {
              "acd"
            , "bcd"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab[cd]ef\0" }
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "ab[c-f]\0" }
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
        , paths : (char*[]) {
              "c"
            , "d"
            , 0
          }
      }}

  /* directories */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "fab/common\0" }
        , paths : (char*[]) {
              "fab/common"
            , 0
          }
      }}

  /* NODESET_SELF */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "./fab/common\0" }
        , paths : (char*[]) {
              "./fab/common"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { ".\0" }
        , paths : (char*[]) {
              "."
            , 0
          }
      }}

  /* NODESET_SHADOW */
    , (pattern_render_test[]) {{
          pattern : (char[]) { "//foo/bar\0" }
        , paths : (char*[]) {
              "//foo/bar"
            , 0
          }
      }}
    , (pattern_render_test[]) {{
          pattern : (char[]) { "//{foo,bar}\0" }
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
