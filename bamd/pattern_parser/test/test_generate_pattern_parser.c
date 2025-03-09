/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "lorien/load.h"
#include "bacon/load.h"
#include "bacon/builder.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern.h"

#include "macros.h"

typedef struct pattern_parser_test {
  XUNITTEST;

  char * text;
  char * expected;
} pattern_parser_test;

static void pattern_parser_unit_setup(xunit_unit * unit)
{
  // load libraries
  lorien_load();
  bacon_load();

  // logging
}

static void pattern_parser_unit_cleanup(xunit_unit * unit)
{
  lorien_unload();
  bacon_unload();
}

//
// tests
//

static void pattern_parser_test_entry(xunit_test * _test)
{
  generate_pattern *A = 0;
  generate_pattern *B = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  pattern_parser * parser = 0;
  char buf[PATH_MAX];
  size_t len;
  yyu_location loc;
  const char * expected;
  const char * actual;

  pattern_parser_test *test = containerof(_test, pattern_parser_test, xu);

  expected = test->expected ?: test->text;

  narrator_growing_create(&N1);
  narrator_growing_create(&N2);
  pattern_parser_create(&parser);

  // round trip parse
  if(generate_pattern_parse_partial(parser, test->text, strlen(test->text) + 2, "-test-", 0, &loc, &A) != 0) {
    tracef("%.*s", (int)parser->error_len, parser->error);
  }
  assert_eq_zu(strlen(test->text), loc.l);

  pattern_say(&A->pattern, &N1->base);
  len = N1->l;
  narrator_seek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;

  if(generate_pattern_parse_partial(parser, buf, len + 2, "-test-", 0, &loc, &B) != 0) {
    tracef("%.*s", (int)parser->error_len, parser->error);
  }

  assert_eq_zu(strlen(expected), loc.l);
  pattern_say(&B->pattern, &N2->base);

  // round-trip
  assert_eq_w(N1->s, N1->l, N2->s, N2->l);

  // matches expected
  actual = N1->s;
  assert_eq_w(expected, strlen(expected), actual, N1->l);

  pattern_parser_free(parser);
  narrator_growing_free(N1);
  narrator_growing_free(N2);
  pattern_free(&A->pattern);
  pattern_free(&B->pattern);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : pattern_parser_unit_setup
  , xu_cleanup : pattern_parser_unit_cleanup
  , xu_entry : pattern_parser_test_entry
  , xu_tests : (pattern_parser_test*[]) {
    /* basic */
      (pattern_parser_test[]) {{
        text : (char[]) { "program.\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "cflags.void.debug\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "xunit\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/BC/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "B.C\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/B.C\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "B.C/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/B.C/D\0" }
      }}
    /* axes */
    , (pattern_parser_test[]) {{
        text : (char[]) { "//module/a/{b,c}/d\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//modules/[b-d]\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//module/[b-d]\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//module/boats\0" }
      }}
    /* alternations */
    , (pattern_parser_test[]) {{
        text : (char[]) { ".tab.{c,h}\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { ".tokens.{c,h}\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { ".lex.{c,h}\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { ".states.{c,h}\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/{B,C}/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/X{B,C}/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/{B,C}Y/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/X{B,C}Y/D\0" }
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "A/{B,}/D\0" }
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "A/{,B}/D\0" }
        , expected : "A/{B,}/D"
      }}
    /* classes */
    , (pattern_parser_test[]) {{
        text : (char[]) { "[a-z0-9]\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/[a-z0-9]/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/[!a-z0-9]/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/[ABC0-9XY]/D\0" }
      }}
    /* variables */
    , (pattern_parser_test[]) {{
          text : (char[]) { "A/$A/D\0" }
        , expected : "A/$A/D"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "A/${A}/D\0" }
        , expected : "A/$A/D"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/$7/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/$foo/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { ".A\0" }
      }}
    /* capture groups / numbered variables */
    , (pattern_parser_test[]) {{
          text : (char[]) { "$1.o\0" }
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "${1}.o\0" }
        , expected : "$1.o"
      }}
    /* initial nodesets / aka sysvars */
    , (pattern_parser_test[]) {{
        text : (char[]) { "$bm_ipcdir/foo\0" }
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "$bm_anchor_rel\0" }
        , expected : "$^A"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "$bm_homedir/foo\0" }
      }}
    /* autovar variables */
    , (pattern_parser_test[]) {{
        text : (char[]) { "$bm_module_id\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "$bm_bamd_id\0" }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
