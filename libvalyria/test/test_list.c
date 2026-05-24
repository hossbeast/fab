/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>

#include "xlinux/xstdlib.h"

#include "list.h"
#include "unit.h"

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

  list_createx(&listp, 0, free);

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 4;
  list_push(listp, itemp);
  itemp = 0;

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 5;
  list_push(listp, itemp);
  itemp = 0;

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 6;
  list_push(listp, itemp);
  itemp = 0;

  assert_eq_d(3, listp->size);
  validate(listp);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  list_unshift_range(listp, itemps, 2);

  assert_eq_d(5, listp->size);
  validate(listp);

  list_pop(listp);
  list_shift(listp);

  assert_eq_d(3, listp->size);
  validate(listp);

  xmalloc(&itemp, sizeof(*itemp));
  itemp->x = 3;
  list_insert(listp, 1, itemp);
  itemp = 0;

  assert_eq_d(4, listp->size);
  validate(listp);

  list_free(listp);
  free(itemp);
}

static void test_load()
{
  list li;
  list_init(&li);

  int x;
  for(x = 0; x < 50; x++)
    list_push(&li, (void*)0x42);

  for(x = 0; x < 300; x++)
    list_unshift(&li, (void*)0x42);

  for(x = 0; x < 35; x++)
    list_insert(&li, x, (void*)0x42);

  list_destroy(&li);
}

static void test_update()
{
  list * listp = 0;
  item * itemp = 0;
  item * itemps[3];

  list_createx(&listp, 0, free);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  xmalloc(&itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  list_insert_range(listp, 0, itemps, sizeof(itemps) / sizeof(itemps[0]));

  assert_eq_d(3, listp->size);
  validate(listp);

  int x;
  for(x = 2; x <= 0; x--)
  {
    xmalloc(&itemp, sizeof(*itemp));
    itemp->x = x + 3;
    list_put(listp, x, itemp);
    itemp = 0;

    assert_eq_d(3, listp->size);
    validate(listp);
  }

  assert_eq_d(3, listp->size);

  list_free(listp);
  free(itemp);
}

static void test_delete()
{
  list * listp = 0;
  item * itemps[4];

  list_createx(&listp, 0, free);

  xmalloc(&itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  xmalloc(&itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  xmalloc(&itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  xmalloc(&itemps[3], sizeof(**itemps));
  itemps[3]->x = 4;
  list_push_range(listp, itemps, sizeof(itemps) / sizeof(itemps[0]));
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
  list_unshift_range(listp, itemps, sizeof(itemps) / sizeof(itemps[0]));
  validate_contents(listp, 4, 1, 2, 3, 4);

  list_delete_range(listp, 1, 3);
  validate_contents(listp, 1, 1);
  list_delete(listp, 0);
  validate_contents(listp, 0);

  list_free(listp);
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
  list_push(A, i1);
  validate_contents(A, 1, 1);

  xmalloc(&i2, sizeof(*i2));
  i2->x = 4;
  list_push(A, i2);
  validate_contents(A, 2, 1, 4);

  xmalloc(&i3, sizeof(*i3));
  i3->x = 2;
  list_push(B, i3);
  validate_contents(B, 1, 2);

  xmalloc(&i4, sizeof(*i4));
  i4->x = 3;
  list_push(B, i4);
  validate_contents(B, 2, 2, 3);

  list_splice(A, 0, B, 0, 2);
  validate_contents(A, 4, 2, 3, 1, 4);
  validate_contents(B, 0);

  list_free(A);
  list_free(B);
  free(i1);
  free(i2);
  free(i3);
  free(i4);
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
  list_push(A, i1);
  validate_contents(A, 1, 1);

  xmalloc(&i2, sizeof(*i2));
  i2->x = 4;
  list_push(A, i2);
  validate_contents(A, 2, 1, 4);

  xmalloc(&i3, sizeof(*i3));
  i3->x = 2;
  list_push(B, i3);
  validate_contents(B, 1, 2);

  xmalloc(&i4, sizeof(*i4));
  i4->x = 3;
  list_push(B, i4);
  validate_contents(B, 2, 2, 3);

  list_replicate(A, 1, B, 0, 2);
  validate_contents(A, 4, 1, 2, 3, 4);
  validate_contents(B, 2, 2, 3);

  list_free(A);
  list_free(B);
  free(i1);
  free(i2);
  free(i3);
  free(i4);
}

int main()
{
  test_basic();
  test_load();
  test_update();
  test_delete();
  test_splice();
  test_replicate();

  summarize;
  return 0;
}
