/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "hashtable.h"
#include "unit.h"

#include "macros.h"
#include "common/hash.h"

static uint32_t hash32_i32(uint32_t h, const void * entry)
{
  return hash32(h, entry, sizeof(int32_t));
}

static int cmp_i32(const void *A, const void *B)
{
  return INTCMP(*(int32_t *)A, *(int32_t *)B);
}

static inline bool contains(hashtable * restrict ht, void * entry)
{
  hashtable_search_context ctx;
  ctx = (typeof(ctx)) { entry : entry };
  return hashtable_search(ht, &ctx);
}

static inline void absent(hashtable * restrict ht, void * entry)
{
  bool exists = contains(ht, entry);
  assert_eq_b(false, exists);
}

static void validate_intlist(hashtable * restrict ht, int * entries, size_t entriesl)
{
  const int * item;
  int x;
  int i;

  // items in the set are in the entries - direct table access
  for(x = 0; x < ht->table_size; x++)
  {
    if((item = hashtable_table_entry(ht, x)))
    {
      for(i = 0; i < entriesl; i++)
      {
        if(*item == entries[i]) {
          break;
        }
      }

      if(i == entriesl) {
        ufailf("item", "in entrylist", "not found : %d", *item);
      }
    }
  }

  // items in the entries are in the set - table access
  for(x = 0; x < entriesl; x++)
  {
    for(i = 0; i < ht->table_size; i++)
    {
      if((item = hashtable_table_entry(ht, i)) && *item == entries[x]) {
        break;
      }
    }

    if(i == ht->table_size) {
      ufailf("entry", "in table", "not found : %d", entries[x]);
    }
  }
}

static inline void construct(hashtable ** restrict ht, int ** entries, int start)
{
  hashtable_create(ht);

  int len = sentinel(entries);
  int x = start;
  do {
    hashtable_put(*ht, entries[x]);
    x = (x + 1) % len;
  } while(x != start);
}

static void test_basic()
{
  hashtable ht;
  hashtable_init(&ht);

  int entries[] = {
      1
    , 200
    , 3
  };

  hashtable_put(&ht, &entries[0]);
  hashtable_put(&ht, &entries[1]);
  hashtable_put(&ht, &entries[2]);

  assert_eq_d(3, ht.size);

  // entries by lookup
  assert_eq_d(*(int*)hashtable_get(&ht, &entries[0]), entries[0]);
  assert_eq_d(*(int*)hashtable_get(&ht, &entries[1]), entries[1]);
  assert_eq_d(*(int*)hashtable_get(&ht, &entries[2]), entries[2]);

  // entries by contains
  assert_eq_b(true, contains(&ht, &entries[0]));
  assert_eq_b(true, contains(&ht, &entries[1]));
  assert_eq_b(true, contains(&ht, &entries[2]));

  // entries by enumeration
  validate_intlist(&ht, entries, sizeof(entries) / sizeof(*entries));

  hashtable_destroy(&ht);
}

int X;
static void destroy(void * ent)
{
  X++;
}

static void test_put()
{
  hashtable * ht = 0;
  int value;
  int *v;

  hashtable_createx(&ht, 0, 0, 0, destroy);

  value = 5;
  hashtable_put(ht, &value);
  assert_eq_d(0, X);
  assert_eq_d(true, contains(ht, &value));
  assert_eq_d(1, ht->size);

  // put destroys the existing value
  hashtable_put(ht, &value);
  assert_eq_d(1, X);
  assert_eq_d(true, contains(ht, &value));
  assert_eq_d(1, ht->size);
  v = hashtable_get(ht, &value);
  assert_eq_d(5, *v);

  hashtable_free(ht);
}

