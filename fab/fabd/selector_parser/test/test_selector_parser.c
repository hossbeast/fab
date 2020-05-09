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
#include "lorien/load.h"
#include "yyutil/load.h"
#include "logger/load.h"
#include "value/load.h"

#include "valyria/map.h"
#include "yyutil/grammar.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "selector_parser.h"
#include "selector.internal.h"
#include "request.h"
#include "node.h"
#include "logging.h"

typedef struct {
  XUNITTEST;

  char * text;
  char * expected;
} selector_parser_test;

static xapi selector_parser_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);

  finally : coda;
}

static xapi selector_parser_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

static xapi selector_parser_test_entry(xunit_test * _test)
{
  enter;

  selector_parser_test * test = (typeof(test))_test;
  char buf[512];

  selector_parser * parser = 0;
  selector * A = 0;
  selector * B = 0;
  narrator * N1 = 0;
  narrator * N2 = 0;
  yyu_location loc;

  // arrange
  fatal(selector_parser_create, &parser);

  // act - initial parse
  fatal(selector_parser_parse_partial, parser, test->text, strlen(test->text) + 2, 0, 0, &loc, &A);
  assert_eq_u32(strlen(test->text), loc.l);

  fatal(narrator_growing_create, &N1);
  fatal(selector_say, A, N1);

  // re-parse
  size_t len = narrator_growing_size(N1);
  fatal(narrator_xseek, N1, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, N1, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  fatal(selector_parser_parse_partial, parser, buf, len + 2, 0, 0, &loc, &B);
  assert_eq_u32(len, loc.l);

  fatal(narrator_growing_create, &N2);
  fatal(selector_say, B, N2);

  assert_eq_selector(A, B);

  // textual equivalence
  assert_eq_w(narrator_growing_buffer(N1), narrator_growing_size(N1), narrator_growing_buffer(N2), narrator_growing_size(N2));

  const char * expected = test->text;
  expected = test->expected ?: expected;
  assert_eq_w(expected, strlen(expected), narrator_growing_buffer(N1), narrator_growing_size(N1));

finally:
  fatal(selector_parser_xfree, parser);
  selector_free(A);
  selector_free(B);
  fatal(narrator_xfree, N1);
  fatal(narrator_xfree, N2);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : selector_parser_test_unit_setup
  , xu_cleanup : selector_parser_test_unit_cleanup
  , xu_entry : selector_parser_test_entry
  , xu_tests : (selector_parser_test*[]) {
      (selector_parser_test[]) {{
          text : (char[]) {
            "pattern : foo/bar/main.o\0\0"
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
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    direction : up\n"
             "  }\n"
             "  pattern : foo/bar/main.o\n"
             "]\0\0"
          }
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "union : [\n"
             "  pattern : foo/bar/main.o\n"
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
             "    relation : strong\n"
             "    file-type : dir\n"
             "  }\n"
             "  pattern : foo/bar/main.o\n"
             "]\0\0\n"
          }
        /* default min-distance of 1 is omitted */
        , expected :
             "sequence : [\n"
             "  traverse : {\n"
             "    max-distance : 1\n"
             "    direction : down\n"
             "    relation : strong\n"
             "    file-type : dir\n"
             "  }\n"
             "  pattern : foo/bar/main.o\n"
             "]\0\0\n"
      }}
    , (selector_parser_test[]) {{
          text : (char[]) {
             "sequence : [\n"
             "  traverse : {\n"
             "    min-distance : 1\n"
             "    max-distance : 2\n"
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
      }}
    , 0
  }
};
