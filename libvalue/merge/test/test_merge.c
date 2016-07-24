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

#include <string.h>

#include "xapi.h"
#include "xapi/errtab.h"
#include "value.h"
#include "value/store.h"
#include "value/make.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"

struct value;
#define LIST_ELEMENT_TYPE struct value
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "merge.internal.h"
#include "make.internal.h"

static xapi unit_setup(xunit_unit * unit)
{
  xproxy(value_load);
}

static xapi unit_release(xunit_unit * unit)
{
  xproxy(value_unload);
}

static inline xapi assert_entryf(value * map, int x, char * const restrict key, float f)
{
  enter;

  value * k = list_get(map->keys, x);
  value * v = list_get(map->vals, x);

  assertf(strcmp(k->s->s, key) == 0, "%s", "%s", k->s->s, key);
  assertf(v->f == f, "%f", "%f", v->f);

  finally : coda;
}

static xapi dst_setup(value_store * const restrict stor, value ** B)
{
  enter;

  value * val;

  fatal(value_float_mk, stor, 0, &val, 20);
  fatal(value_map_mks, stor, 0, *B, B, "foo", val);
  fatal(value_float_mk, stor, 0, &val, 10);
  fatal(value_map_mks, stor, 0, *B, B, "bar", val);
  fatal(value_float_mk, stor, 0, &val, 15);
  fatal(value_map_mks, stor, 0, *B, B, "baz", val);

  finally : coda;
}

static xapi merge_test_one(xunit_test * test)
{
  enter;

  value * A = 0;
  value * B = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // source map
  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, A, &A, "foo", val);

  // standard dest map
  fatal(dst_setup, stor, &B);
  fatal(value_merge, B, A);

  int x = 0;
  fatal(assert_entryf, B, x++, "bar", 10);
  fatal(assert_entryf, B, x++, "baz", 15);
  fatal(assert_entryf, B, x++, "foo", 5);

finally:
  value_store_free(stor);
coda;
}

static xapi merge_test_two(xunit_test * test)
{
  enter;

  value * A = 0;
  value * B = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, A, &A, "bar", val);

  fatal(dst_setup, stor, &B);
  fatal(value_merge, B, A);

  int x = 0;
  fatal(assert_entryf, B, x++, "bar", 5);
  fatal(assert_entryf, B, x++, "baz", 15);
  fatal(assert_entryf, B, x++, "foo", 20);

finally:
  value_store_free(stor);
coda;
}

static xapi merge_test_three(xunit_test * test)
{
  enter;

  value * A = 0;
  value * B = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, A, &A, "qux", val);

  fatal(dst_setup, stor, &B);
  fatal(value_merge, B, A);

  int x = 0;
  fatal(assert_entryf, B, x++, "bar", 10);
  fatal(assert_entryf, B, x++, "baz", 15);
  fatal(assert_entryf, B, x++, "foo", 20);
  fatal(assert_entryf, B, x++, "qux", 5);

finally:
  value_store_free(stor);
coda;
}

static xapi merge_test_four(xunit_test * test)
{
  enter;

  value * A = 0;
  value * B = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, A, &A, "ace", val);

  fatal(dst_setup, stor, &B);
  fatal(value_merge, B, A);

  int x = 0;
  fatal(assert_entryf, B, x++, "ace", 5);
  fatal(assert_entryf, B, x++, "bar", 10);
  fatal(assert_entryf, B, x++, "baz", 15);
  fatal(assert_entryf, B, x++, "foo", 20);

finally:
  value_store_free(stor);
coda;
}

static xapi merge_test_five(xunit_test * test)
{
  enter;

  value * A = 0;
  value * B = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, A, &A, "ace", val);
  fatal(value_map_mks, stor, 0, A, &A, "alf", val);
  fatal(value_map_mks, stor, 0, A, &A, "arg", val);

  fatal(dst_setup, stor, &B);
  fatal(value_merge, B, A);

  int x = 0;
  fatal(assert_entryf, B, x++, "ace", 5);
  fatal(assert_entryf, B, x++, "alf", 5);
  fatal(assert_entryf, B, x++, "arg", 5);
  fatal(assert_entryf, B, x++, "bar", 10);
  fatal(assert_entryf, B, x++, "baz", 15);
  fatal(assert_entryf, B, x++, "foo", 20);

finally:
  value_store_free(stor);
coda;
}

static xapi merge_test_six(xunit_test * test)
{
  enter;

  value * A = 0;
  value * B = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, A, &A, "zod", val);
  fatal(value_map_mks, stor, 0, A, &A, "foo", val);
  fatal(value_map_mks, stor, 0, A, &A, "che", val);

  fatal(dst_setup, stor, &B);
  fatal(value_merge, B, A);

  int x = 0;
  fatal(assert_entryf, B, x++, "bar", 10);
  fatal(assert_entryf, B, x++, "baz", 15);
  fatal(assert_entryf, B, x++, "che", 5);
  fatal(assert_entryf, B, x++, "foo", 5);
  fatal(assert_entryf, B, x++, "zod", 5);

finally:
  value_store_free(stor);
coda;
}

//
// public
//

xunit_unit xunit = {
    setup : unit_setup
  , release : unit_release 
  , tests : (xunit_test*[]) {
      (xunit_test[]){{
          entry : merge_test_one
        , name : "merge_test_one"
      }}
    , (xunit_test[]){{
          entry : merge_test_two
        , name : "merge_test_two"
      }}
    , (xunit_test[]){{
          entry : merge_test_three
        , name : "merge_test_three"
      }}
    , (xunit_test[]){{
          entry : merge_test_four
        , name : "merge_test_four"
      }}
    , (xunit_test[]){{
          entry : merge_test_five
        , name : "merge_test_five"
      }}
    , (xunit_test[]){{
          entry : merge_test_six
        , name : "merge_test_six"
      }}
    , 0
  }
};
