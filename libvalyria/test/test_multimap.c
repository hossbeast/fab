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

#include <stdlib.h>
#include <stdio.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xstdlib.h"

#include "multimap.internal.h"
#include "test_util.h"
#include "faults.h"

#include "macros.h"

typedef struct item
{
  int a;
  int x;
} item;

static void destructor(void * value)
{
  wfree(value);
}

static xapi set(multimap * restrict m, const char * restrict key, int val)
{
  enter;

  item * itemp;
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = val;
  fatal(multimap_set, m, MMS(key), itemp);

  finally : coda;
}

static bool in_valset(const int * restrict valset, int val)
{
  int n = sentinel(valset);
  int i;

  // keys in the map are in the keyset
  for(i = 0; i < n; i++)
  {
    if(val == valset[i])
      return true;
  }

  return false;
}

static bool in_keyset(const char ** restrict keyset, const char * restrict key)
{
  int n = sentinel(keyset);
  int i;

  // keys in the map are in the keyset
  for(i = 0; i < n; i++)
  {
    if(strcmp(key, keyset[i]) == 0)
      return true;
  }

  return false;
}

static bool in_mapkeys(const multimap * restrict m, const char * restrict key)
{
  size_t i;
  for(i = 0; i < multimap_table_size(m); i++)
  {
    const char * tablekey = multimap_table_key(m, i);
    if(tablekey && strcmp(tablekey, key) == 0)
      return true;
  }

  return false;
}

static bool in_mapvals(item ** values, size_t valuesl, int val)
{
  size_t x;
  for(x = 0; x < valuesl; x++)
  {
    if(val == values[x]->x)
      return true;
  }

  return false;
}

static bool is_tenth_factor(int product, int value)
{
  while(product)
  {
    if(value == product)
      return true;

    product /= 10;
  }

  return false;
}

/// validate_tables
//
// verify that keys/values can be enumerated with _table_ apis
//
// also verify that, for each key/value pair in the multimap, the relation holds that values are
// powers of 10 of the key
//
static xapi validate_tables(multimap * restrict m)
{
  enter;

  int x;
  for(x = 0; x < multimap_table_size(m); x++)
  {
    const char * key = multimap_table_key(m, x);
    const item * itemp = multimap_table_value(m, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      int keyint;
      sscanf(key, "%d", &keyint);
      assert_that(is_tenth_factor(keyint, itemp->x));
    }
  }

  finally : coda;
}

static xapi validate_lookup(multimap * restrict m, const char * restrict key, int * valset)
{
  enter;

  int x;
  int i;

  void * tmp = 0;
  item ** values = 0;
  size_t valuesl = 0;

  fatal(multimap_get, m, MMS(key), &tmp, &values, &valuesl);

  size_t n = sentinel(valset);
  assert_eq_zu(n, valuesl);

  for(x = 0; x < valuesl; x++)
    assert_that(in_valset(valset, values[x]->x));

  for(i = 0; i < n; i++)
    assert_that(in_mapvals(values, valuesl, valset[i]));

finally:
  xapi_infos("key", key);
  wfree(tmp);
coda;
}

static xapi validate_keyset(const multimap * restrict m, const char ** restrict keyset)
{
  enter;

  int n = sentinel(keyset);
  int x;

  // keys that are in the map are in the keyset
  for(x = 0; x < multimap_table_size(m); x++)
  {
    const char * key = multimap_table_key(m, x);
    if(key)
      assert_that(in_keyset(keyset, key));
  }

  // keys that are in the keyset are in the map
  for(x = 0; x < n; x++)
    assert_that(in_mapkeys(m, keyset[x]));

  finally : coda;
}

static xapi validate_absent(multimap * restrict m, const char * restrict key)
{
  enter;

  size_t itemsl = 0;
  fatal(multimap_get, m, MMS(key), 0, 0, &itemsl);
  assert_eq_zu(0, itemsl);

  finally : coda;
}

static const char *set_keyset[] = { "100", "2", "30", 0 };

static xapi validate_set_entryset(multimap * m)
{
  enter;

  // size tracking
  assert_eq_d(6, multimap_size(m));

  // entries by validate_lookup
  fatal(validate_lookup, m, "100", (int[]) { 1, 10, 100, 0 });
  fatal(validate_lookup, m, "2", (int[]) { 2, 0 });
  fatal(validate_lookup, m, "30", (int[]) { 3, 30, 0 });

  // entries by enumeration
  fatal(validate_tables, m);
  fatal(validate_keyset, m, set_keyset);

  finally : coda;
}

