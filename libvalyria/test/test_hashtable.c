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

#include "hashtable.h"
#include "test_util.h"

#include "macros.h"
#include "hash.h"

static inline xapi absent(hashtable * restrict ht, void * restrict entry)
{
  enter;

  bool i = hashtable_contains(ht, entry);
  assert_eq_b(false, i);

  finally : coda;
}

static xapi validate_entries(hashtable * restrict ht, void * _entries)
{
  enter;

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
  fatal(hashtable_entries, ht, &items, &itemsl);
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

finally:
  fatal(wfree, items);
coda;
}

static inline xapi construct(hashtable ** restrict ht, int ** entries, int start)
{
  enter;

  fatal(hashtable_create, ht, sizeof(int));

  int len = sentinel(entries);
  int x = start;
  do {
    fatal(hashtable_put, *ht, entries[x]);
    x = (x + 1) % len;
  } while(x != start);

  finally : coda;
}

static xapi test_sizes()
{
  enter;

  hashtable * ht = 0;

  int x;
  for(x = 1; x <= 17; x++)
  {
    fatal(hashtable_create, &ht, x);

    int y;
    for(y = 0; y < 10; y++)
    {
      char data[32];
      sprintf(data, "%d-%d", x, y);
      fatal(hashtable_put, ht, data);
      assert_notnull(hashtable_get(ht, data));
    }

    fatal(hashtable_ixfree, &ht);
  }

finally:
  fatal(hashtable_xfree, ht);
coda;
}

static xapi test_basic()
{
  enter;

  hashtable * ht = 0;
  fatal(hashtable_create, &ht, sizeof(int));

  int * entries[] = {
      (int[]) { 1 }
    , (int[]) { 200 }
    , (int[]) { 3 }
    , 0
  };

  fatal(hashtable_put, ht, entries[0]);
  fatal(hashtable_put, ht, entries[1]);
  fatal(hashtable_put, ht, entries[2]);

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
  fatal(validate_entries, ht, entries);

finally:
  fatal(hashtable_xfree, ht);
coda;
}

int X;
static void destroy(void * ent)
{
  X++;
}

static xapi test_put()
{
  enter;

  hashtable * ht = 0;
  fatal(hashtable_createx, &ht, sizeof(int), 0, 0, 0, destroy, 0);

  int value = 5;
  fatal(hashtable_put, ht, &value);
  assert_eq_d(0, X);
  assert_eq_d(true, hashtable_contains(ht, &value));
  assert_eq_d(1, ht->size);

  // put destroys the existing value
  fatal(hashtable_put, ht, &value);
  assert_eq_d(1, X);
  assert_eq_d(true, hashtable_contains(ht, &value));
  assert_eq_d(1, ht->size);
  int * v = hashtable_get(ht, &value);
  assert_eq_d(5, *v);

finally:
  fatal(hashtable_xfree, ht);
coda;
}

static xapi test_delete()
{
  enter;

  hashtable * ht = 0;
  fatal(hashtable_create, &ht, sizeof(int));

  int * entries[] = {
      (int[]){ 1 }
    , (int[]){ 200 }
    , (int[]){ 3 }
    , (int[]){ 37 }
    , (int[]){ 9000000 }
    , 0
  };

  fatal(hashtable_put, ht, (int[]){ 1 });
  fatal(hashtable_put, ht, (int[]){ 200 });
  fatal(hashtable_put, ht, (int[]){ 3 });
  fatal(hashtable_put, ht, (int[]){ 37 });
  fatal(hashtable_put, ht, (int[]){ 9000000 });

  // successive deletion
  fatal(hashtable_delete, ht, entries[4]);
  assert_eq_d(4, ht->size);
  fatal(absent, ht, entries[4]);
  entries[4] = 0;
  fatal(validate_entries, ht, entries);

  fatal(hashtable_delete, ht, entries[3]);
  assert_eq_d(3, ht->size);
  fatal(absent, ht, entries[3]);
  entries[3] = 0;
  fatal(validate_entries, ht, entries);

  fatal(hashtable_delete, ht, entries[2]);
  assert_eq_d(2, ht->size);
  fatal(absent, ht, entries[2]);
  entries[2] = 0;
  fatal(validate_entries, ht, entries);

  fatal(hashtable_delete, ht, entries[1]);
  assert_eq_d(1, ht->size);
  fatal(absent, ht, entries[1]);
  entries[1] = 0;
  fatal(validate_entries, ht, entries);

  fatal(hashtable_delete, ht, entries[0]);
  assert_eq_d(0, ht->size);
  fatal(absent, ht, entries[0]);
  entries[0] = 0;
  fatal(validate_entries, ht, entries);

finally:
  fatal(hashtable_xfree, ht);
coda;
}

