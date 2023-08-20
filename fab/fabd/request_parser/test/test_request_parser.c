/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"

#include "valyria/map.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "request_parser.h"
#include "request.internal.h"
#include "request.h"
#include "fsent.h"

typedef struct {
  XUNITTEST;

  char * text;
} request_parser_test;

static void request_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void request_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void request_parser_test_entry(xunit_test * _test)
{
  request_parser_test * test = (typeof(test))_test;
  char buf[512];

  request_parser * parser = 0;
  request A = {};
  request B = {};
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;

  // arrange
  request_parser_create(&parser);

  // act - initial parse
  request_parser_parse(parser, test->text, strlen(test->text) + 2, 0, &A);

  // round-trip
  narrator_growing_create(&N1);
  request_say(&A, &N1->base);

  size_t len = N1->l;
  narrator_seek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  request_parser_parse(parser, buf, len + 2, 0, &B);

  narrator_growing_create(&N2);
  request_say(&B, &N2->base);

  // round-trip
  assert_eq_w(N1->s, N1->l, N2->s, N2->l);

  request_parser_xfree(parser);
  request_destroy(&A);
  request_destroy(&B);
  narrator_growing_free(N1);
  narrator_growing_free(N2);
}

//
// manifest
//

static xunit_unit xunit = {
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
                " graph : fs"
              " }"
            " ]"
            " run"
            "\0\0"
          }
      }}
    , (request_parser_test[]) {{
          text : (char[]) {
            "["
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
XUNIT_UNIT(xunit);
