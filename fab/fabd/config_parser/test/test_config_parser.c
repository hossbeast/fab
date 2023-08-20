/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"

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
  bacon_load();
}

static void config_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
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
  narrator_seek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  config_parser_parse(parser, buf, len + 2, 0, 0, &B);

  narrator_growing_create(&N2);
  config_say(B, &N2->base);

  // roound trip
  assert_eq_w(N1->s, N1->l, N2->s, N2->l);

  config_parser_xfree(parser);
  config_xfree(A);
  config_xfree(B);
  narrator_growing_free(N1);
  narrator_growing_free(N2);
}

//
// manifest
//

static xunit_unit xunit = {
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
XUNIT_UNIT(xunit);
