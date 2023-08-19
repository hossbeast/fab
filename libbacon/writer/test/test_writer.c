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

#include "xlinux/xstdlib.h"

#include "bacon/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"


#include "bacon.h"
#include "assert.internal.h"
#include "writer.internal.h"
#include "store.internal.h"

#include "macros.h"
#include "common/attrs.h"

typedef struct writer_test {
  XUNITTEST;
} writer_test;

//
// public
//

static void value_writer_unit_setup(xunit_unit * unit)
{
  // load libraries
  bacon_load();
}

static void value_writer_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

//
// tests
//

static void test_basic(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  value_writer_create(&writer);

  value_writer_open(writer, g_narrator_stdout);
  value_writer_push_list(writer);
    value_writer_string(writer, "foo+88");

    value_writer_push_set(writer);
    value_writer_bool(writer, true);
    value_writer_bool(writer, false);
    value_writer_pop_set(writer);

    value_writer_push_set(writer);
    value_writer_bool(writer, false);
    value_writer_bool(writer, true);
    value_writer_pop_set(writer);

    value_writer_push_set(writer);
      value_writer_bool(writer, false);
      value_writer_string(writer, "not-escaped");
      value_writer_string(writer, "noescape");
    value_writer_pop_set(writer);

    value_writer_push_set(writer);
      value_writer_push_mapping(writer);
        value_writer_string(writer, "foo+103");
        value_writer_push_set(writer);
          value_writer_mapping_string_uint(writer, "foo+105", 0xabcdabcdabcdULL);
          value_writer_mapping_string_int(writer, "foo+106", -1234512345123451234);
          value_writer_mapping_string_bool(writer, "foo/107", true);
        value_writer_pop_set(writer);
      value_writer_pop_mapping(writer);
    value_writer_pop_set(writer);

  value_writer_pop_list(writer);

  narrator_says(g_narrator_stdout, "\n");

  value_writer_xfree(writer);
}

static void test_set(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

//printf("\n++ %s\n", __FUNCTION__);

  value_writer * writer = 0;
  value_writer_create(&writer);
  value_writer_open(writer, g_narrator_stdout);

  value_writer_push_set(writer);
  value_writer_push_mapping(writer);
  value_writer_string(writer, "path");
  value_writer_string(writer, "foo/bar");
  value_writer_pop_mapping(writer);
  value_writer_mapping_string_float(writer, "foo", 42);
  value_writer_pop_set(writer);

  narrator_says(g_narrator_stdout, "\n");

  value_writer_xfree(writer);
}

static void test_set_set_top(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

//printf("\n++ %s\n", __FUNCTION__);

  value_writer * writer = 0;
  value_writer_create(&writer);
  value_writer_open(writer, g_narrator_stdout);

  value_writer_push_mapping(writer);
  value_writer_string(writer, "path");
  value_writer_string(writer, "foo/bar");
  value_writer_pop_mapping(writer);
  value_writer_mapping_string_float(writer, "foo", 42);

  narrator_says(g_narrator_stdout, "\n");

  value_writer_xfree(writer);
}

static void test_set_embedded(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  value_writer_create(&writer);
  value_writer_open(writer, g_narrator_stdout);

  value_writer_push_set(writer);
    value_writer_push_mapping(writer);
      value_writer_string(writer, "container");
      value_writer_push_set(writer);
      value_writer_push_mapping(writer);
        value_writer_string(writer, "path");
        value_writer_string(writer, "foo/bar");
      value_writer_pop_mapping(writer);
      value_writer_pop_set(writer);
    value_writer_pop_mapping(writer);
  value_writer_pop_set(writer);

  narrator_says(g_narrator_stdout, "\n");

  value_writer_xfree(writer);
}

static void test_list(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  value_writer_create(&writer);
  value_writer_open(writer, g_narrator_stdout);

  value_writer_push_list(writer);
    value_writer_bool(writer, true);
    value_writer_bool(writer, false);
    value_writer_bool(writer, true);

    value_writer_mapping_string_float(writer, "foo", 42);
    value_writer_push_mapping(writer);
      value_writer_string(writer, "path");
      value_writer_string(writer, "foo/bar");
    value_writer_pop_mapping(writer);
    value_writer_string(writer, "foo/bar");
    value_writer_string(writer, "foo/bar");
  value_writer_pop_list(writer);

  narrator_says(g_narrator_stdout, "\n");

  value_writer_xfree(writer);
}

static void test_list_list_top(xunit_test * _test)
{
  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  value_writer_create(&writer);
  value_writer_open(writer, g_narrator_stdout);

  value_writer_bool(writer, true);
  value_writer_bool(writer, false);
  value_writer_bool(writer, true);
  value_writer_mapping_string_float(writer, "foo", 42);
  value_writer_push_mapping(writer);
    value_writer_string(writer, "path");
    value_writer_string(writer, "foo/bar");
  value_writer_pop_mapping(writer);

  value_writer_bool(writer, true);
  value_writer_bool(writer, false);

  narrator_says(g_narrator_stdout, "\n");

  value_writer_xfree(writer);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : value_writer_unit_setup
  , xu_cleanup : value_writer_unit_cleanup
  , xu_tests : (void*)(writer_test*[]) {
      (writer_test[]){{ xu_entry : test_basic }}
    , (writer_test[]){{ xu_entry : test_set }}
    , (writer_test[]){{ xu_entry : test_set_set_top }}
    , (writer_test[]){{ xu_entry : test_set_embedded }}
    , (writer_test[]){{ xu_entry : test_list }}
    , (writer_test[]){{ xu_entry : test_list_list_top }}
    , 0
  }
};
XUNIT_UNIT(xunit);
