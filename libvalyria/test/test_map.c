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

#include "map.h"
#include "test_util.h"

#include "macros.h"

typedef struct item
{
  int a;
  int x;
} item;

static inline void insert(map * restrict m, const char * restrict key, int val)
{
  item * itemp = 0;
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = val;
  map_put(m, MMS(key), itemp, 0);
  itemp = 0;

  wfree(itemp);
}

static inline void contains(map * restrict m, const char * restrict key, int val)
{
  item * i = map_get(m, MMS(key));
  assert_notnull(i);
  assert_eq_d(val, i->x);
}

static inline void absent(map * restrict m, const char * restrict key)
{
  item * i = map_get(m, MMS(key));
  assert_null(i);
}

static void validate_enumeration(map * restrict m)
{
  int x;
  for(x = 0; x < m->table_size; x++)
  {
    const char * key = map_table_key(m, x);
    const item * itemp = map_table_value(m, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      if(strlen(key) == 0)
      {
        assert_eq_d(42, itemp->x);
      }
      else
      {
        int val;
        sscanf(key, "%d", &val);
        assert_eq_d(val, itemp->x);
      }
    }
  }
}

static void validate_keyset(map * restrict m, char ** restrict keyset)
{
  int n = sentinel(keyset);
  int x;
  int i;

  char ** keys;
  uint16_t keysl;

  // keys in the map are in the keyset
  for(x = 0; x < m->table_size; x++)
  {
    const char * key = map_table_key(m, x);
    if(key)
    {
      for(i = 0; i < n; i++)
      {
        if(strcmp(key, keyset[i]) == 0)
          break;
      }

      assert_ne_d(n, i);
    }
  }

  // keys in the keyset are in the map
  for(x = 0; x < n; x++)
  {
    for(i = 0; i < m->table_size; i++)
    {
      const char * key = map_table_key(m, i);
      if(key && strcmp(key, keyset[x]) == 0)
          break;
    }

    assert_ne_d(m->table_size, i);
  }

  map_keys(m, &keys, &keysl);

  wfree(keys);
}

static void test_basic()
{
  map * m = 0;
  map_createx(&m, 0, wfree);

  char * keyset[] = {
      "1"
    , "200"
    , "3"
    , ""
    , 0
  };

  insert(m, "1", 1);
  insert(m, "200", 200);
  insert(m, "3", 3);
  insert(m, "", 42);

  assert_eq_d(4, m->size);

  // entries by lookup
  contains(m, "1", 1);
  contains(m, "3", 3);
  contains(m, "200", 200);
  contains(m, "", 42);

  // entries by enumeration
  validate_enumeration(m);
  validate_keyset(m, keyset);

  map_xfree(m);
}

static void test_recycle()
{
  char space[64];
  map * m = 0;
  int x;
  int y;
  int i;

  map_createx(&m, 0, wfree);

  for(i = 0; i < 4; i++)
  {
    y = i * 1000;
    x = y;
    do {
      snprintf(space, sizeof(space), "%d", x);
      insert(m, space, x);
      x = (x + 1) % 5000;
    } while(x != y);

    assert_eq_d(5000, m->size);

    // entries by lookup
    for(x = 0; x < 5000; x++)
    {
      snprintf(space, sizeof(space), "%d", x);
      contains(m, space, x);
    }

    // entries by enumeration
    validate_enumeration(m);
    map_recycle(m);
  }

  map_xfree(m);
}

static void test_delete()
{
  map * m = 0;
  map_createx(&m, 0, wfree);

  char * keyset[] = {
      "1"
    , "200"
    , "3"
    , "37"
    , "9000000"
    , ""
    , 0
  };

  insert(m, "1", 1);
  insert(m, "200", 200);
  insert(m, "3", 3);
  insert(m, "37", 37);
  insert(m, "9000000", 9000000);
  insert(m, "", 42);

  // successive deletion
  map_delete(m, MMS(keyset[5]));
  assert_eq_d(5, m->size);
  absent(m, keyset[5]);
  validate_enumeration(m);
  keyset[5] = 0;
  validate_keyset(m, keyset);

  map_delete(m, MMS(keyset[4]));
  assert_eq_d(4, m->size);
  absent(m, keyset[4]);
  validate_enumeration(m);
  keyset[4] = 0;
  validate_keyset(m, keyset);

  map_delete(m, MMS(keyset[3]));
  assert_eq_d(3, m->size);
  absent(m, keyset[3]);
  validate_enumeration(m);
  keyset[3] = 0;
  validate_keyset(m, keyset);

  map_delete(m, MMS(keyset[2]));
  assert_eq_d(2, m->size);
  absent(m, keyset[2]);
  validate_enumeration(m);
  keyset[2] = 0;
  validate_keyset(m, keyset);

  map_delete(m, MMS(keyset[1]));
  assert_eq_d(1, m->size);
  absent(m, keyset[1]);
  validate_enumeration(m);
  keyset[1] = 0;
  validate_keyset(m, keyset);

  map_delete(m, MMS(keyset[0]));
  assert_eq_d(0, m->size);
  absent(m, keyset[0]);
  validate_enumeration(m);
  keyset[0] = 0;
  validate_keyset(m, keyset);

  map_xfree(m);
}

static void test_successive_mappings()
{
  map * m = 0;
  map_createx(&m, 0, wfree);

  insert(m, "1", 1);
  assert_eq_d(1, m->size);

  // new mapping with the same key should overwrite/free the previous mapping
  insert(m, "1", 1);
  assert_eq_d(1, m->size);
  insert(m, "1", 1);
  assert_eq_d(1, m->size);

  // successive deletion
  map_delete(m, MMS("1"));
  assert_eq_d(0, m->size);
  absent(m, "1");

  map_xfree(m);
}

static void test_equal()
{
  map * A = 0;
  map * B = 0;
  map_createx(&A, 0, wfree);
  map_createx(&B, 0, wfree);

  assert_eq_b(true, map_equal(A, B));

  insert(A, "1", 1);
  assert_eq_b(false, map_equal(A, B));

  insert(B, "1", 1);
  assert_eq_b(true, map_equal(A, B));

  map_delete(B, "1", 1);
  assert_eq_b(false, map_equal(A, B));

  map_recycle(A);
  assert_eq_b(true, map_equal(A, B));

  map_xfree(A);
  map_xfree(B);
}

int main()
{
  test_basic();
  test_recycle();
  test_delete();
  test_equal();
  test_successive_mappings();

  summarize;
  return 0;
}
