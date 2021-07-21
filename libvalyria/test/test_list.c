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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "list.h"
#include "test_util.h"

typedef struct item
{
  int a;
  int x;
} item;

/// validate
//
// SUMMARY
//  accesses each item in the list and asserts that the list is sorted in ascending order
//
static void validate(list * listp)
{
  int x;
  for(x = 1; x < listp->size; x++)
  {
    item * A = list_get(listp, x - 1);
    item * B = list_get(listp, x);

    assert_gt_d(A->x, B->x);
  }
}

static void validate_contents(list * listp, size_t num, ...)
{
  va_list va;
  va_start(va, num);

  assert_eq_zu(listp->size, num);

  int x;
  for(x = 0; x < num; x++)
  {
    int el = va_arg(va, typeof(el));
    assert_eq_d(el, ((item*)list_get(listp, x))->x);
  }

  va_end(va);
}

static void test_basic()
{
  list * listp = 0;
  item * itemp = 0;
  item * itemps[2];

  list_createx(&listp, 0, 0, wfree);

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 4;
  list_push(listp, itemp, 0);
  itemp = 0;

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 5;
  list_push(listp, itemp, 0);
  itemp = 0;

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 6;
  list_push(listp, itemp, 0);
  itemp = 0;

  assert_eq_d(3, listp->size);
  validate(listp);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  list_unshift_range(listp, itemps, 0, 2);

  assert_eq_d(5, listp->size);
  validate(listp);

  list_pop(listp);
  list_shift(listp);

  assert_eq_d(3, listp->size);
  validate(listp);

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 3;
  list_insert(listp, 1, itemp, 0);
  itemp = 0;

  assert_eq_d(4, listp->size);
  validate(listp);

  list_xfree(listp);
  wfree(itemp);
}

static void test_load()
{
  list * listp = 0;
  list_create(&listp);

  int x;
  for(x = 0; x < 50; x++)
    list_push(listp, (void*)0x42, 0);

  for(x = 0; x < 300; x++)
    list_unshift(listp, (void*)0x42, 0);

  for(x = 0; x < 35; x++)
    list_insert(listp, x, (void*)0x42, 0);

  list_xfree(listp);
}

static void test_insertion_sort()
{
  list * listp = 0;
  item * itemp = 0;
  size_t lx;
  int lc;

  int ** items = (int*[]) {
      (int[]) { 4, 6, 7, 5, 0 }
    , (int[]) { 9, 8, 7, 6, 5, 4, 3, 2, 0 }
    , (int[]) { 8, 7, 4, 3, 2, 12, 15, 33, 0 }
    , (int[]) { 1, 0 }
    , 0
  };

  int x;
  for(x = 0; x < sentinel(items); x++)
  {
    list_ixfree(&listp);
    list_createx(&listp, 0, 0, wfree);

    int y;
    for(y = 0; y < sentinel(items[x]); y++)
    {
      xmalloc(&itemp, sizeof(*itemp));
      itemp->x = items[x][y];

      list_search(listp, itemp, sizeof(item), 0, 0, 0, &lx, &lc);
      if(lc <= 0)
        list_insert(listp, lx, itemp, sizeof(item));
      else if(lc > 0)
        list_insert(listp, lx + 1, itemp, sizeof(item));

      itemp = 0;
    }

    validate(listp);
  }

  list_xfree(listp);
  wfree(itemp);
}

