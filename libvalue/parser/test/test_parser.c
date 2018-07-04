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

#include "value.h"
#include "value/store.h"
#include "value/make.h"
#include "logger.h"
#include "logger/category.h"
#include "value/assert.h"

#include "parser.h"

#include "macros.h"

//
// public
//

static xapi value_parser_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(yyutil_load);
  fatal(value_load);

  fatal(logger_finalize);

  finally : coda;
}

static xapi value_parser_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

//
// tests
//

static xapi value_parser_test_maps(xunit_test * test)
{
  enter;

  value_parser * parser = 0;
  value_store * store = 0;
  value * actual = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(narrator_growing_create, &N0);
  fatal(narrator_growing_create, &N1);

  char * value_text = "core.filesystem { \"/\" { invalidate notify } \"/mnt/remote\" { invalidate stat } }";

  // parse
  fatal(value_parser_create, &parser);
  fatal(value_parser_parse, &parser, &store, MMS(value_text), 0, &actual, 0);

  // assert
  value * val = 0;
  value * map = 0;
  value * map2 = 0;
  value * expected = 0;

  fatal(value_string_mks, store, 0, 0, &val, "notify");
  fatal(value_map_mks, store, 0, 0, &map, "invalidate", val, 0);
  fatal(value_map_mks, store, 0, map2, &map2, "/", map, 0);
  fatal(value_string_mks, store, 0, 0, &val, "stat");
  fatal(value_map_mks, store, 0, 0, &map, "invalidate", val, 0);
  fatal(value_map_mks, store, 0, map2, &map2, "/mnt/remote", map, 0);
  fatal(value_map_mks, store, 0, 0, &map, "filesystem", map2, 0);
  fatal(value_map_mks, store, 0, 0, &expected, "core", map, 0);

  fatal(value_say, expected, N0);
  fatal(value_say, actual, N1);
  assert_eq_s(narrator_growing_buffer(N0), narrator_growing_buffer(N1));

finally:
  fatal(value_parser_xfree, parser);
  fatal(value_store_xfree, store);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
coda;
}

static xapi value_parser_test_lists(xunit_test * test)
{
  enter;

  value_parser * parser = 0;
  value_store * store = 0;
  value * actual = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(narrator_growing_create, &N0);
  fatal(narrator_growing_create, &N1);

  char * value_text = "core ( 1 true 2.0 \"foo\" )";

  // parse
  fatal(value_parser_create, &parser);
  fatal(value_parser_parse, &parser, &store, MMS(value_text), 0, &actual, 0);

  // assert
  value * val = 0;
  value * list = 0;
  value * expected = 0;

  fatal(value_integer_mk, store, 0, &val, 1);
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_boolean_mk, store, 0, &val, 1);
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_float_mk, store, 0, &val, 2);
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_string_mks, store, 0, 0, &val, "foo");
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_map_mks, store, 0, 0, &expected, "core", list, 0);

  fatal(value_say, expected, N0);
  fatal(value_say, actual, N1);

  assert_eq_s(narrator_growing_buffer(N0), narrator_growing_buffer(N1));

finally:
  fatal(value_parser_xfree, parser);
  fatal(value_store_xfree, store);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
coda;
}

static xapi value_parser_test_sets(xunit_test * test)
{
  enter;

  value_parser * parser = 0;
  value_store * store = 0;
  value * actual = 0;
  value * val = 0;
  value * expected = 0;
  char * text;

  fatal(value_parser_create, &parser);

  // parse
  text = "[ 1 true 2.0 \"foo\" ]";
  fatal(value_parser_parse, &parser, &store, MMS(text), 0, &actual, 0);

  // assert
  expected = 0;
  fatal(value_integer_mk, store, 0, &val, 1);
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  fatal(value_boolean_mk, store, 0, &val, 1);
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  fatal(value_float_mk, store, 0, &val, 2);
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  fatal(value_string_mks, store, 0, 0, &val, "foo");
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  assert_eq_value(expected, actual);

  // parse
  text = "[ [ 1 2 ] [ 2 1 ] ]";
  fatal(value_parser_parse, &parser, &store, MMS(text), 0, &actual, 0);

  expected = 0;
  fatal(value_integer_mk, store, 0, &val, 1);
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  fatal(value_integer_mk, store, 0, &val, 2);
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  fatal(value_set_mkv, store, 0, 0, &expected, expected);
  assert_eq_value(expected, actual);

  // parse
  text = "( [ 1 2 ] [ 2 1 ] )";
  fatal(value_parser_parse, &parser, &store, MMS(text), 0, &actual, 0);

  expected = 0;
  fatal(value_integer_mk, store, 0, &val, 1);
  fatal(value_set_mkv, store, 0, expected, &expected, val);
  fatal(value_integer_mk, store, 0, &val, 2);
  fatal(value_set_mkv, store, 0, expected, &val, val);
  fatal(value_list_mkv, store, 0, 0, &expected, val);
  fatal(value_list_mkv, store, 0, expected, &expected, val);
  assert_eq_value(expected, actual);

finally:
  fatal(value_parser_xfree, parser);
  fatal(value_store_xfree, store);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : value_parser_unit_setup
  , xu_cleanup : value_parser_unit_cleanup
  , xu_tests : (xunit_test*[]) {
      (xunit_test[]) {{ xu_entry : value_parser_test_maps }}
    , (xunit_test[]) {{ xu_entry : value_parser_test_lists }}
    , (xunit_test[]) {{ xu_entry : value_parser_test_sets }}
    , 0
  }
};
