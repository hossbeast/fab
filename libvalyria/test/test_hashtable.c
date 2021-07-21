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

#include "hashtable.h"
#include "test_util.h"

#include "macros.h"
#include "common/hash.h"

static inline void absent(hashtable * restrict ht, void * restrict entry)
{
  bool i = hashtable_contains(ht, entry);
  assert_eq_b(false, i);
}

static void validate_entries(hashtable * restrict ht, void * _entries)
{
  int ** entries = _entries;

  const int ** items = 0;
  size_t itemsl;

  int x;
  int i;

  int entriesl = sentinel(entries);

  // items in the set are in the entries - direct table access
  for(x = 0; x < ht->table_size; x++)
  {
    const int * item;
    if((item = hashtable_table_entry(ht, x)))
    {
      for(i = 0; i < entriesl; i++)
      {
        if(*item == *entries[i])
          break;
      }

      if(i == entriesl)
        ufailf("item", "in entries", "not found : %ht", item);
    }
  }

  // items in the set are in the entries - returned sequential list
  hashtable_entries(ht, &items, &itemsl);
  for(x = 0; x < itemsl; x++)
  {
    for(i = 0; i < entriesl; i++)
    {
      if(*items[x] == *entries[i])
        break;
    }

    if(i == entriesl)
      ufailf("item", "in entries", "not found : %ht", items[x]);
  }

  // items in the entries are in the set - table access
  for(x = 0; x < entriesl; x++)
  {
    for(i = 0; i < ht->table_size; i++)
    {
      const int * item;
      if((item = hashtable_table_entry(ht, i)) && *item == *entries[x])
        break;
    }

    if(i == ht->table_size)
      ufailf("item", "in set", "not found : %ht", entries[x]);
  }

  wfree(items);
}

static inline void construct(hashtable ** restrict ht, int ** entries, int start)
{
  hashtable_create(ht, sizeof(int));

  int len = sentinel(entries);
  int x = start;
  do {
    hashtable_put(*ht, entries[x]);
    x = (x + 1) % len;
  } while(x != start);
}

static void test_sizes()
{
  hashtable * ht = 0;

  int x;
  for(x = 1; x <= 17; x++)
  {
    hashtable_create(&ht, x);

    int y;
    for(y = 0; y < 10; y++)
    {
      char data[32];
      sprintf(data, "%d-%d", x, y);
      hashtable_put(ht, data);
      assert_notnull(hashtable_get(ht, data));
    }

    hashtable_ixfree(&ht);
  }

  hashtable_xfree(ht);
}

static void test_basic()
{
  hashtable * ht = 0;
  hashtable_create(&ht, sizeof(int));

  int * entries[] = {
      (int[]) { 1 }
    , (int[]) { 200 }
    , (int[]) { 3 }
    , 0
  };

  hashtable_put(ht, entries[0]);
  hashtable_put(ht, entries[1]);
  hashtable_put(ht, entries[2]);

  assert_eq_d(3, ht->size);

  // entries by lookup
  assert_eq_d(*(int*)hashtable_get(ht, entries[0]), *entries[0]);
  assert_eq_d(*(int*)hashtable_get(ht, entries[1]), *entries[1]);
  assert_eq_d(*(int*)hashtable_get(ht, entries[2]), *entries[2]);

  // entries by contains
  assert_eq_b(true, hashtable_contains(ht, entries[0]));
  assert_eq_b(true, hashtable_contains(ht, entries[1]));
  assert_eq_b(true, hashtable_contains(ht, entries[2]));

  // entries by enumeration
  validate_entries(ht, entries);

  hashtable_xfree(ht);
}

int X;
static void destroy(void * ent)
{
  X++;
}

static void test_put()
{
  hashtable * ht = 0;
  hashtable_createx(&ht, sizeof(int), 0, 0, 0, destroy);

  int value = 5;
  hashtable_put(ht, &value);
  assert_eq_d(0, X);
  assert_eq_d(true, hashtable_contains(ht, &value));
  assert_eq_d(1, ht->size);

  // put destroys the existing value
  hashtable_put(ht, &value);
  assert_eq_d(1, X);
  assert_eq_d(true, hashtable_contains(ht, &value));
  assert_eq_d(1, ht->size);
  int * v = hashtable_get(ht, &value);
  assert_eq_d(5, *v);

  hashtable_xfree(ht);
}