static xapi validate_delete_last(multimap * restrict m, const char ** keyset, size_t new_size)
{
  enter;

  size_t last = sentinel(keyset) - 1;

  fatal(multimap_delete, m, MMS(keyset[last]));
  fatal(validate_absent, m, keyset[last]);
  fatal(validate_tables, m);
  keyset[last] = 0;
  fatal(validate_keyset, m, keyset);
  assert_eq_zu(new_size, multimap_size(m));

  finally : coda;
}

static xapi test_set()
{
  enter;

  multimap * m = 0;
  fatal(multimap_createx, &m, destructor, 0, 0);

  // force this key to hash onto the last slot
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("100"));

  fatal(set, m, "100", 1);
  fatal(set, m, "100", 10);
  fatal(set, m, "100", 100);
  fatal(set, m, "2", 2);
  fatal(set, m, "30", 3);
  fatal(set, m, "30", 30);

  fatal(validate_set_entryset, m);

  // force a different key to rehash onto the boundary
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("30"));
  fatal(multimap_rehash, m);

  fatal(validate_set_entryset, m);

finally:
  fatal(multimap_xfree, m);
coda;
}

static xapi test_set_mingle()
{
  enter;

  multimap * m = 0;
  fatal(multimap_createx, &m, destructor, 0, 0);

  // force these keys to hash onto the same slot
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("100"));
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("2"));

  fatal(set, m, "100", 1);
  fatal(set, m, "2", 2);
  fatal(set, m, "100", 10);
  fatal(set, m, "100", 100);
  fatal(set, m, "30", 3);
  fatal(set, m, "30", 30);

  fatal(validate_set_entryset, m);

  // force different keys to rehash onto the boundary
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("30"));
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("2"));
  fatal(multimap_rehash, m);

  fatal(validate_set_entryset, m);

finally:
  fatal(multimap_xfree, m);
coda;
}

static const char * delete_keyset[] = { "100", "200", "3", "37", "9000000", 0 };

static xapi test_delete()
{
  enter;

  multimap * m = 0;
  fatal(multimap_createx, &m, destructor, 0, 0);

  const char * keyset[8];
  memcpy(keyset, delete_keyset, sizeof(delete_keyset));

  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("100"));

  fatal(set, m, "100", 1);
  fatal(set, m, "100", 10);
  fatal(set, m, "100", 100);
  fatal(set, m, "200", 200);
  fatal(set, m, "3", 3);
  fatal(set, m, "37", 37);
  fatal(set, m, "9000000", 9000000);

  // iterative deletion
  fatal(validate_delete_last, m, keyset, 6);
  fatal(validate_delete_last, m, keyset, 5);
  fatal(validate_delete_last, m, keyset, 4);
  fatal(validate_delete_last, m, keyset, 3);
  fatal(validate_delete_last, m, keyset, 0);

finally:
  fatal(multimap_xfree, m);
coda;
}

static xapi test_delete_mingle()
{
  enter;

  multimap * m = 0;
  fatal(multimap_createx, &m, destructor, 0, 0);

  const char * keyset[8];
  memcpy(keyset, delete_keyset, sizeof(delete_keyset));

  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("100"));
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("200"));
  valyria_fault_activate(VALYRIA_MAPDEF_HASH_BOUNDARY_KEY, MMS("37"));

  fatal(set, m, "3", 3);
  fatal(set, m, "100", 1);
  fatal(set, m, "100", 10);
  fatal(set, m, "200", 200);
  fatal(set, m, "37", 37);
  fatal(set, m, "9000000", 9000000);
  fatal(set, m, "100", 100);

  // iterative deletion
  fatal(validate_delete_last, m, keyset, 6);
  fatal(validate_delete_last, m, keyset, 5);
  fatal(validate_delete_last, m, keyset, 4);
  fatal(validate_delete_last, m, keyset, 3);
  fatal(validate_delete_last, m, keyset, 0);

finally:
  fatal(multimap_xfree, m);
coda;
}

static xapi test_load()
{
  enter;

  char space[64];
  multimap * m = 0;

  fatal(multimap_createx, &m, destructor, 0, 0);

  int x;
  for(x = 1; x <= 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    fatal(set, m, space, x * 10);
    fatal(set, m, space, x * 100);
    fatal(set, m, space, x * 1000);
  }

  assert_eq_d(5000 * 3, multimap_size(m));

  // entries by lookup
  for(x = 1; x <= 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    fatal(validate_lookup, m, space, (int[]) { x * 10, x * 100, x * 1000, 0 });
  }

finally:
  fatal(multimap_xfree, m);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_set);
  fatal(test_set_mingle);
  fatal(test_delete);
  fatal(test_delete_mingle);
  fatal(test_load);

  summarize;

  finally : coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(run_tests);

finally:
  if(XAPI_UNWINDING)
    xapi_backtrace();
conclude(&R);
  xapi_teardown();

  return !!R;
}