static void test_search()
{
  list * listp = 0;
  item * itemp = 0;
  size_t lx;
  int lc;
  item * p;
  bool r;

  list_createx(&listp, 0, 0, wfree);
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 1;
  list_push(listp, itemp, sizeof(item));
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 7;
  list_push(listp, itemp, sizeof(item));
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 32;
  list_push(listp, itemp, sizeof(item));
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 101;
  list_push(listp, itemp, sizeof(item));

  r = list_search(listp, (item[]) {{ x : 0 }}, sizeof(item), 0, &p, 0, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  list_search(listp, (item[]) {{ x : 1 }}, sizeof(item), 0, &p, 0, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(1, p->x);
  assert_eq_zu(0, lx);
  assert_eq_d(0, lc);

  list_search(listp, (item[]) {{ x : 7 }}, sizeof(item), 0, &p, 0, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(7, p->x);
  assert_eq_zu(1, lx);
  assert_eq_d(0, lc);

  list_search(listp, (item[]) {{ x : 32 }}, sizeof(item), 0, &p, 0, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  list_search(listp, (item[]) {{ x : 101 }}, sizeof(item), 0, &p, 0, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(101, p->x);
  assert_eq_zu(3, lx);
  assert_eq_d(0, lc);

  list_search(listp, (item[]) {{ x : 102 }}, sizeof(item), 0, &p, 0, &lx, &lc);
  assert_null(p);

  list_xfree(listp);
}

static void test_search_range()
{
  list * listp = 0;
  item * itemp = 0;
  size_t lx;
  int lc;
  bool r;
  item * p;

  list_createx(&listp, 0, 0, wfree);
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 1;
  list_push(listp, itemp, sizeof(item));
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 7;
  list_push(listp, itemp, sizeof(item));
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 32;
  list_push(listp, itemp, sizeof(item));
  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 101;
  list_push(listp, itemp, sizeof(item));

  r = list_search_range(listp, 0, listp->size, (item[]) {{ x : 0 }}, sizeof(item), 0, &p, 0, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  r = list_search_range(listp, 1, 3, (item[]) {{ x : 1 }}, sizeof(item), 0, &p, 0, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  list_search_range(listp, 2, 2, (item[]) {{ x : 7 }}, sizeof(item), 0, &p, 0, 0, 0);
  assert_null(p);

  list_search_range(listp, 0, 4, (item[]) {{ x : 32 }}, sizeof(item), 0, &p, 0, &lx, 0);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);

  list_search_range(listp, 1, 3, (item[]) {{ x : 32 }}, sizeof(item), 0, &p, 0, &lx, &lc);;
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  list_search_range(listp, 2, 2, (item[]) {{ x : 32 }}, sizeof(item), 0, &p, 0, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  list_search_range(listp, 3, 1, (item[]) {{ x : 32 }}, sizeof(item), 0, &p, 0, 0, &lc);
  assert_null(p);

  list_xfree(listp);
}

static void test_update()
{
  list * listp = 0;
  item * itemp = 0;
  item * itemps[3];

  list_createx(&listp, 0, 0, wfree);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  xmalloc(&itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  list_insert_range(listp, 0, itemps, 0, sizeof(itemps) / sizeof(itemps[0]));

  assert_eq_d(3, listp->size);
  validate(listp);

  int x;
  for(x = 2; x <= 0; x--)
  {
    xmalloc(&itemp, sizeof(*itemp));
    itemp->x = x + 3;
    list_update(listp, x, itemp, 0);
    itemp = 0;

    assert_eq_d(3, listp->size);
    validate(listp);
  }

  assert_eq_d(3, listp->size);

  list_xfree(listp);
  wfree(itemp);
}

static void test_delete()
{
  list * listp = 0;
  item * itemps[4];

  list_createx(&listp, 0, 0, wfree);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  xmalloc(&itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  xmalloc(&itemps[3], sizeof(**itemps));
  itemps[3]->x = 4;
  list_push_range(listp, itemps, 0, sizeof(itemps) / sizeof(itemps[0]));
  validate_contents(listp, 4, 1, 2, 3, 4);

  list_delete(listp, 1);
  validate_contents(listp, 3, 1, 3, 4);
  list_delete(listp, 2);
  validate_contents(listp, 2, 1, 3);
  list_delete(listp, 0);
  validate_contents(listp, 1, 3);
  list_delete(listp, 0);
  validate_contents(listp, 0);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  xmalloc(&itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  xmalloc(&itemps[3], sizeof(**itemps));
  itemps[3]->x = 4;
  list_unshift_range(listp, itemps, 0, sizeof(itemps) / sizeof(itemps[0]));
  validate_contents(listp, 4, 1, 2, 3, 4);

  list_delete_range(listp, 1, 3);
  validate_contents(listp, 1, 1);
  list_delete(listp, 0);
  validate_contents(listp, 0);

  list_xfree(listp);
}

static void test_splice()
{
  list * A = 0;
  list * B = 0;
  item * i1 = 0;
  item * i2 = 0;
  item * i3 = 0;
  item * i4 = 0;

  list_create(&A);
  list_create(&B);

  xmalloc(&i1, sizeof(*i1));
  i1->x = 1;
  list_push(A, i1, 0);
  validate_contents(A, 1, 1);

  xmalloc(&i2, sizeof(*i2));
  i2->x = 4;
  list_push(A, i2, 0);
  validate_contents(A, 2, 1, 4);

  xmalloc(&i3, sizeof(*i3));
  i3->x = 2;
  list_push(B, i3, 0);
  validate_contents(B, 1, 2);

  xmalloc(&i4, sizeof(*i4));
  i4->x = 3;
  list_push(B, i4, 0);
  validate_contents(B, 2, 2, 3);

  list_splice(A, 0, B, 0, 2);
  validate_contents(A, 4, 2, 3, 1, 4);
  validate_contents(B, 0);

  list_xfree(A);
  list_xfree(B);
  wfree(i1);
  wfree(i2);
  wfree(i3);
  wfree(i4);
}

static void test_replicate()
{
  list * A = 0;
  list * B = 0;
  item * i1 = 0;
  item * i2 = 0;
  item * i3 = 0;
  item * i4 = 0;

  list_create(&A);
  list_create(&B);

  xmalloc(&i1, sizeof(*i1));
  i1->x = 1;
  list_push(A, i1, 0);
  validate_contents(A, 1, 1);

  xmalloc(&i2, sizeof(*i2));
  i2->x = 4;
  list_push(A, i2, 0);
  validate_contents(A, 2, 1, 4);

  xmalloc(&i3, sizeof(*i3));
  i3->x = 2;
  list_push(B, i3, 0);
  validate_contents(B, 1, 2);

  xmalloc(&i4, sizeof(*i4));
  i4->x = 3;
  list_push(B, i4, 0);
  validate_contents(B, 2, 2, 3);

  list_replicate(A, 1, B, 0, 2);
  validate_contents(A, 4, 1, 2, 3, 4);
  validate_contents(B, 2, 2, 3);

  list_xfree(A);
  list_xfree(B);
  wfree(i1);
  wfree(i2);
  wfree(i3);
  wfree(i4);
}

int main()
{
  test_basic();
  test_load();
  test_insertion_sort();
  test_update();
  test_search();
  test_search_range();
  test_delete();
  test_splice();
  test_replicate();

  summarize;
  return 0;
}
