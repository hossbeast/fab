/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR src PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <string.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "value.h"
#include "value/assert.h"
#include "value/make.h"
#include "value/store.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "xapi.h"
#include "xapi/errtab.h"
#include "xunit.h"
#include "xunit/assert.h"

#include "internal.h"
#include "make.internal.h"
#include "query.internal.h"

static xapi unit_setup(xunit_unit * unit)
{
  xproxy(value_load);
}

static xapi unit_cleanup(xunit_unit * unit)
{
  xproxy(value_unload);
}

//
// tests
//

static xapi query_test_map_one_level(xunit_test * test)
{
  enter;

  value * map;
  value * foo;
  value * bar;
  value * baz;
  value * exp;
  value * act;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // tree { bar 10 baz 15 foo 20 }
  fatal(value_float_mk, stor, 0, &foo, 20);
  fatal(value_map_mks, stor, 0, 0, &map, "foo", foo, 0);
  fatal(value_float_mk, stor, 0, &bar, 10);
  fatal(value_map_mks, stor, 0, map, &map, "bar", bar, 0);
  fatal(value_float_mk, stor, 0, &baz, 15);
  fatal(value_map_mks, stor, 0, map, &map, "baz", baz, 0);

  // act
  exp = foo;
  act = value_query(map, "foo");

  // assert
  assert_eq_value(exp, act);

  // act
  exp = bar;
  act = value_query(map, "bar");

  // assert
  assert_eq_value(exp, act);

  // act
  exp = baz;
  act = value_query(map, "baz");

  // assert
  assert_eq_value(exp, act);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi query_test_map_two_levels(xunit_test * test)
{
  enter;

  value * map;
  value * foo;
  value * bar;
  value * baz;
  value * exp;
  value * act;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // tree { foo { bar 15 baz 20 } }
  fatal(value_float_mk, stor, 0, &bar, 15);
  fatal(value_map_mks, stor, 0, 0, &foo, "bar", bar, 0);
  fatal(value_float_mk, stor, 0, &baz, 20);
  fatal(value_map_mks, stor, 0, foo, &foo, "baz", baz, 0);
  fatal(value_map_mks, stor, 0, 0, &map, "foo", foo, 0);

  // act
  exp = bar;
  act = value_query(map, "foo.bar");

  // assert
  assert_eq_value(exp, act);

  // act
  exp = baz;
  act = value_query(map, "foo.baz");

  // assert
  assert_eq_value(exp, act);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi query_test_map_nomatch(xunit_test * test)
{
  enter;

  value * map;
  value * foo;
  value * bar;
  value * baz;
  value * act;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // tree { foo { bar 15 baz 20 } }
  fatal(value_float_mk, stor, 0, &bar, 15);
  fatal(value_map_mks, stor, 0, 0, &foo, "bar", bar, 0);
  fatal(value_float_mk, stor, 0, &baz, 20);
  fatal(value_map_mks, stor, 0, foo, &foo, "baz", baz, 0);
  fatal(value_map_mks, stor, 0, 0, &map, "foo", foo, 0);

  // act
  act = value_query(map, "qux");

  // assert
  assert_null_value(act);

  // act
  act = value_query(map, "foot");

  // assert
  assert_null_value(act);

  // act
  act = value_query(map, "foo.qux");

  // assert
  assert_null_value(act);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi query_test_list(xunit_test * test)
{
  enter;

  value * list = 0;
  value * one;
  value * two;
  value * three;
  value * exp;
  value * act;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // tree ( 1 true 3 )
  fatal(value_integer_mk, stor, 0, &one, 1);
  fatal(value_list_mkv, stor, 0, list, &list, one);
  fatal(value_boolean_mk, stor, 0, &two, 1);
  fatal(value_list_mkv, stor, 0, list, &list, two);
  fatal(value_integer_mk, stor, 0, &three, 3);
  fatal(value_list_mkv, stor, 0, list, &list, three);

  // act
  exp = one;
  act = value_query(list, "0");

  // assert
  assert_eq_value(exp, act);

  // act
  exp = two;
  act = value_query(list, "1");

  // assert
  assert_eq_value(exp, act);

  // act
  exp = three;
  act = value_query(list, "2");

  // assert
  assert_eq_value(exp, act);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi query_test_list_nomatch(xunit_test * test)
{
  enter;

  value * list = 0;
  value * one;
  value * two;
  value * three;
  value * act;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // tree ( 1 true 3 )
  fatal(value_integer_mk, stor, 0, &one, 1);
  fatal(value_list_mkv, stor, 0, list, &list, one);
  fatal(value_boolean_mk, stor, 0, &two, 1);
  fatal(value_list_mkv, stor, 0, list, &list, two);
  fatal(value_integer_mk, stor, 0, &three, 3);
  fatal(value_list_mkv, stor, 0, list, &list, three);

  // act
  act = value_query(list, "7");

  // assert
  assert_null_value(act);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi query_test_list_empty(xunit_test * test)
{
  enter;

  value * list = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // list ( )
  fatal(value_list_mkv, stor, 0, list, &list, 0);

  // assert
  assert_eq_d(0, list->items->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

//
// public
//

xunit_unit xunit = {
    xu_setup : unit_setup
  , xu_cleanup : unit_cleanup
  , xu_tests : (xunit_test*[]) {
      (xunit_test[]){{ xu_entry : query_test_map_one_level }}
    , (xunit_test[]){{ xu_entry : query_test_map_two_levels }}
    , (xunit_test[]){{ xu_entry : query_test_map_nomatch }}
    , (xunit_test[]){{ xu_entry : query_test_list }}
    , (xunit_test[]){{ xu_entry : query_test_list_nomatch }}
    , (xunit_test[]){{ xu_entry : query_test_list_empty }}
    , 0
  }
};
