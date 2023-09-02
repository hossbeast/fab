/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "llist.h"
#include "unit.h"

typedef struct item {
  int a;
  int x;
  llist lln;
} item;

static __attribute__((unused)) void test_append()
{
  llist list;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  llist_append(&list, &i1.lln);
  assert_eq_zu(1, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_append(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };


  llist_append(&list, &i3.lln);
  assert_eq_zu(3, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };
}

static __attribute__((unused)) void test_prepend()
{
  llist list;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  llist_prepend(&list, &i1.lln);
  assert_eq_zu(1, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_prepend(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  x = 2;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x--;
  };

  llist_prepend(&list, &i3.lln);
  assert_eq_zu(3, llist_count(&list));

  x = 3;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x--;
  };
}

static __attribute__((unused)) void test_delete()
{
  llist list;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_prepend(&list, &i1.lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1.lln);
  assert_eq_zu(0, llist_count(&list));

  llist_prepend(&list, &i1.lln);
  llist_prepend(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i1.lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i2.lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_prepend(&list, &i1.lln);
  llist_prepend(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i2.lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1.lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_append(&list, &i1.lln);
  llist_prepend(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i2.lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1.lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_append(&list, &i1.lln);
  llist_prepend(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i2.lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1.lln);
  assert_eq_zu(0, llist_count(&list));
}

static __attribute__((unused)) void test_splice_head()
{
  llist A;
  llist B;
  item *e;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };

  A = LLIST_INITIALIZER(A);
  llist_append(&A, &i1.lln);
  llist_append(&A, &i2.lln);
  llist_append(&A, &i3.lln);

  e = llist_first(&A, typeof(*e), lln);
  assert_eq_d(1, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(2, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(3, e->x);

  B = LLIST_INITIALIZER(B);
  llist_append(&B, &i4.lln);
  llist_append(&B, &i5.lln);
  llist_append(&B, &i6.lln);

  assert_eq_zu(3, llist_count(&A));
  assert_eq_zu(3, llist_count(&B));

  llist_splice_head(&A, &B);

  assert_eq_zu(6, llist_count(&A));
  assert_eq_zu(0, llist_count(&B));

  // cursor traversal
  e = llist_first(&A, typeof(*e), lln);
  assert_eq_d(4, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(5, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(6, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(1, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(2, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(3, e->x);
}

static __attribute__((unused)) void test_splice_tail()
{
  llist A;
  llist B;
  item *e;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };

  A = LLIST_INITIALIZER(A);
  llist_append(&A, &i1.lln);
  llist_append(&A, &i2.lln);
  llist_append(&A, &i3.lln);

  B = LLIST_INITIALIZER(B);
  llist_append(&B, &i4.lln);
  llist_append(&B, &i5.lln);
  llist_append(&B, &i6.lln);

  assert_eq_zu(3, llist_count(&A));
  assert_eq_zu(3, llist_count(&B));

  llist_splice_tail(&A, &B);

  assert_eq_zu(6, llist_count(&A));
  assert_eq_zu(0, llist_count(&B));

  e = llist_first(&A, typeof(*e), lln);
  assert_eq_d(1, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(2, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(3, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(4, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(5, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(6, e->x);
}

static __attribute__((unused)) void test_safe()
{
  llist list;
  llist *T;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  llist_append(&list, &i1.lln);
  assert_eq_zu(1, llist_count(&list));

  x = 1;
  llist_foreach_safe(&list, e, lln, T) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_append(&list, &i2.lln);
  assert_eq_zu(2, llist_count(&list));

  x = 1;
  llist_foreach_safe(&list, e, lln, T) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_append(&list, &i3.lln);
  assert_eq_zu(3, llist_count(&list));

  x = 1;
  llist_foreach_safe(&list, e, lln, T) {
    assert_eq_d(x, e->x);
    x++;
  };
}

static int item_cmp(llist *a, llist *b)
{
  const item *A = containerof(a, item, lln);
  const item *B = containerof(b, item, lln);

  return INTCMP(A->x, B->x);
}

static __attribute__((unused)) void test_sort_ordered_even_small()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };

  llist_init(&list);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(2, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static void __attribute__((unused)) test_sort_ordered_even_large()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };
  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };
  item i7 = { .x = 7 };
  item i8 = { .x = 8 };

  llist_init(&list);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);
  llist_append(&list, &i3.lln);
  llist_append(&list, &i4.lln);
  llist_append(&list, &i5.lln);
  llist_append(&list, &i6.lln);
  llist_append(&list, &i7.lln);
  llist_append(&list, &i8.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(8, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static void __attribute__((unused)) test_sort_ordered_odd_small()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  llist_init(&list);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);
  llist_append(&list, &i3.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(3, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static __attribute__((unused)) void test_sort_ordered_odd_large()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };
  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };
  item i7 = { .x = 7 };
  item i8 = { .x = 8 };
  item i9 = { .x = 9 };

  llist_init(&list);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);
  llist_append(&list, &i3.lln);
  llist_append(&list, &i4.lln);
  llist_append(&list, &i5.lln);
  llist_append(&list, &i6.lln);
  llist_append(&list, &i7.lln);
  llist_append(&list, &i8.lln);
  llist_append(&list, &i9.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(9, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static __attribute__((unused)) void test_sort_unordered_even_small()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };

  llist_init(&list);
  llist_append(&list, &i2.lln);
  llist_append(&list, &i1.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(2, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static __attribute__((unused)) void test_sort_unordered_even_large()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };
  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };
  item i7 = { .x = 7 };
  item i8 = { .x = 8 };

  llist_init(&list);
  llist_append(&list, &i6.lln);
  llist_append(&list, &i5.lln);
  llist_append(&list, &i8.lln);
  llist_append(&list, &i7.lln);
  llist_append(&list, &i4.lln);
  llist_append(&list, &i3.lln);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(8, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static void __attribute__((unused)) test_sort_unordered_odd_small()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  llist_init(&list);
  llist_append(&list, &i3.lln);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(3, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

static __attribute__((unused)) void test_sort_unordered_odd_large()
{
  llist list;
  item *e, *T;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };
  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };
  item i7 = { .x = 7 };
  item i8 = { .x = 8 };
  item i9 = { .x = 9 };

  llist_init(&list);
  llist_append(&list, &i5.lln);
  llist_append(&list, &i6.lln);
  llist_append(&list, &i1.lln);
  llist_append(&list, &i2.lln);
  llist_append(&list, &i9.lln);
  llist_append(&list, &i3.lln);
  llist_append(&list, &i4.lln);
  llist_append(&list, &i7.lln);
  llist_append(&list, &i8.lln);

  llist_sort(&list, item_cmp);

  assert_eq_zu(9, llist_count(&list));

  T = 0;
  llist_foreach(&list, e, lln) {
    if(!T) {
      T = e;
      continue;
    }
    assert_gt_d(T->x, e->x);
    T = e;
  }
}

int main()
{
  test_append();
  test_prepend();
  test_delete();
  test_splice_head();
  test_splice_tail();
  test_safe();
  test_sort_ordered_even_small();
  test_sort_ordered_even_large();
  test_sort_ordered_odd_small();
  test_sort_ordered_odd_large();
  test_sort_unordered_even_small();
  test_sort_unordered_even_large();
  test_sort_unordered_odd_small();
  test_sort_unordered_odd_large();

  summarize;
  return 0;
}
