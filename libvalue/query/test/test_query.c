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

#include "xapi.h"
#include "xapi/errtab.h"
#include "value.h"
#include "value/store.h"
#include "value/make.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "narrator.h"
#include "narrator/fixed.h"

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
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(value_store_create, &stor);
  fatal(narrator_fixed_create, &N0, 1024);
  fatal(narrator_fixed_create, &N1, 1024);

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
  fatal(narrator_reset, N0); fatal(value_say, exp, N0);
  fatal(narrator_reset, N1); fatal(value_say, act, N1);
  assertf(value_cmp(exp, act) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

  // act
  exp = bar;
  act = value_query(map, "bar");

  // assert
  fatal(narrator_reset, N0); fatal(value_say, exp, N0);
  fatal(narrator_reset, N1); fatal(value_say, act, N1);
  assertf(value_cmp(exp, act) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

  // act
  exp = baz;
  act = value_query(map, "baz");

  // assert
  fatal(narrator_reset, N0); fatal(value_say, exp, N0);
  fatal(narrator_reset, N1); fatal(value_say, act, N1);
  assertf(value_cmp(exp, act) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

finally:
  fatal(value_store_xfree, stor);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
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
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(value_store_create, &stor);
  fatal(narrator_fixed_create, &N0, 1024);
  fatal(narrator_fixed_create, &N1, 1024);

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
  fatal(narrator_reset, N0); fatal(value_say, exp, N0);
  fatal(narrator_reset, N1); fatal(value_say, act, N1);
  assertf(value_cmp(exp, act) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

  // act
  exp = baz;
  act = value_query(map, "foo.baz");

  // assert
  fatal(narrator_reset, N0); fatal(value_say, exp, N0);
  fatal(narrator_reset, N1); fatal(value_say, act, N1);
  assertf(value_cmp(exp, act) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

finally:
  fatal(value_store_xfree, stor);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
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
  narrator * N0 = 0;

  fatal(value_store_create, &stor);
  fatal(narrator_fixed_create, &N0, 1024);

  // tree { foo { bar 15 baz 20 } }
  fatal(value_float_mk, stor, 0, &bar, 15);
  fatal(value_map_mks, stor, 0, 0, &foo, "bar", bar, 0);
  fatal(value_float_mk, stor, 0, &baz, 20);
  fatal(value_map_mks, stor, 0, foo, &foo, "baz", baz, 0);
  fatal(value_map_mks, stor, 0, 0, &map, "foo", foo, 0);

  // act
  act = value_query(map, "qux");

  // assert
  if(act)
  {
    fatal(narrator_reset, N0);
    fatal(value_say, act, N0);
  }
  assertf(act == 0, "%s", "%s", "(nomatch)", narrator_fixed_buffer(N0));

  // act
  act = value_query(map, "foot");

  // assert
  if(act)
  {
    fatal(narrator_reset, N0);
    fatal(value_say, act, N0);
  }
  assertf(act == 0, "%s", "%s", "(nomatch)", narrator_fixed_buffer(N0));

  // act
  act = value_query(map, "foo.qux");

  // assert
  if(act)
  {
    fatal(narrator_reset, N0);
    fatal(value_say, act, N0);
  }
  assertf(act == 0, "%s", "%s", "(nomatch)", narrator_fixed_buffer(N0));

finally:
  fatal(value_store_xfree, stor);
  fatal(narrator_xfree, N0);
coda;
}

//
// public
//

xunit_unit xunit = {
    setup : unit_setup
  , cleanup : unit_cleanup
  , tests : (xunit_test*[]) {
      (xunit_test[]){{ entry : query_test_map_one_level }}
    , (xunit_test[]){{ entry : query_test_map_two_levels }}
    , (xunit_test[]){{ entry : query_test_map_nomatch }}
    , 0
  }
};
