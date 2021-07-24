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

#include "lorien/load.h"
#include "yyutil/load.h"
#include "value/load.h"

#include "valyria/map.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "config_parser.h"
#include "config.internal.h"
#include "request.h"
#include "fsent.h"

#include "yyutil/box.h"

typedef struct {
  XUNITTEST;

  char * text;
  config * cfg;
} config_parser_test;

static void config_parser_test_unit_setup(xunit_unit * unit)
{
  yyutil_load();
  value_load();
  logging_finalize();
}

static void config_parser_test_unit_cleanup(xunit_unit * unit)
{
  yyutil_unload();
  value_unload();
}

static void config_parser_test_entry(xunit_test * _test)
{
  config_parser_test * test = containerof(_test, config_parser_test, xu);
  char buf[1024];

  config_parser * parser = 0;
  configblob * A = 0;
  configblob * B = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;

  // arrange
  config_parser_create(&parser);

  // act - initial parse
  config_parser_parse(parser, test->text, strlen(test->text) + 2, 0, 0, &A);

  // round-trip
  narrator_growing_create(&N1);
  config_say(A, &N1->base);

  size_t len = N1->l;
  narrator_xseek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_xread(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  config_parser_parse(parser, buf, len + 2, 0, 0, &B);

  narrator_growing_create(&N2);
  config_say(B, &N2->base);

  // roound trip
  assert_eq_w(N1->s, N1->l, N2->s, N2->l);

finally:
  config_parser_xfree(parser);
  config_xfree(A);
  config_xfree(B);
  narrator_growing_free(N1);
  narrator_growing_free(N2);
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
            "walker : { include : {"
            "  /usr/lib/fab/builtin-goats"
            "  /home/todd/my/goats"
            "}}\0\0"
          }
      }}
    , (config_parser_test[]) {{
          text : (char[]) {
            "{"
            " walker : { include : {"
            "   /usr/lib/fab/builtin-goats"
            "   /home/todd/my/goats"
            " }}"
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
            "walker : { include : {"
            "  /usr/lib/fab/builtin-goats"
            "  /home/todd/my/goats"
            "}}"
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
            "\0\0"
          }
      }}
    , 0
  }
};