static void test_delete()
{
  hashtable * ht = 0;
  hashtable_create(&ht, sizeof(int));

  int * entries[] = {
      (int[]){ 1 }
    , (int[]){ 200 }
    , (int[]){ 3 }
    , (int[]){ 37 }
    , (int[]){ 9000000 }
    , 0
  };

  hashtable_put(ht, (int[]){ 1 });
  hashtable_put(ht, (int[]){ 200 });
  hashtable_put(ht, (int[]){ 3 });
  hashtable_put(ht, (int[]){ 37 });
  hashtable_put(ht, (int[]){ 9000000 });

  // successive deletion
  hashtable_delete(ht, entries[4]);
  assert_eq_d(4, ht->size);
  absent(ht, entries[4]);
  entries[4] = 0;
  validate_entries(ht, entries);

  hashtable_delete(ht, entries[3]);
  assert_eq_d(3, ht->size);
  absent(ht, entries[3]);
  entries[3] = 0;
  validate_entries(ht, entries);

  hashtable_delete(ht, entries[2]);
  assert_eq_d(2, ht->size);
  absent(ht, entries[2]);
  entries[2] = 0;
  validate_entries(ht, entries);

  hashtable_delete(ht, entries[1]);
  assert_eq_d(1, ht->size);
  absent(ht, entries[1]);
  entries[1] = 0;
  validate_entries(ht, entries);

  hashtable_delete(ht, entries[0]);
  assert_eq_d(0, ht->size);
  absent(ht, entries[0]);
  entries[0] = 0;
  validate_entries(ht, entries);

  hashtable_xfree(ht);
}

static void test_load()
{
  hashtable * ht = 0;
  int x;

  hashtable_create(&ht, sizeof(int));

  for(x = 0; x < 1000; x++)
  {
    assert_eq_d(x, ht->size);
    hashtable_put(ht, &x);
  }

  assert_eq_d(1000, ht->size);

  for(x = 0; x < 1000; x++)
    assert_eq_b(true, hashtable_contains(ht, &x));

  for(x = 1000; x < 2000; x++)
  {
    assert_eq_d(x, ht->size);
    hashtable_put(ht, &x);
  }

  assert_eq_d(2000, ht->size);

  for(x = 0; x < 2000; x++)
    assert_eq_b(true, hashtable_contains(ht, &x));

  for(x = 2000; x < 3000; x++)
  {
    assert_eq_d(x, ht->size);
    hashtable_put(ht, &x);
  }

  assert_eq_d(3000, ht->size);

  for(x = 0; x < 3000; x++)
    assert_eq_b(true, hashtable_contains(ht, &x));

  hashtable_xfree(ht);
}

static void test_equality()
{
  hashtable * A = 0;
  hashtable * B = 0;

  int * items[] = {
      (int[]) { 0x1234 }
    , (int[]) { 0x78bc }
    , (int[]) { 0x38bc }
    , (int[]) { 0x38 }
    , (int[]) { 0x380 }
    , (int[]) { 0x38000 }
    , (int[]) { 0x380001 }
    , (int[]) { 0x3800014 }
    , (int[]) { 0x3800014f }
    , 0
  };

  construct(&A, items, 0);
  construct(&B, items, 5);

  assert_eq_b(true, hashtable_equal(A, B));

  int x;
  for(x = 0; x < sentinel(items); x++)
  {
    hashtable_delete(A, items[x]);
    assert_eq_b(false, hashtable_equal(A, B));
    hashtable_delete(B, items[x]);
    assert_eq_b(true, hashtable_equal(A, B));
  }

  hashtable_xfree(A);
  hashtable_xfree(B);
}

