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
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "value.h"
#include "value/store.h"
#include "value/make.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "merge.internal.h"
#include "VALUE.errtab.h"
#include "make.internal.h"

static xapi unit_setup(xunit_unit * unit)
{
  xproxy(value_load);
}

static xapi unit_cleanup(xunit_unit * unit)
{
  xproxy(value_unload);
}

static xapi assert_entryi(value * map, int x, char * const restrict key, int64_t i)
{
  enter;

  value * k = list_get(map->keys, x);
  value * v = list_get(map->vals, x);

  assertf(strcmp(k->s->s, key) == 0, "%s", "%s", key, k->s->s);
  assertf(v->i == i, "%lld", "%lld", i, v->i);

  finally : coda;
}

static xapi assert_entryf(value * map, int x, char * const restrict key, double f)
{
  enter;

  value * k = list_get(map->keys, x);
  value * v = list_get(map->vals, x);

  assertf(strcmp(k->s->s, key) == 0, "%s", "%s", key, k->s->s);
  assertf(v->f == f, "%f", "%f", f, v->f);

  finally : coda;
}

static xapi assert_elementf(value * map, int x, double exp)
{
  enter;

  value * act = list_get(map->els, x);
  assertf(act->f == exp, "%f", "%f", exp, act->f);

  finally : coda;
}

static xapi mapping_mkf(value_store * stor, value ** map, char * key, double f)
{
  enter;

  value * val = 0;

  fatal(value_float_mk, stor, 0, &val, f);
  fatal(value_map_mks, stor, 0, *map, map, key, val, 0);

  finally : coda;
}

static xapi entry_mkf(value_store * stor, value ** list, double f)
{
  enter;

  value * val = 0;

  fatal(value_float_mk, stor, 0, &val, f);
  fatal(value_list_mkv, stor, 0, *list, list, val);

  finally : coda;
}

//
// tests
//

