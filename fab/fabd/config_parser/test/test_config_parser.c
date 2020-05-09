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

#include "config_parser.h"
#include "config.internal.h"
#include "request.h"
#include "node.h"
#include "logging.h"

#include "box.h"

typedef struct {
  XUNITTEST;

  char * text;
  config * cfg;
} config_parser_test;

static xapi config_parser_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);

  finally : coda;
}

static xapi config_parser_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

static xapi config_parser_test_entry(xunit_test * _test)
{
  enter;

  config_parser_test * test = containerof(_test, config_parser_test, xu);
  char buf[1024];

  config_parser * parser = 0;
  config * A = 0;
  config * B = 0;
  narrator * N1 = 0;
  narrator * N2 = 0;

  // arrange
  fatal(config_parser_create, &parser);

  // act - initial parse
  fatal(config_parser_parse, parser, test->text, strlen(test->text) + 2, 0, 0, &A);

  // round-trip
  fatal(narrator_growing_create, &N1);
  fatal(config_say, A, N1);

  size_t len = narrator_growing_size(N1);
  fatal(narrator_xseek, N1, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, N1, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  fatal(config_parser_parse, parser, buf, len + 2, 0, 0, &B);

  fatal(narrator_growing_create, &N2);
  fatal(config_say, B, N2);

  // roound trip
  assert_eq_w(narrator_growing_buffer(N1), narrator_growing_size(N1), narrator_growing_buffer(N2), narrator_growing_size(N2));

finally:
  fatal(config_parser_xfree, parser);
  fatal(config_xfree, A);
  fatal(config_xfree, B);
  fatal(narrator_xfree, N1);
  fatal(narrator_xfree, N2);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : config_parser_test_unit_setup
  , xu_cleanup : config_parser_test_unit_cleanup
  , xu_entry : config_parser_test_entry
  , xu_tests : (config_parser_test*[]) {
      (config_parser_test[]) {{
          text : (char[]) {
            "extern : {"
            "  /usr/lib/fab/builtin-modules"
            "  /home/todd/my/modules"
            "}\0\0"
          }
      }}
    , (config_parser_test[]) {{
          text : (char[]) {
            "{"
            " extern : {"
            "   /usr/lib/fab/builtin-modules"
            "   /home/todd/my/modules"
            " }"
            "}\0\0"
          }
      }}
   , (config_parser_test[]) {{
          text : (char[]) {
            "filesystems : {"
            "  \"/\" : {"
            "    invalidate : notify"
            "  }"
            "  /home : {"
            "    invalidate : stat"
            "  }"
            "}\0\0"
          }
      }}
   , (config_parser_test[]) {{
          text : (char[]) {
            "formula : {"
            "  capture-stdout : leading"
            "  stdout-buffer-size : 4096"
            "}\0\0"
          }
      }}
   , (config_parser_test[]) {{
          text : (char[]) {
            "formula : {"
            "  capture-stdout : leading"
            "  stdout-buffer-size : 4096"
            "  capture-stderr : leading"
            "  stderr-buffer-size : 4096"
            "  success : {"
            "    show-arguments : false"
            "    show-environment : false"
            "    show-status : false"
            "    show-stdout : false"
            "    show-stdout-limit-lines : 40"
            "    show-stdout-limit-bytes : 0"
            "    show-stderr : true"
            "    show-stderr-limit-lines : 0"
            "    show-stderr-limit-bytes : 0"
            "  }"
            "  error : {"
            "    show-arguments : true"
            "    show-environment : true"
            "    show-status : true"
            "    show-stdout : true"
            "    show-stdout-limit-lines : 0"
            "    show-stdout-limit-bytes : 0"
            "    show-stderr : true"
            "    show-stderr-limit-lines : 0"
            "    show-stderr-limit-bytes : 0"
            "  }"
            "}\0\0"
          }
      }}
    , (config_parser_test[]) {{
          text : (char[]) {
            "extern : {"
            "  /usr/lib/fab/builtin-modules"
            "  /home/todd/my/modules"
            "}"
            "filesystems : {"
            "  \"/\" : {"
            "    invalidate : notify"
            "  }"
            "  /home : {"
            "    invalidate : stat"
            "  }"
            "}"
            "build : {"
            "  concurrency : 0"
            "}"
            "formula : {"
            "  capture-stdout : leading"
            "  stdout-buffer-size : 4096"
            "  error : {"
            "    show-arguments : true"
            "    show-environment : true"
            "    show-status : true"
            "    show-stdout : false"
            "    show-stdout-limit-lines : 0"
            "    show-stdout-limit-bytes : 10"
            "    show-stderr : true"
            "    show-stderr-limit-lines : 440"
            "    show-stderr-limit-bytes : 0"
            "  }"
            "}\0\0"
          }
      }}
    , (config_parser_test[]) {{
          text : (char[]) {
            "logging : {"
            " console : {"
            "  exprs : ["
            "   UNITS "
            "  ]"
            " }"
            " logfile : {"
            "  exprs : ["
            "   UNITS BAR baz wux"
            "  ]"
            " }"
            "}\0\0"
          }
      }}
    /* var */
    , (config_parser_test[]) {{
          text : (char[]) {
            "var : {"
            " cflags : foo"
            " lflags : ["
            "   UNITS "
            " ]"
            "}\0\0"
          }
      }}
    , (config_parser_test[]) {{
          text : (char[]) {
            "var : {"
            " cflags : foo"
            " lflags = ["
            "   bar"
            " ]"
            "}\0\0"
          }
      }}
    , 0
  }
};