static void test_order()
{
  hashtable * A = 0;
  hashtable * B = 0;
  hashtable * C = 0;
  hashtable * D = 0;

  int * items[] = {
      (int[]) { 0x1234 }
    , (int[]) { 0x78bc }
    , (int[]) { 0x38bc }
    , (int[]) { 0x38 }
    , (int[]) { 0x380 }
    , (int[]) { 0x38000 }
    , (int[]) { 0x380001 }
    , (int[]) { 0x3800014 }
    , (int[]) { 0x3800014f }
    , 0
  };

  construct(&A, items, 0);
  construct(&B, items, 1);
  construct(&C, items, 3);
  construct(&D, items, 5);

  assert_eq_b(true, hashtable_equal(A, B));
  assert_eq_b(true, hashtable_equal(B, C));
  assert_eq_b(true, hashtable_equal(C, D));

  int x;
  for(x = 0; x < sentinel(items); x++)
  {
    hashtable_delete(A, items[x]);
    assert_eq_b(false, hashtable_equal(A, B));

    hashtable_delete(B, items[x]);
    assert_eq_b(true, hashtable_equal(A, B));
  }

  hashtable_xfree(A);
  hashtable_xfree(B);
  hashtable_xfree(C);
  hashtable_xfree(D);
}

static void test_splice()
{
  hashtable * A = 0;
  hashtable * B = 0;
  int x;

  hashtable_create(&A, sizeof(int));
  hashtable_create(&B, sizeof(int));

  for(x = 0; x < 5000; x++)
    hashtable_put(A, &x);

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
    assert_eq_b(true, hashtable_contains(A, &x));

  assert_eq_b(false, hashtable_contains(A, (int[]) { 50000 }));
  assert_eq_b(false, hashtable_contains(B, (int[]) { 50000 }));

  hashtable_delete(A, (int[]) { 450 });
  hashtable_delete(A, (int[]) { 451 });
  hashtable_delete(A, (int[]) { 452 });

  assert_eq_d(4997, A->size);

  // splice A -> B
  hashtable_splice(B, A);

  assert_eq_d(0, A->size);
  assert_eq_d(4997, B->size);

  // lookup in A and B
  for(x = 0; x < 5000; x++)
  {
    if(x == 450 || x == 451 || x == 452)
      continue;

    assert_eq_b(false, hashtable_contains(A, &x));
    assert_eq_b(true, hashtable_contains(B, &x));
  }

  assert_eq_b(false, hashtable_contains(A, (int[]) { 50000 }));
  assert_eq_b(false, hashtable_contains(B, (int[]) { 50000 }));

  hashtable_xfree(A);
  hashtable_xfree(B);
}

static void test_replicate()
{
  hashtable * A = 0;
  hashtable * B = 0;
  int x;

  hashtable_create(&A, sizeof(int));
  hashtable_create(&B, sizeof(int));

  for(x = 0; x < 5000; x++)
    hashtable_put(A, &x);

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
    assert_eq_b(true, hashtable_contains(A, &x));

  assert_eq_b(false, hashtable_contains(A, (int[]) { 50000 }));
  assert_eq_b(false, hashtable_contains(B, (int[]) { 50000 }));

  hashtable_delete(A, (int[]) { 450 });
  hashtable_delete(A, (int[]) { 451 });
  hashtable_delete(A, (int[]) { 452 });

  assert_eq_d(4997, A->size);

  // replicate A -> B
  hashtable_replicate(B, A);

  assert_eq_d(4997, A->size);
  assert_eq_d(4997, B->size);

  // lookup in A and B
  for(x = 0; x < 5000; x++)
  {
    if(x == 450 || x == 451 || x == 452)
      continue;

    assert_eq_b(true, hashtable_contains(A, &x));
    assert_eq_b(true, hashtable_contains(B, &x));
  }

  assert_eq_b(false, hashtable_contains(A, (int[]) { 50000 }));
  assert_eq_b(false, hashtable_contains(B, (int[]) { 50000 }));

  hashtable_xfree(A);
  hashtable_xfree(B);
}

int main()
{
  test_sizes();
  test_basic();
  test_put();
  test_load();
  test_delete();
  test_equality();
  test_order();
  test_replicate();
  test_splice();

  summarize;
  return 0;
}
