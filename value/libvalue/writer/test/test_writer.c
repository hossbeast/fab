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
#include "xapi/errtab.h"
#include "xlinux/xstdlib.h"

#include "value/load.h"
#include "yyutil/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "logger.h"
#include "logger/category.h"

#include "value.h"
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

static xapi value_writer_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(value_load);
  fatal(logger_finalize);

  finally : coda;
}

static xapi value_writer_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(value_unload);

  finally : coda;
}

//
// tests
//

static xapi test_basic(xunit_test * _test)
{
  enter;

  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);

  fatal(value_writer_open, writer, g_narrator_stdout);
  fatal(value_writer_push_list, writer);
    fatal(value_writer_string, writer, "foo+88");

    fatal(value_writer_push_set, writer);
    fatal(value_writer_bool, writer, true);
    fatal(value_writer_bool, writer, false);
    fatal(value_writer_pop_set, writer);

    fatal(value_writer_push_set, writer);
    fatal(value_writer_bool, writer, false);
    fatal(value_writer_bool, writer, true);
    fatal(value_writer_pop_set, writer);

    fatal(value_writer_push_set, writer);
      fatal(value_writer_bool, writer, false);
      fatal(value_writer_string, writer, "not-escaped");
      fatal(value_writer_string, writer, "noescape");
    fatal(value_writer_pop_set, writer);

    fatal(value_writer_push_set, writer);
      fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "foo+103");
        fatal(value_writer_push_set, writer);
          fatal(value_writer_mapping_string_uint, writer, "foo+105", 0xabcdabcdabcdULL);
          fatal(value_writer_mapping_string_int, writer, "foo+106", -1234512345123451234);
          fatal(value_writer_mapping_string_bool, writer, "foo/107", true);
        fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_pop_set, writer);

  fatal(value_writer_pop_list, writer);

  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  fatal(value_writer_xfree, writer);
coda;
}

static xapi test_set(xunit_test * _test)
{
  enter;

  writer_test * test = (typeof(test))_test;

//printf("\n++ %s\n", __FUNCTION__);

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);
  fatal(value_writer_open, writer, g_narrator_stdout);

  fatal(value_writer_push_set, writer);
  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, "path");
  fatal(value_writer_string, writer, "foo/bar");
  fatal(value_writer_pop_mapping, writer);
  fatal(value_writer_mapping_string_float, writer, "foo", 42);
  fatal(value_writer_pop_set, writer);

  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  fatal(value_writer_xfree, writer);
coda;
}

static xapi test_set_set_top(xunit_test * _test)
{
  enter;

  writer_test * test = (typeof(test))_test;

//printf("\n++ %s\n", __FUNCTION__);

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);
  fatal(value_writer_open, writer, g_narrator_stdout);

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, "path");
  fatal(value_writer_string, writer, "foo/bar");
  fatal(value_writer_pop_mapping, writer);
  fatal(value_writer_mapping_string_float, writer, "foo", 42);

  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  fatal(value_writer_xfree, writer);
coda;
}

static xapi test_set_embedded(xunit_test * _test)
{
  enter;

  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);
  fatal(value_writer_open, writer, g_narrator_stdout);

  fatal(value_writer_push_set, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "container");
      fatal(value_writer_push_set, writer);
      fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "path");
        fatal(value_writer_string, writer, "foo/bar");
      fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  fatal(value_writer_pop_set, writer);

  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  fatal(value_writer_xfree, writer);
coda;
}

static xapi test_list(xunit_test * _test)
{
  enter;

  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);
  fatal(value_writer_open, writer, g_narrator_stdout);

  fatal(value_writer_push_list, writer);
    fatal(value_writer_bool, writer, true);
    fatal(value_writer_bool, writer, false);
    fatal(value_writer_bool, writer, true);

    fatal(value_writer_mapping_string_float, writer, "foo", 42);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "path");
      fatal(value_writer_string, writer, "foo/bar");
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "foo/bar");
    fatal(value_writer_string, writer, "foo/bar");
  fatal(value_writer_pop_list, writer);

  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  fatal(value_writer_xfree, writer);
coda;
}

static xapi test_list_list_top(xunit_test * _test)
{
  enter;

  writer_test * test = (typeof(test))_test;

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);
  fatal(value_writer_open, writer, g_narrator_stdout);

  fatal(value_writer_bool, writer, true);
  fatal(value_writer_bool, writer, false);
  fatal(value_writer_bool, writer, true);
  fatal(value_writer_mapping_string_float, writer, "foo", 42);
  fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "path");
    fatal(value_writer_string, writer, "foo/bar");
  fatal(value_writer_pop_mapping, writer);

  fatal(value_writer_bool, writer, true);
  fatal(value_writer_bool, writer, false);

  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  fatal(value_writer_xfree, writer);
coda;
}

//
// manifest
//

xunit_unit xunit = {
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
