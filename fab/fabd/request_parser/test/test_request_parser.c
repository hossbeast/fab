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

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "request_parser.h"
#include "request.internal.h"
#include "request.h"
#include "node.h"
#include "logging.h"

typedef struct {
  XUNITTEST;

  char * text;
} request_parser_test;

static xapi request_parser_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);

  finally : coda;
}

static xapi request_parser_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

static xapi request_parser_test_entry(xunit_test * _test)
{
  enter;

  request_parser_test * test = (typeof(test))_test;
  char buf[512];

  request_parser * parser = 0;
  request * A = 0;
  request * B = 0;
  narrator * N1 = 0;
  narrator * N2 = 0;

  // arrange
  fatal(request_parser_create, &parser);

  // act - initial parse
  fatal(request_parser_parse, parser, test->text, strlen(test->text) + 2, 0, &A);

  // round-trip
  fatal(narrator_growing_create, &N1);
  fatal(request_say, A, N1);

  size_t len = narrator_growing_size(N1);
  fatal(narrator_xseek, N1, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, N1, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  fatal(request_parser_parse, parser, buf, len + 2, 0, &B);

  fatal(narrator_growing_create, &N2);
  fatal(request_say, B, N2);

  // round-trip
  assert_eq_w(narrator_growing_buffer(N1), narrator_growing_size(N1), narrator_growing_buffer(N2), narrator_growing_size(N2));

finally:
  fatal(request_parser_xfree, parser);
  fatal(request_xfree, A);
  fatal(request_xfree, B);
  fatal(narrator_xfree, N1);
  fatal(narrator_xfree, N2);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : request_parser_test_unit_setup
  , xu_cleanup : request_parser_test_unit_cleanup
  , xu_entry : request_parser_test_entry
  , xu_tests : (request_parser_test*[]) {
      (request_parser_test[]) {{
          text : (char[]) {
            " list"
            " select : ["
              " pattern : foo/bar/main.o"
              " all"
            " ]"
            " describe"
            " invalidate"
            " reset-selection"
            " select : ["
              " module"
              " traverse : {"
                " direction : down"
                " relation : fs"
              " }"
            " ]"
            " run"
            "\0\0"
          }
      }}
    , (request_parser_test[]) {{
          text : (char[]) {
            "["
            " stage-config : { build : { concurrency : 0 } }"
            " reconfigure"
            "]\0\0"
          }
      }}
    , (request_parser_test[]) {{
          text : (char[]) {
            "["
            " stage-config : { build : { concurrency : 0 } }"
            " reconfigure"
            " select : [ pattern : foo/bar/main.o ]"
            " invalidate"
            "]\0\0"
          }
      }}
    , (request_parser_test[]) {{
          text : (char[]) {
            "goals : {"
            " build"
            " script"
            " target-direct : [ module ]"
            "}\0\0"
          }
      }}
    , (request_parser_test[]) {{
          text : (char[]) {
            "goals : {"
            " target-transitive : [ module ]"
            "}\0\0"
          }
      }}
    , 0
  }
};
