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

  char * text;
  bacon_type container;

  int expected;
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

  char buf[512];
  bacon_parser * parser = 0;
  bacon_value * val0 = 0;
  bacon_value * val1 = 0;
  narrator_growing * N0 = 0;
  narrator_growing * N1 = 0;
  size_t len;
  int r;

  narrator_growing_create(&N0);
  narrator_growing_create(&N1);

  bacon_parser_alloc(&parser);

printf("text>>\n");
printf("%s\n", test->text);
printf("<<\n");
hexdump(test->text, strlen(test->text));

  // parse
  r = bacon_parser_parse(parser, MMS(test->text), 0, test->container, &val0);
  if(r) {
    printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
  }
  assert_eq_d(test->expected, r);
  bacon_value_say(val0, &N0->base);
printf("first-pass>>\n");
printf("%.*s\n", (int)N0->l, N0->s);
printf("<<\n");
hexdump(N0->s, N0->l);

  // round-trip
  if(val0)
  {
    len = N0->l;
    narrator_seek(&N0->base, 0, NARRATOR_SEEK_SET, 0);
    narrator_read(&N0->base, buf, len, 0);
    buf[len] = 0;
    r = bacon_parser_parse(parser, buf, len, 0, 0, &val1);
    if(r) {
      printf("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
    }
    assert_eq_d(0, r);
  }
  bacon_value_say(val1, &N1->base);
printf("second-pass>>\n");
printf("%.*s\n", (int)N1->l, N1->s);
printf("<<\n");
hexdump(N0->s, N0->l);

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
    , (parser_test[]) {{ text : "1.42" }}
    , (parser_test[]) {{ text : "-1.42" }}
    , (parser_test[]) {{ text : "-142" }}
    , (parser_test[]) {{ text : "142" }}
    , (parser_test[]) {{ text : "0x142" }}
    , (parser_test[]) {{ text : "foo" }}
    , (parser_test[]) {{ text : "foo/bar" }}
    , (parser_test[]) {{ text : "foo-bar" }}
    , (parser_test[]) {{ text : "/bar" }}
    , (parser_test[]) {{ text : "bar/" }}
    , (parser_test[]) {{ text : "bar-" }}

    /* string escapes */
    , (parser_test[]) {{ text : "\"bar\"" }}
    , (parser_test[]) {{ text : "\"ba\\x65r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x90r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x00r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x{0}r\"" }}
    , (parser_test[]) {{ text : "\"ba\\x{00}r\"" }}
    , (parser_test[]) {{ text : "\"\\x35\"" }}

    /* mappings */
    , (parser_test[]) {{ text : "true : foo" }}
    , (parser_test[]) {{ text : "false : true" }}
    , (parser_test[]) {{ text : "142 : false" }}
    , (parser_test[]) {{ text : "1.42 : 142" }}
    , (parser_test[]) {{ text : "foo : 1.42" }}

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
      }}
    , (parser_test[]) {{
          text : "true : true 1 : 1"
        , container : BACON_TYPE_LIST
      }}

    /* sets */
    , (parser_test[]) {{
          text : "{ true true }"
      }}
    , (parser_test[]) {{
          text : "{ true true 1 1 }"
      }}
    , (parser_test[]) {{
          text : "{ foo true 1 2.00 }"
      }}
    , (parser_test[]) {{
          text : "foo true 1 2.00"
        , container : BACON_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "foo : true 1 : 2.00"
        , container : BACON_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "foo : true 1 : 2.00 foo : true"
        , container : BACON_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "{ a : 42 }"
        , container : BACON_TYPE_SET
      }}
    , (parser_test[]) {{
          text : "core : { foo true 1 2.00 }"
      }}
    , (parser_test[]) {{
          text : "/ : { foo true 1 2.00 }"
      }}
    , (parser_test[]) {{
          text : "core : { foo : { bar : baz } qux : true }"
      }}
    , (parser_test[]) {{
          text : "core : { / : { invalidate : notify } /mnt/remote : { invalidate : stat } }"
      }}

    /* other */
    , (parser_test[]) {{
          text : ""
            "bisonflags   : \"--warnings=error\"\n"
            "cflags       : -g\n"
            "cflags.eapi  : [ -DEAPI -DXAPI_MODE_ERRORCODE ]\n"
        , container : BACON_TYPE_SET
      }}

    /* parsing errors should not result in leaked values */
    , (parser_test[]) {{
          text : "asdf :: asdf"
        , expected : 1
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
