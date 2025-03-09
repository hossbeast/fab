/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "bacon/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern.h"

#include "macros.h"

typedef struct pattern_parser_test {
  XUNITTEST;

  char *text;
  char *expected;
} pattern_parser_test;

//
// public
//

static void pattern_parser_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void pattern_parser_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

//
// tests
//

static void pattern_parser_test_entry(xunit_test * _test)
{
  path_pattern *pat = 0;
  pattern_parser * parser = 0;
  yyu_location loc;
  char actual[512];
  const char *expected;
  narrator_fixed fixed;
  int r;

  pattern_parser_test *test = containerof(_test, pattern_parser_test, xu);

  expected = test->expected ?: test->text;

  pattern_parser_create(&parser);

  /* act */
  if((r = path_pattern_parse_partial(parser, test->text, strlen(test->text) + 2, "-test-", 0, &loc, &pat)) != 0) {
    tracef("%.*s", (int)parser->error_len, parser->error);
  }
  assert_eq_d(0, r);
  assert_eq_zu(strlen(test->text), loc.l);

  narrator_fixed_init(&fixed, actual, sizeof(actual));
  pattern_say(&pat->pattern, &fixed.base);

  // matches expected
  assert_eq_w(expected, strlen(expected), fixed.s, fixed.l);

  pattern_free(&pat->pattern);
  pattern_parser_free(parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : pattern_parser_unit_setup
  , xu_cleanup : pattern_parser_unit_cleanup
  , xu_entry : pattern_parser_test_entry
  , xu_tests : (pattern_parser_test*[]) {
  /* all path patterns are bound */
  /* basic */
      (pattern_parser_test[]) {{
        text : (char[]) { "./cflags.void.debug\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./xunit\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "../A/BC/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "../../B.C\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "././A/B.C\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./B.C/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./A/B.C/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./A/B.C/D\0" }
      }}
  /* absolute shadow */
    , (pattern_parser_test[]) {{
        text : (char[]) { "//module/a/b/d\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//shadow\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//shadow/boats\0" }
      }}
  /* qualifiers */
    , (pattern_parser_test[]) {{
        text : (char[]) { "./abcd+(/)\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./abcd~(/F)\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./abcd~(.)\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./abcd+(.)\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./abcd+(@./)\0" }
      }}
  /* variables */
    , (pattern_parser_test[]) {{
        text : (char[]) { "./$CONFIG_VARIANT\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./a$CONFIG_VARIANT\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./${CONFIG_VARIANT}\0" }
      , expected : "./$CONFIG_VARIANT"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./a${CONFIG_VARIANT}\0" }
      , expected : "./a$CONFIG_VARIANT"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./${CONFIG_VARIANT}a\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./a${CONFIG_VARIANT}a\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./$99\0" }
      , expected : "./${99}"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./9$99\0" }
      , expected : "./9${99}"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./${99}\0" }
      , expected : "./${99}"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./9${99}\0" }
      , expected : "./9${99}"
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./${99}9\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "./9${99}9\0" }
      }}
    /* global sysvars nodesets */
    , (pattern_parser_test[]) {{
        text : (char[]) { "$bm_ipcdir\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "$bm_ipcdir/foo\0" }
      }}
    /* segment sysvars */
    , (pattern_parser_test[]) {{
        text : (char[]) { "foo/$bm_bamd_id/foo\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "foo/${bm_bamd_id}foo/bar\0" }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
