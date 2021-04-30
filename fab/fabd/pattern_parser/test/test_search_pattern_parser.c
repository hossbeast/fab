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
#include "xapi/errtab.h"
#include "xlinux/xstdlib.h"

#include "value/load.h"
#include "yyutil/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "logger.h"
#include "logger/category.h"

#include "pattern_parser.h"
#include "pattern.h"
#include "PATTERN.errtab.h"
#include "logging.h"

#include "macros.h"

typedef struct pattern_parser_test {
  XUNITTEST;

  char * text;
  char * expected;
} pattern_parser_test;

//
// public
//

static xapi pattern_parser_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(yyutil_load);
  fatal(value_load);

  // logging
  fatal(logging_finalize);

  finally : coda;
}

static xapi pattern_parser_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

//
// tests
//

static xapi pattern_parser_test_entry(xunit_test * _test)
{
  enter;

  pattern * A = 0;
  pattern * B = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  pattern_parser * parser = 0;
  char buf[512];
  size_t len;
  yyu_location loc;
  const char * expected;

  pattern_parser_test *test = containerof(_test, pattern_parser_test, xu);

  expected = test->expected ?: test->text;

  fatal(narrator_growing_create, &N1);
  fatal(narrator_growing_create, &N2);
  fatal(pattern_parser_create, &parser);

  // round trip parse
  fatal(search_pattern_parse_partial, parser, test->text, strlen(test->text) + 2, "-test-", 0, &loc, &A);
  assert_eq_zu(strlen(test->text), loc.l);
  fatal(pattern_say, A, &N1->base);

  len = N1->l;
  fatal(narrator_xseek, &N1->base, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, &N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;

  fatal(search_pattern_parse_partial, parser, buf, len + 2, "-test-", 0, &loc, &B);
  assert_eq_zu(len, loc.l);
  fatal(pattern_say, B, &N2->base);

  // round-trip
  assert_eq_w(N1->s, N1->l, N2->s, N2->l);

  // matches expected
  const char * actual = N1->s;
  assert_eq_w(expected, strlen(expected), actual, N1->l);

finally:
  fatal(pattern_parser_xfree, parser);
  fatal(narrator_growing_free, N1);
  fatal(narrator_growing_free, N2);
  pattern_free(A);
  pattern_free(B);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : pattern_parser_unit_setup
  , xu_cleanup : pattern_parser_unit_cleanup
  , xu_entry : pattern_parser_test_entry
  , xu_tests : (pattern_parser_test*[]) {
  /* basic */
      (pattern_parser_test[]) {{
        text : (char[]) { "program.?\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "cflags.xapi.debug\0" }
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
  /* star */
    , (pattern_parser_test[]) {{
        text : (char[]) { "*.y\0" }
      }}
  /* axes */
    , (pattern_parser_test[]) {{
        text : (char[]) { "//module/a/{b,c}/d\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//modules/[b-d]\0" }
      }}
#if 0
    , (pattern_parser_test[]) {{
          text : (char[]) { "//fs:down/bar\0" }
        , expected : "*/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//fs:down+foo/bar\0" }
        , expected : "foo/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//fs:self-or-below/bar\0" }
        , expected : "//fs:self-or-below/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//dirs:self-or-below/bar\0" }
        , expected : "**/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//dirs:self-or-below+foo/bar\0" }
        , expected : "**+foo/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//dirs:self-or-below~foo/bar\0" }
        , expected : "**~foo/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//fs:down~foo/bar\0" }
        , expected : "*~foo/bar"
      }}
    , (pattern_parser_test[]) {{
          text : (char[]) { "//module//requires:self-or-below/foo\0" }
      }}
#endif
    , (pattern_parser_test[]) {{
          text : (char[]) { "*+foo/bar\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "*~foo/bar\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "**/bar\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "**~test/bar\0" }
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
      , expected : "A/{B,}/D"
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
  /* groups */
    , (pattern_parser_test[]) {{
        text : (char[]) { "**/(*).errtab\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "([ABC0-9XY])\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "A/(?<foo>[ABC0-9XY])/D\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "([ABC0-9XY])~baz\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "(([ABC0-9XY])~baz)+baz\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "(a)+(b)~(c)\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "([abc]~b)+(b)~(c)\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "(([abc]~b)+b)+(b)~(c)\0" }
      }}
  /* shadow fs */
    , (pattern_parser_test[]) {{
        text : (char[]) { "//shadow\0" }
      }}
    , (pattern_parser_test[]) {{
        text : (char[]) { "//shadow/boats\0" }
      }}
    , 0
  }
};