static void test_delete()
{
  hashtable * ht = 0;
  hashtable_createx(&ht, 0, hash32_i32, cmp_i32, 0);
  size_t entries_len;

  int entries[] = {
      1
    , 200
    , 3
    , 37
    , 9000000
  };
  entries_len = sizeof(entries) / sizeof(*entries);

  hashtable_put(ht, (int[]){ 1 });
  hashtable_put(ht, (int[]){ 200 });
  hashtable_put(ht, (int[]){ 3 });
  hashtable_put(ht, (int[]){ 37 });
  hashtable_put(ht, (int[]){ 9000000 });

  // successive deletion
  hashtable_delete(ht, &entries[4]);
  assert_eq_d(4, ht->size);
  absent(ht, &entries[4]);
  entries_len--;
  validate_intlist(ht, entries, entries_len);

  hashtable_delete(ht, &entries[3]);
  assert_eq_d(3, ht->size);
  absent(ht, &entries[3]);
  entries_len--;
  validate_intlist(ht, entries, entries_len);

  hashtable_delete(ht, &entries[2]);
  assert_eq_d(2, ht->size);
  absent(ht, &entries[2]);
  entries_len--;
  validate_intlist(ht, entries, entries_len);

  hashtable_delete(ht, &entries[1]);
  assert_eq_d(1, ht->size);
  absent(ht, &entries[1]);
  entries_len--;
  validate_intlist(ht, entries, entries_len);

  hashtable_delete(ht, &entries[0]);
  assert_eq_d(0, ht->size);
  absent(ht, &entries[0]);
  entries_len--;
  validate_intlist(ht, entries, entries_len);

  hashtable_free(ht);
}

static void test_load()
{
  hashtable * ht = 0;
  int x;

  hashtable_create(&ht);

  for(x = 0; x < 1000; x++)
  {
    assert_eq_d(x, ht->size);
    hashtable_put(ht, (void*)(intptr_t)x);
  }

  assert_eq_d(1000, ht->size);

  for(x = 0; x < 1000; x++)
  {
    assert_eq_b(true, contains(ht, (void*)(intptr_t)x));
  }

  for(x = 1000; x < 2000; x++)
  {
    assert_eq_d(x, ht->size);
    hashtable_put(ht, (void*)(intptr_t)x);
  }

  assert_eq_d(2000, ht->size);

  for(x = 0; x < 2000; x++)
  {
    assert_eq_b(true, contains(ht, (void*)(intptr_t)x));
  }

  for(x = 2000; x < 3000; x++)
  {
    assert_eq_d(x, ht->size);
    hashtable_put(ht, (void*)(intptr_t)x);
  }

  assert_eq_d(3000, ht->size);

  for(x = 0; x < 3000; x++)
  {
    assert_eq_b(true, contains(ht, (void*)(intptr_t)x));
  }

  hashtable_free(ht);
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

  hashtable_free(A);
  hashtable_free(B);
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

  hashtable_free(A);
  hashtable_free(B);
  hashtable_free(C);
  hashtable_free(D);
}

static void test_splice()
{
  hashtable * A = 0;
  hashtable * B = 0;
  int x;

  hashtable_create(&A);
  hashtable_create(&B);

  for(x = 0; x < 5000; x++)
    hashtable_put(A, (void*)(intptr_t)x);

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
    assert_eq_b(true, contains(A, (void*)(intptr_t)x));

  assert_eq_b(false, contains(A, (void*)(intptr_t)50000));
  assert_eq_b(false, contains(B, (void*)(intptr_t)50000));

  hashtable_delete(A, (void*)(intptr_t)450);
  hashtable_delete(A, (void*)(intptr_t)451);
  hashtable_delete(A, (void*)(intptr_t)452);

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

    assert_eq_b(false, contains(A, (void*)(intptr_t)x));
    assert_eq_b(true, contains(B, (void*)(intptr_t)x));
  }

  assert_eq_b(false, contains(A, (void*)(intptr_t)50000));
  assert_eq_b(false, contains(B, (void*)(intptr_t)50000));

  hashtable_free(A);
  hashtable_free(B);
}

static void test_replicate()
{
  hashtable * A = 0;
  hashtable * B = 0;
  int x;

  hashtable_create(&A);
  hashtable_create(&B);

  for(x = 0; x < 5000; x++)
    hashtable_put(A, (void*)(intptr_t)x);

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
    assert_eq_b(true, contains(A, (void*)(intptr_t)x));

  assert_eq_b(false, contains(A, (void*)(intptr_t)50000));
  assert_eq_b(false, contains(B, (void*)(intptr_t)50000));

  hashtable_delete(A, (void*)(intptr_t)450);
  hashtable_delete(A, (void*)(intptr_t)451);
  hashtable_delete(A, (void*)(intptr_t)452);

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

    assert_eq_b(true, contains(A, (void*)(intptr_t)x));
    assert_eq_b(true, contains(B, (void*)(intptr_t)x));
  }

  assert_eq_b(false, contains(A, (void*)(intptr_t)50000));
  assert_eq_b(false, contains(B, (void*)(intptr_t)50000));

  hashtable_free(A);
  hashtable_free(B);
}

int main()
{
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