static xapi test_load()
{
  enter;

  hashtable * ht = 0;
  int x;

  fatal(hashtable_create, &ht, sizeof(int));

  for(x = 0; x < 1000; x++)
  {
    assert_eq_d(x, ht->size);
    fatal(hashtable_put, ht, &x);
  }

  assert_eq_d(1000, ht->size);

  for(x = 0; x < 1000; x++)
    assert_eq_b(true, hashtable_contains(ht, &x));

  for(x = 1000; x < 2000; x++)
  {
    assert_eq_d(x, ht->size);
    fatal(hashtable_put, ht, &x);
  }

  assert_eq_d(2000, ht->size);

  for(x = 0; x < 2000; x++)
    assert_eq_b(true, hashtable_contains(ht, &x));

  for(x = 2000; x < 3000; x++)
  {
    assert_eq_d(x, ht->size);
    fatal(hashtable_put, ht, &x);
  }

  assert_eq_d(3000, ht->size);

  for(x = 0; x < 3000; x++)
    assert_eq_b(true, hashtable_contains(ht, &x));

finally:
  fatal(hashtable_xfree, ht);
coda;
}

static xapi test_equality()
{
  enter;

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

  fatal(construct, &A, items, 0);
  fatal(construct, &B, items, 5);

  assert_eq_b(true, hashtable_equal(A, B));

  int x;
  for(x = 0; x < sentinel(items); x++)
  {
    fatal(hashtable_delete, A, items[x]);
    assert_eq_b(false, hashtable_equal(A, B));
    fatal(hashtable_delete, B, items[x]);
    assert_eq_b(true, hashtable_equal(A, B));
  }

finally:
  fatal(hashtable_xfree, A);
  fatal(hashtable_xfree, B);
coda;
}

static xapi test_order()
{
  enter;

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

  fatal(construct, &A, items, 0);
  fatal(construct, &B, items, 1);
  fatal(construct, &C, items, 3);
  fatal(construct, &D, items, 5);

  assert_eq_b(true, hashtable_equal(A, B));
  assert_eq_b(true, hashtable_equal(B, C));
  assert_eq_b(true, hashtable_equal(C, D));

  int x;
  for(x = 0; x < sentinel(items); x++)
  {
    fatal(hashtable_delete, A, items[x]);
    assert_eq_b(false, hashtable_equal(A, B));

    fatal(hashtable_delete, B, items[x]);
    assert_eq_b(true, hashtable_equal(A, B));
  }

finally:
  fatal(hashtable_xfree, A);
  fatal(hashtable_xfree, B);
  fatal(hashtable_xfree, C);
  fatal(hashtable_xfree, D);
coda;
}

static xapi test_splice()
{
  enter;

  hashtable * A = 0;
  hashtable * B = 0;
  int x;

  fatal(hashtable_create, &A, sizeof(int));
  fatal(hashtable_create, &B, sizeof(int));

  for(x = 0; x < 5000; x++)
    fatal(hashtable_put, A, &x);

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
    assert_eq_b(true, hashtable_contains(A, &x));

  assert_eq_b(false, hashtable_contains(A, (int[]) { 50000 }));
  assert_eq_b(false, hashtable_contains(B, (int[]) { 50000 }));

  fatal(hashtable_delete, A, (int[]) { 450 });
  fatal(hashtable_delete, A, (int[]) { 451 });
  fatal(hashtable_delete, A, (int[]) { 452 });

  assert_eq_d(4997, A->size);

  // splice A -> B
  fatal(hashtable_splice, B, A);

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

finally:
  fatal(hashtable_xfree, A);
  fatal(hashtable_xfree, B);
coda;
}

static xapi test_replicate()
{
  enter;

  hashtable * A = 0;
  hashtable * B = 0;
  int x;

  fatal(hashtable_create, &A, sizeof(int));
  fatal(hashtable_create, &B, sizeof(int));

  for(x = 0; x < 5000; x++)
    fatal(hashtable_put, A, &x);

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
    assert_eq_b(true, hashtable_contains(A, &x));

  assert_eq_b(false, hashtable_contains(A, (int[]) { 50000 }));
  assert_eq_b(false, hashtable_contains(B, (int[]) { 50000 }));

  fatal(hashtable_delete, A, (int[]) { 450 });
  fatal(hashtable_delete, A, (int[]) { 451 });
  fatal(hashtable_delete, A, (int[]) { 452 });

  assert_eq_d(4997, A->size);

  // replicate A -> B
  fatal(hashtable_replicate, B, A);

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

finally:
  fatal(hashtable_xfree, A);
  fatal(hashtable_xfree, B);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_sizes);
  fatal(test_basic);
  fatal(test_put);
  fatal(test_load);
  fatal(test_delete);
  fatal(test_equality);
  fatal(test_order);
  fatal(test_replicate);
  fatal(test_splice);

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