static xapi merge_test_map_update(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst { bar 10 baz 15 foo 20 }
  fatal(value_float_mk, stor, 0, &val, 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", val, 0);
  fatal(value_float_mk, stor, 0, &val, 10);
  fatal(value_map_mks, stor, 0, dst, &dst, "bar", val, 0);
  fatal(value_float_mk, stor, 0, &val, 15);
  fatal(value_map_mks, stor, 0, dst, &dst, "baz", val, 0);

  // src { foo 5 }
  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, src, &src, "foo", val, 0);

  // { bar 10 baz 15 foo 5 }
  fatal(value_merge, dst, src);

  int x = 0;
  fatal(assert_entryf, dst, x++, "bar", 10);
  fatal(assert_entryf, dst, x++, "baz", 15);
  fatal(assert_entryf, dst, x++, "foo", 5);
  assertf(dst->keys->l == x, "len %d", "len %d", x, dst->keys->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_append(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { bar 10 baz 15 foo 20 }
  fatal(value_integer_mk, stor, 0, &val, 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", val, 0);
  fatal(value_integer_mk, stor, 0, &val, 10);
  fatal(value_map_mks, stor, 0, dst, &dst, "bar", val, 0);
  fatal(value_integer_mk, stor, 0, &val, 15);
  fatal(value_map_mks, stor, 0, dst, &dst, "baz", val, 0);

  // src : { qux 5 }
  fatal(value_integer_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, src, &src, "qux", val, 0);

  // dst : { bar 10 baz 15 foo 20 qux 5 }
  fatal(value_merge, dst, src);

  int x = 0;
  fatal(assert_entryi, dst, x++, "bar", 10);
  fatal(assert_entryi, dst, x++, "baz", 15);
  fatal(assert_entryi, dst, x++, "foo", 20);
  fatal(assert_entryi, dst, x++, "qux", 5);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_prepend(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { bar 10 baz 15 foo 20 }
  fatal(value_float_mk, stor, 0, &val, 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", val, 0);
  fatal(value_float_mk, stor, 0, &val, 10);
  fatal(value_map_mks, stor, 0, dst, &dst, "bar", val, 0);
  fatal(value_float_mk, stor, 0, &val, 15);
  fatal(value_map_mks, stor, 0, dst, &dst, "baz", val, 0);

  // src : { ace 5 }
  fatal(value_float_mk, stor, 0, &val, 5);
  fatal(value_map_mks, stor, 0, src, &src, "ace", val, 0);

  // dst : { ace 5 bar 10 baz 15 foo 20 }
  fatal(value_merge, dst, src);

  int x = 0;
  fatal(assert_entryf, dst, x++, "ace", 5);
  fatal(assert_entryf, dst, x++, "bar", 10);
  fatal(assert_entryf, dst, x++, "baz", 15);
  fatal(assert_entryf, dst, x++, "foo", 20);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_add(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { foo 10 bar 20 }
  fatal(value_float_mk, stor, 0, &val, 10);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", val, 0);
  fatal(value_float_mk, stor, 0, &val, 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "bar", val, 0);

  // src : { baz 30 qux 40 }
  fatal(value_float_mk, stor, 0, &val, 30);
  fatal(value_map_mks, stor, 0, src, &src, "baz", val, VALUE_MERGE_ADD);
  fatal(value_float_mk, stor, 0, &val, 40);
  fatal(value_map_mks, stor, 0, src, &src, "qux", val, VALUE_MERGE_ADD);

  // dst : { bar 20 baz 30 foo 10 qux 40 }
  fatal(value_merge, dst, src);

  int x = 0;
  fatal(assert_entryf, dst, x++, "bar", 20);
  fatal(assert_entryf, dst, x++, "baz", 30);
  fatal(assert_entryf, dst, x++, "foo", 10);
  fatal(assert_entryf, dst, x++, "qux", 40);
  assertf(dst->keys->l == x, "len %d", "len %d", x, dst->keys->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_set(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * map = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { valyria { foo 10 bar 20 } }
  fatal(mapping_mkf, stor, &map, "foo", 10);
  fatal(mapping_mkf, stor, &map, "bar", 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "valyria", map, 0);

  // src : { valyria = { baz 30 qux 40 } }
  map = 0;
  fatal(mapping_mkf, stor, &map, "baz", 30);
  fatal(mapping_mkf, stor, &map, "qux", 40);
  fatal(value_map_mks, stor, 0, src, &src, "valyria", map, VALUE_MERGE_SET);

  // dst : { valyria { baz 30 qux 40 } }
  fatal(value_merge, dst, src);

  map = list_get(dst->vals, 0);
  int x = 0;
  fatal(assert_entryf, map, x++, "baz", 30);
  fatal(assert_entryf, map, x++, "qux", 40);
  assertf(map->keys->l == x, "len %d", "len %d", x, map->keys->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_list_add(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * list = 0;
  value_store * stor = 0;
  int x = 0;

  fatal(value_store_create, &stor);

  // dst : { foo [ 10 20 ] }
  fatal(entry_mkf, stor, &list, 10);
  fatal(entry_mkf, stor, &list, 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", list, 0);

  // src : { foo += [ 30 40 ] }
  list = 0;
  fatal(entry_mkf, stor, &list, 30);
  fatal(entry_mkf, stor, &list, 40);
  fatal(value_map_mks, stor, 0, src, &src, "foo", list, VALUE_MERGE_ADD);

  // dst : { valyria [ 10 20 30 40 ] }
  fatal(value_merge, dst, src);

  list = list_get(dst->vals, 0);
  fatal(assert_elementf, list, x++, 10);
  fatal(assert_elementf, list, x++, 20);
  fatal(assert_elementf, list, x++, 30);
  fatal(assert_elementf, list, x++, 40);
  assertf(list->els->l == x, "len %d", "len %d", x, list->els->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_list_set(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * list = 0;
  value_store * stor = 0;
  int x = 0;

  fatal(value_store_create, &stor);

  // dst : { foo [ 10 20 ] }
  fatal(entry_mkf, stor, &list, 10);
  fatal(entry_mkf, stor, &list, 20);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", list, 0);

  // src : { foo = [ 30 40 ] }
  list = 0;
  fatal(entry_mkf, stor, &list, 30);
  fatal(entry_mkf, stor, &list, 40);
  fatal(value_map_mks, stor, 0, src, &src, "foo", list, VALUE_MERGE_SET);

  // dst : { valyria [ 30 40 ] }
  fatal(value_merge, dst, src);

  list = list_get(dst->vals, 0);
  fatal(assert_elementf, list, x++, 30);
  fatal(assert_elementf, list, x++, 40);
  assertf(list->els->l == x, "len %d", "len %d", x, list->els->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_list_scalar(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : [ 10 20 ]
  fatal(entry_mkf, stor, &dst, 10);
  fatal(entry_mkf, stor, &dst, 20);

  // src : [ 30 40 ]
  fatal(entry_mkf, stor, &src, 30);
  fatal(entry_mkf, stor, &src, 40);

  // dst : [ 10 20 30 40 ]
  fatal(value_merge, dst, src);

  int x = 0;
  fatal(assert_elementf, dst, x++, 10);
  fatal(assert_elementf, dst, x++, 20);
  fatal(assert_elementf, dst, x++, 30);
  fatal(assert_elementf, dst, x++, 40);
  assertf(dst->els->l == x, "len %d", "len %d", x, dst->els->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_list_aggregate(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * val = 0;
  value * map = 0;
  value_store * stor = 0;
  int x;

  fatal(value_store_create, &stor);

  // dst : [ { foo [ 10 ] } ]
  fatal(value_float_mk, stor, 0, &val, 10);
  fatal(value_map_mks, stor, 0, map, &map, "foo", val, 0);
  fatal(value_list_mkv, stor, 0, dst, &dst, map);

  // src : [ { foo [ 20 ] } ]
  map = 0;
  fatal(value_float_mk, stor, 0, &val, 20);
  fatal(value_map_mks, stor, 0, map, &map, "bar", val, 0);
  fatal(value_list_mkv, stor, 0, src, &src, map);

  // dst : [ { foo [ 10 ] } { bar [ 20 ] } ]
  fatal(value_merge, dst, src);

  map = list_get(dst->els, 0);
  x = 0;
  fatal(assert_entryf, map, x++, "foo", 10);
  assertf(map->keys->l == x, "len %d", "len %d", x, map->keys->l);

  map = list_get(dst->els, 1);
  x = 0;
  fatal(assert_entryf, map, x++, "bar", 20);
  assertf(map->keys->l == x, "len %d", "len %d", x, map->keys->l);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_map_set_inner(xunit_test * test)
{
  enter;

  value * src = 0;
  value * dst = 0;
  value * list = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { foo [ 42 ] }
  fatal(entry_mkf, stor, &list, 42);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", list, 0);

  // src : { foo = true }
  fatal(value_boolean_mk, stor, 0, &val, 1);
  fatal(value_map_mks, stor, 0, src, &src, "foo", val, VALUE_MERGE_SET);

  // expected to fail with a particular exit value and kvp containing the path
  fatal(value_merge, dst, src);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_difftype_aggregates(xunit_test * test)
{
  enter;

  char space[64];

  value * src = 0;
  value * dst = 0;
  value * map = 0;
  value * map1 = 0;
  value * map2 = 0;
  value * list = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { foo { bar { baz 10 } } }
  fatal(mapping_mkf, stor, &map1, "baz", 10);
  fatal(value_map_mks, stor, 0, map2, &map2, "bar", map1, 0);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", map2, 0);

  // src : { foo { bar [ 10 ] } }
  fatal(entry_mkf, stor, &list, 10);
  fatal(value_map_mks, stor, 0, map, &map, "bar", list, 0);
  fatal(value_map_mks, stor, 0, src, &src, "foo", map, 0);

  // expected to fail with a particular exit value and kvp containing the path
  xapi exit;
  if((exit = invoke(value_merge, dst, src)))
  {
    xapi_trace_info("path", space, sizeof(space));
    xapi_calltree_unwind();
  }

  assert_exit(VALUE_DIFFTYPE, exit);
  assert_eq_s("foo.bar", space);

finally:
  fatal(value_store_xfree, stor);
coda;
}

static xapi merge_test_difftype_mixed(xunit_test * test)
{
  enter;

  char space[64];

  value * src = 0;
  value * dst = 0;
  value * list = 0;
  value * val = 0;
  value_store * stor = 0;

  fatal(value_store_create, &stor);

  // dst : { foo [ 1 ] }
  fatal(entry_mkf, stor, &list, 10);
  fatal(value_map_mks, stor, 0, dst, &dst, "foo", list, 0);

  // src : { foo 1 }
  fatal(value_float_mk, stor, 0, &val, 1);
  fatal(value_map_mks, stor, 0, src, &src, "foo", val, 0);

  // expected to fail with a particular exit value and kvp containing the path
  xapi exit;
  if((exit = invoke(value_merge, dst, src)))
  {
    xapi_trace_info("path", space, sizeof(space));
    xapi_calltree_unwind();
  }

  assert_exit(VALUE_DIFFTYPE, exit);
  assert_eq_s("foo", space);

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
      (xunit_test[]){{ xu_entry : merge_test_map_update }}
    , (xunit_test[]){{ xu_entry : merge_test_map_prepend }}
    , (xunit_test[]){{ xu_entry : merge_test_map_append }}
    , (xunit_test[]){{ xu_entry : merge_test_map_add }}
    , (xunit_test[]){{ xu_entry : merge_test_map_set }}
    , (xunit_test[]){{ xu_entry : merge_test_map_list_add }}
    , (xunit_test[]){{ xu_entry : merge_test_map_list_set }}
    , (xunit_test[]){{ xu_entry : merge_test_list_scalar }}
    , (xunit_test[]){{ xu_entry : merge_test_list_aggregate }}
    , (xunit_test[]){{ xu_entry : merge_test_map_set_inner }}

    // failure cases
    , (xunit_test[]){{ xu_entry : merge_test_difftype_aggregates }}
    , (xunit_test[]){{ xu_entry : merge_test_difftype_mixed }}
    , 0
  }
};
