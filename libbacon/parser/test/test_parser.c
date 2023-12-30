/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "bacon/load.h"
#include "bacon/assert.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"


#include "value.h"
#include "value.h"
#include "make.h"
#include "glue.h"

#include "macros.h"
#include "common/attrs.h"

typedef struct parser_test {
  XUNITTEST;

  bacon_type container;
  char *text;
  char *expected;
  bool noexpect;

  int parse_expected;
} parser_test;

static void test_parser_setup(xunit_unit * unit)
{
  // load libraries
  bacon_load();
}

static void test_parser_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void test_parser_entry(xunit_test * _test)
{
  parser_test * test = containerof(_test, parser_test, xu);

  char buf[PATH_MAX];
  bacon_parser * parser = 0;
  bacon_value * val0 = 0;
  bacon_value * val1 = 0;
  narrator_growing *N0 = 0;
  narrator_growing *N1 = 0;
  size_t len;
  int r;

  narrator_growing_create(&N0);
  narrator_growing_create(&N1);
  bacon_parser_alloc(&parser);

  // parse
  if((r = bacon_parser_parse(parser, MMS(test->text), 0, test->container, &val0))) {
    printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
  }
  assert_eq_d(test->parse_expected, r);
  bacon_value_say_flat(val0, &N0->base);
  if(test->parse_expected) {
  } else if(test->noexpect) {
  } else if(test->expected) {
    assert_eq_s(test->expected, N0->s);
  } else {
    assert_eq_s(test->text, N0->s);
  }

  // round-trip
  if(val0)
  {
    len = N0->l;
    narrator_seek(&N0->base, 0, NARRATOR_SEEK_SET, 0);
    narrator_read(&N0->base, buf, len, 0);
    buf[len] = 0;
    if((r = bacon_parser_parse(parser, buf, len, 0, 0, &val1))) {
      printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
      RTA;
    }
  }

  assert_eq_value(val0, val1);
  /* first-pass and second-pass strings can differ, since element order in rbtree is subject to insertion order */

  bacon_value_free(parser, val0);
  bacon_value_free(parser, val1);
  bacon_parser_free(parser);
  narrator_growing_free(N0);
  narrator_growing_free(N1);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : test_parser_setup
  , xu_entry : test_parser_entry
  , xu_cleanup : test_parser_cleanup
  , xu_tests : (parser_test*[]) {
    /* primitives */
      (parser_test[]) {{ text : "true" }}
    , (parser_test[]) {{ text : "false" }}
    , (parser_test[]) {{ text : "1.42"    , expected : "1.420000" }}
    , (parser_test[]) {{ text : "-1.42"   , expected : "-1.420000" }}
    , (parser_test[]) {{ text : "-142" }}
    , (parser_test[]) {{ text : "142" }}
    , (parser_test[]) {{ text : "0x142"   , expected : "322" }}
    , (parser_test[]) {{ text : "foo" }}
    , (parser_test[]) {{ text : "foo/bar" }}
    , (parser_test[]) {{ text : "foo-bar" }}
    , (parser_test[]) {{ text : "/bar" }}
    , (parser_test[]) {{ text : "bar/" }}
    , (parser_test[]) {{ text : "/bar/" }}
    , (parser_test[]) {{ text : "bar." }}
    , (parser_test[]) {{ text : ".bar" }}
    , (parser_test[]) {{ text : ".bar." }}
    , (parser_test[]) {{ text : "bar+" }}
    , (parser_test[]) {{ text : "+bar" }}
    , (parser_test[]) {{ text : "+bar+" }}
    , (parser_test[]) {{ text : "bar-" }}
    , (parser_test[]) {{ text : "-bar" }}
    , (parser_test[]) {{ text : "-bar-" }}
    , (parser_test[]) {{ text : "-DFABIPCDIR=/var/run/fab" }}

    /* string escapes */
    , (parser_test[]) {{ text : "\"bar\""           , expected : "bar" }}
    /* hex refs */
    , (parser_test[]) {{ text : "\"ba\\x65r\""      , expected : "baer" }}
    , (parser_test[]) {{ text : "\"ba\\x90r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x00r\"" }}
    , (parser_test[]) {{ text : "\"\\x35\""         , expected : "\"5\"" }}
    , (parser_test[]) {{ text : "\"\\x53\""         , expected : "S" }}
    /* character refs */
    , (parser_test[]) {{ text : "\"\\a\"" }}
    , (parser_test[]) {{ text : "\"\\b\"" }}
    , (parser_test[]) {{ text : "\"\\t\"" }}
    , (parser_test[]) {{ text : "\"\\n\"" }}
    , (parser_test[]) {{ text : "\"\\x0a\""         , expected : "\"\\n\"" }}

    /* mappings */
    , (parser_test[]) {{ text : "true : foo" }}
    , (parser_test[]) {{ text : "false : true" }}
    , (parser_test[]) {{ text : "142 : false" }}
    , (parser_test[]) {{ text : "1.42 : 142"    , expected : "1.420000 : 142" }}
    , (parser_test[]) {{ text : "foo : 1.42"    , expected : "foo : 1.420000" }}

    , (parser_test[]) {{ text : "cflags : -g"    , expected : "cflags : -g" }}

    /* lists */
    , (parser_test[]) {{
          text : "[ 1 true 2 foo ]"
      }}
    , (parser_test[]) {{
          text : "[ [ 1 true ] 2 foo ]"
      }}
    , (parser_test[]) {{
          text : "[ 1 true [ 2 foo ] ]"
      }}
    , (parser_test[]) {{
          text : "[ 1 : true 2 : false ]"
      }}
    , (parser_test[]) {{
          text : "[ true : [ 1 2 3 ] ]"
      }}
    , (parser_test[]) {{
          text : "[ true true 1 1 ]"
      }}
    , (parser_test[]) {{
          text : "true true 1 1"
        , container : BACON_TYPE_LIST
        , expected : "[ true true 1 1 ]"
      }}
    , (parser_test[]) {{
          text : "true : true 1 : 1"
        , expected : "[ true : true 1 : 1 ]"
        , container : BACON_TYPE_LIST
      }}

    /* sets - order is not guaranteed */
    , (parser_test[]) {{
          text : "{ true true }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "{ true true 1 1 }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "{ foo true 1 2.00 }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "foo true 1 2.00"
        , container : BACON_TYPE_SET
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "foo : true 1 : 2.00"
        , container : BACON_TYPE_SET
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "foo : true 1 : 2.00 foo : true"
        , container : BACON_TYPE_SET
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "{ a : 42 }"
        , container : BACON_TYPE_SET
        , expected : "{ { a : 42 } }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "core : { foo true 1 2.00 }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "/ : { foo true 1 2.00 }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "core : { foo : { bar : baz } qux : true }"
        , noexpect : true
      }}
    , (parser_test[]) {{
          text : "core : { / : { invalidate : notify } /mnt/remote : { invalidate : stat } }"
        , noexpect : true
      }}

    /* real-world cases */
    , (parser_test[]) {{
          text : ""
            "bisonflags   : \"--warnings=error\"\n"
            "cflags       : -g\n"
            "cflags.eapi  : [ -DEAPI -DXAPI_MODE_ERRORCODE ]\n"
        , container : BACON_TYPE_SET
        , noexpect : true
      }}

    , (parser_test[]) {{
          text : ""
            "bisonflags   : --warnings=error\n"
            "CFLAGS       : {\n"
            "  -g -m64 -Wall -Werror -Wno-format-truncation -D_GNU_SOURCE\n"
            "  -fno-builtin-log -Wmissing-prototypes \"-fdiagnostics-color=always\" -Wno-misleading-indentation -Wno-trigraphs\n"
            "  -DFABIPCDIR=/var/run/fab\n"
            "  -include/home/todd/fab/include/tracef.h\n"
            "  -include/home/todd/fab/include/hexdump.h\n"
            "}\n"
        , container : BACON_TYPE_SET
        , noexpect : true
      }}

    /* error cases */
    /* parsing errors should not result in leaked values */
    , (parser_test[]) {{
          text : "asdf :: asdf"
        , parse_expected : 1
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
