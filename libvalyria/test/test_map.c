/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "map.h"
#include "unit.h"

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

  free(itemp);
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
}

static void test_basic()
{
  map * m = 0;
  map_createx(&m, 0, free);

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

  map_free(m);
}

static void test_recycle()
{
  char space[64];
  map * m = 0;
  int x;
  int y;
  int i;

  map_createx(&m, 0, free);

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

  map_free(m);
}

static void test_delete()
{
  map * m = 0;
  map_createx(&m, 0, free);

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

  map_free(m);
}

static void test_successive_mappings()
{
  map m;
  map_initx(&m, 0, free);

  insert(&m, "1", 1);
  assert_eq_d(1, m.size);

  // new mapping with the same key should overwrite/free the previous mapping
  insert(&m, "1", 1);
  assert_eq_d(1, m.size);
  insert(&m, "1", 1);
  assert_eq_d(1, m.size);

  // successive deletion
  map_delete(&m, MMS("1"));
  assert_eq_d(0, m.size);
  absent(&m, "1");

  map_destroy(&m);
}

static void test_equal()
{
  map * A = 0;
  map * B = 0;
  map_createx(&A, 0, free);
  map_createx(&B, 0, free);

  assert_eq_b(true, map_equal(A, B));

  insert(A, "1", 1);
  assert_eq_b(false, map_equal(A, B));

  insert(B, "1", 1);
  assert_eq_b(true, map_equal(A, B));

  map_delete(B, "1", 1);
  assert_eq_b(false, map_equal(A, B));

  map_recycle(A);
  assert_eq_b(true, map_equal(A, B));

  map_free(A);
  map_free(B);
}

static void test_load()
{
  map * m = 0;
  int x;
  char buf[32];

  map_createx(&m, 0, free);

  /* first 1000 */
  for(x = 0; x < 1000; x++)
  {
    assert_eq_d(x, m->size);
    snprintf(buf, sizeof(buf), "%d", x);
    insert(m, buf, x);
  }

  assert_eq_d(1000, m->size);
  for(x = 0; x < 1000; x++)
  {
    snprintf(buf, sizeof(buf), "%d", x);
    contains(m, buf, x);
  }

  /* delete a few entries */
  map_delete(m, MMS("42"));
  map_delete(m, MMS("43"));
  map_delete(m, MMS("44"));

  /* second 1000 */
  for(x = 1000; x < 2000; x++)
  {
    assert_eq_d(x - 3, m->size);
    snprintf(buf, sizeof(buf), "%d", x);
    insert(m, buf, x);
  }

  assert_eq_d(2000 - 3, m->size);
  for(x = 0; x < 2000; x++)
  {
    snprintf(buf, sizeof(buf), "%d", x);
    if(x == 42 || x == 43 || x == 44) {
      absent(m, buf);
    } else {
      contains(m, buf, x);
    }
  }

  /* delete a few entries */
  map_delete(m, MMS("1042"));
  map_delete(m, MMS("1043"));
  map_delete(m, MMS("1044"));

  /* third 1000 */
  for(x = 2000; x < 3000; x++)
  {
    assert_eq_d(x - 6, m->size);
    snprintf(buf, sizeof(buf), "%d", x);
    insert(m, buf, x);
  }

  assert_eq_d(3000 - 6, m->size);
  for(x = 0; x < 3000; x++)
  {
    snprintf(buf, sizeof(buf), "%d", x);
    if(x == 42 || x == 43 || x == 44 || x == 1042 || x == 1043 || x == 1044) {
      absent(m, buf);
    } else {
      contains(m, buf, x);
    }
  }

  map_free(m);
}

static void test_store()
{
  map m;
  map_init(&m);
  map_search_context ctx;
  char *v;

  char * keyset[] = {
      "1"
    , "200"
    , 0
  };

  /* keyset 1 */
  ctx = (typeof(ctx)) { key : keyset[0], key_len : strlen(keyset[0]) };
  assert_eq_b(false, map_search(&m, &ctx));
  map_store(&m, &ctx, keyset[0], 0);

  assert_eq_b(true, map_search(&m, &ctx));
  v = map_get(&m, keyset[0], strlen(keyset[0]));
  assert_eq_s(keyset[0], v);

  /* keyset 2 */
  ctx = (typeof(ctx)) { key : keyset[1], key_len : strlen(keyset[1]) };
  assert_eq_b(false, map_search(&m, &ctx));
  map_store(&m, &ctx, keyset[1], 1);

  assert_eq_b(true, map_search(&m, &ctx));
  v = map_get(&m, keyset[1], strlen(keyset[1]));
  assert_eq_s(keyset[1], v);

  map_destroy(&m);
}

int main()
{
  test_basic();
  test_recycle();
  test_delete();
  test_equal();
  test_successive_mappings();
  test_load();
  test_store();

  summarize;
  return 0;
}
