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

static void set(multimap * restrict m, const char * restrict key, int val)
{
  item * itemp;
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = val;
  multimap_set(m, MMS(key), itemp);
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
  for(i = 0; i < m->table_size; i++)
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
static void validate_tables(multimap * restrict m)
{
  int x;
  for(x = 0; x < m->table_size; x++)
  {
    const char * key = multimap_table_key(m, x);
    const item * itemp = multimap_table_value(m, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      if(strlen(key) == 0)
      {
        assert_that(in_valset((int[]) { 0xdead, 0xface, 0 }, itemp->x));
      }
      else
      {
        int keyint;
        sscanf(key, "%d", &keyint);
        assert_that(is_tenth_factor(keyint, itemp->x));
      }
    }
  }
}

static void validate_lookup(multimap * restrict m, const char * restrict key, int * valset)
{
  int x;
  int i;

  void * tmp = 0;
  item ** values = 0;
  size_t valuesl = 0;

  multimap_get(m, MMS(key), &tmp, &values, &valuesl);

  size_t n = sentinel(valset);
  assert_eq_zu(n, valuesl);

  for(x = 0; x < valuesl; x++)
    assert_that(in_valset(valset, values[x]->x));

  for(i = 0; i < n; i++)
    assert_that(in_mapvals(values, valuesl, valset[i]));

  wfree(tmp);
}

static void validate_keyset(const multimap * restrict m, const char ** restrict keyset)
{
  int n = sentinel(keyset);
  int x;

  // keys that are in the map are in the keyset
  for(x = 0; x < m->table_size; x++)
  {
    const char * key = multimap_table_key(m, x);
    if(key)
      assert_that(in_keyset(keyset, key));
  }

  // keys that are in the keyset are in the map
  for(x = 0; x < n; x++)
    assert_that(in_mapkeys(m, keyset[x]));
}

static void validate_absent(multimap * restrict m, const char * restrict key)
{
  size_t itemsl = 0;
  multimap_get(m, MMS(key), 0, 0, &itemsl);
  assert_eq_zu(0, itemsl);
}

static const char *set_keyset[] = { "100", "2", "30", "", 0 };

static void validate_set_entryset(multimap * m)
{
  // size tracking
  assert_eq_d(8, m->size);

  // entries by validate_lookup
  validate_lookup(m, "100", (int[]) { 1, 10, 100, 0 });
  validate_lookup(m, "2", (int[]) { 2, 0 });
  validate_lookup(m, "30", (int[]) { 3, 30, 0 });
  validate_lookup(m, "", (int[]) { 0xdead, 0xface, 0 });

  // entries by enumeration
  validate_tables(m);
  validate_keyset(m, set_keyset);
}

static void validate_delete_last(multimap * restrict m, const char ** keyset, size_t new_size)
{
  size_t last = sentinel(keyset) - 1;

  multimap_delete(m, MMS(keyset[last]));
  validate_absent(m, keyset[last]);
  validate_tables(m);
  keyset[last] = 0;
  validate_keyset(m, keyset);
  assert_eq_zu(new_size, m->size);
}

static void test_set()
{
  multimap * m = 0;
  multimap_createx(&m, destructor, 0);

  // force this key to hash onto the last slot
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("100"));

  set(m, "100", 1);
  set(m, "100", 10);
  set(m, "100", 100);
  set(m, "2", 2);
  set(m, "30", 3);
  set(m, "30", 30);
  set(m, "", 0xdead);
  set(m, "", 0xface);

  validate_set_entryset(m);

  // force a different key to rehash onto the boundary
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("30"));
  multimap_rehash((multimap_t*)m);

  validate_set_entryset(m);

  multimap_xfree(m);
}

static void test_set_mingle()
{
  multimap * m = 0;
  multimap_createx(&m, destructor, 0);

  // force these keys to hash onto the same slot
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("100"));
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("2"));

  set(m, "100", 1);
  set(m, "2", 2);
  set(m, "100", 10);
  set(m, "100", 100);
  set(m, "30", 3);
  set(m, "30", 30);
  set(m, "", 0xdead);
  set(m, "", 0xface);

  validate_set_entryset(m);

  // force different keys to rehash onto the boundary
  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("30"));
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("2"));
  multimap_rehash((multimap_t*)m);

  validate_set_entryset(m);

  multimap_xfree(m);
}

static const char * delete_keyset[] = { "100", "200", "3", "37", "9000000", "", 0 };

static void test_delete()
{
  multimap * m = 0;
  multimap_createx(&m, destructor, 0);

  const char * keyset[8];
  memcpy(keyset, delete_keyset, sizeof(delete_keyset));

  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("100"));

  set(m, "100", 1);
  set(m, "100", 10);
  set(m, "100", 100);
  set(m, "200", 200);
  set(m, "3", 3);
  set(m, "37", 37);
  set(m, "9000000", 9000000);
  set(m, "", 0xdead);
  set(m, "", 0xface);

  // iterative deletion
  validate_delete_last(m, keyset, 7);
  validate_delete_last(m, keyset, 6);
  validate_delete_last(m, keyset, 5);
  validate_delete_last(m, keyset, 4);
  validate_delete_last(m, keyset, 3);
  validate_delete_last(m, keyset, 0);

  multimap_xfree(m);
}

static void test_delete_mingle()
{
  multimap * m = 0;
  multimap_createx(&m, destructor, 0);

  const char * keyset[8];
  memcpy(keyset, delete_keyset, sizeof(delete_keyset));

  valyria_faults_reset();
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("100"));
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("200"));
  valyria_fault_activate(VALYRIA_KEY_INDEX_BOUNDARY, MMS("37"));

  set(m, "3", 3);
  set(m, "100", 1);
  set(m, "100", 10);
  set(m, "200", 200);
  set(m, "37", 37);
  set(m, "9000000", 9000000);
  set(m, "100", 100);
  set(m, "", 0xdead);
  set(m, "", 0xface);

  // iterative deletion
  validate_delete_last(m, keyset, 7);
  validate_delete_last(m, keyset, 6);
  validate_delete_last(m, keyset, 5);
  validate_delete_last(m, keyset, 4);
  validate_delete_last(m, keyset, 3);
  validate_delete_last(m, keyset, 0);

  multimap_xfree(m);
}

static void test_load()
{
  char space[64];
  multimap * m = 0;

  multimap_createx(&m, destructor, 0);

  int x;
  for(x = 1; x <= 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    set(m, space, x * 10);
    set(m, space, x * 100);
    set(m, space, x * 1000);
  }

  assert_eq_d(5000 * 3, m->size);

  // entries by lookup
  for(x = 1; x <= 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    validate_lookup(m, space, (int[]) { x * 10, x * 100, x * 1000, 0 });
  }

  multimap_xfree(m);
}

int main()
{
  test_set();
  test_set_mingle();
  test_delete();
  test_delete_mingle();
  test_load();

  summarize;
  return 0;
}
