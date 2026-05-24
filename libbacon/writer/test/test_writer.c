/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/load.h"

#include "xunit.h"
#include "narrator.h"

#include "writer.h"


typedef struct writer_test {
  XUNITTEST;
} writer_test;

//
// public
//

static void bacon_writer_unit_setup(xunit_unit * unit)
{
  // load libraries
  bacon_load();
}

static void bacon_writer_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

//
// tests
//

static void test_basic(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);

  bacon_writer_open(writer, g_narrator_stdout);
  bacon_writer_push_list(writer);
    bacon_writer_string(writer, "foo+88");

    bacon_writer_push_set(writer);
    bacon_writer_bool(writer, true);
    bacon_writer_bool(writer, false);
    bacon_writer_pop_set(writer);

    bacon_writer_push_set(writer);
    bacon_writer_bool(writer, false);
    bacon_writer_bool(writer, true);
    bacon_writer_pop_set(writer);

    bacon_writer_push_set(writer);
      bacon_writer_bool(writer, false);
      bacon_writer_string(writer, "not-escaped");
      bacon_writer_string(writer, "noescape");
    bacon_writer_pop_set(writer);

    bacon_writer_push_set(writer);
      bacon_writer_push_mapping(writer);
        bacon_writer_string(writer, "foo+103");
        bacon_writer_push_set(writer);
          bacon_writer_mapping_string_uint(writer, "foo+105", 0xabcdabcdabcdULL);
          bacon_writer_mapping_string_int(writer, "foo+106", -1234512345123451234);
          bacon_writer_mapping_string_bool(writer, "foo/107", true);
        bacon_writer_pop_set(writer);
      bacon_writer_pop_mapping(writer);
    bacon_writer_pop_set(writer);

  bacon_writer_pop_list(writer);

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_string(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer writer_stor;
  bacon_writer *writer = &writer_stor;
  bacon_writer_init(writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_string(writer, "foo+88");

  narrator_says(g_narrator_stdout, "\n");
  bacon_writer_destroy(writer);
}

static void test_set(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_push_set(writer);
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, "path");
  bacon_writer_string(writer, "foo/bar");
  bacon_writer_pop_mapping(writer);
  bacon_writer_mapping_string_float(writer, "foo", 42);
  bacon_writer_pop_set(writer);

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_set_comments(xunit_test * _test)
{
  writer_test *test = (typeof(test))_test;

  bacon_writer *writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, "top");
  bacon_writer_push_set(writer); bacon_writer_comments(writer, "push-top");
    bacon_writer_push_mapping(writer);
    bacon_writer_string(writer, "path");
    bacon_writer_string(writer, "foo/bar");
    bacon_writer_pop_mapping(writer); bacon_writer_comments(writer, "end-path");
    bacon_writer_mapping_string_float(writer, "foo", 42); bacon_writer_comments(writer, "end-foo");
  bacon_writer_pop_set(writer); bacon_writer_comments(writer, "end-topt");

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_set_set_top(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, "path");
  bacon_writer_string(writer, "foo/bar");
  bacon_writer_pop_mapping(writer);
  bacon_writer_mapping_string_float(writer, "foo", 42);

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_set_embedded(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_push_set(writer);
    bacon_writer_push_mapping(writer);
      bacon_writer_string(writer, "container");
      bacon_writer_push_set(writer);
      bacon_writer_push_mapping(writer);
        bacon_writer_string(writer, "path");
        bacon_writer_string(writer, "foo/bar");
      bacon_writer_pop_mapping(writer);
      bacon_writer_pop_set(writer);
    bacon_writer_pop_mapping(writer);
  bacon_writer_pop_set(writer);

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_list(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_push_list(writer);
    bacon_writer_bool(writer, true);
    bacon_writer_bool(writer, false);
    bacon_writer_bool(writer, true);

    bacon_writer_mapping_string_float(writer, "foo", 42);
    bacon_writer_push_mapping(writer);
      bacon_writer_string(writer, "path");
      bacon_writer_string(writer, "foo/bar");
    bacon_writer_pop_mapping(writer);
    bacon_writer_string(writer, "foo/bar");
    bacon_writer_string(writer, "foo/bar");
  bacon_writer_pop_list(writer);

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_list_list_top(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_bool(writer, true);
  bacon_writer_bool(writer, false);
  bacon_writer_bool(writer, true);
  bacon_writer_mapping_string_float(writer, "foo", 42);
  bacon_writer_push_mapping(writer);
    bacon_writer_string(writer, "path");
    bacon_writer_string(writer, "foo/bar");
  bacon_writer_pop_mapping(writer);

  bacon_writer_bool(writer, true);
  bacon_writer_bool(writer, false);

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

static void test_list_comments(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  bacon_writer_push_list(writer); bacon_writer_comments(writer, "list-push");
    bacon_writer_bool(writer, true); bacon_writer_comments(writer, "true");
    bacon_writer_bool(writer, false); bacon_writer_comments(writer, "false");
    bacon_writer_bool(writer, true); bacon_writer_comments(writer, "true");

    bacon_writer_mapping_string_float(writer, "foo", 42); bacon_writer_comments(writer, "mapping-push");
    bacon_writer_push_mapping(writer);
      bacon_writer_string(writer, "path");
      bacon_writer_string(writer, "foo/bar");
    bacon_writer_pop_mapping(writer); bacon_writer_comments(writer, "mapping-pop");
    bacon_writer_string(writer, "foo/bar");
    bacon_writer_string(writer, "foo/bar");
  bacon_writer_pop_list(writer); bacon_writer_comments(writer, "list-pop");

  narrator_says(g_narrator_stdout, "\n");

  bacon_writer_free(writer);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : bacon_writer_unit_setup
  , xu_cleanup : bacon_writer_unit_cleanup
  , xu_tests : (void*)(writer_test*[]) {
      (writer_test[]){{ xu_entry : test_basic }}
    , (writer_test[]){{ xu_entry : test_string }}
    , (writer_test[]){{ xu_entry : test_set }}
    , (writer_test[]){{ xu_entry : test_set_comments }}
    , (writer_test[]){{ xu_entry : test_set_set_top }}
    , (writer_test[]){{ xu_entry : test_set_embedded }}
    , (writer_test[]){{ xu_entry : test_list }}
    , (writer_test[]){{ xu_entry : test_list_list_top }}
    , (writer_test[]){{ xu_entry : test_list_comments }}
    , 0
  }
};
XUNIT_UNIT(xunit);
