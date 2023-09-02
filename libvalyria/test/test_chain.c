/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "chain.h"
#include "unit.h"

typedef struct item
{
  int a;
  int x;
  chain chn;
} item;

static void test_append()
{
  const chain *T;
  item * e;
  int x;

  item i1 = { .x = 0 };
  item i2 = { .x = 1 };

  chain_init(&i1.chn);
  assert_eq_zu(1, chain_count(&i1.chn));

  x = 0;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(1, x);

  chain_append(&i1.chn, &i2.chn);
  assert_eq_zu(2, chain_count(&i1.chn));
  assert_eq_zu(2, chain_count(&i2.chn));

  x = 0;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(2, x);

  x = 1;
  chain_foreach(T, e, chn, &i2) {
    assert_eq_d(x, e->x);
    x++;
    x %= 2;
  }
  assert_eq_d(1, x);
}

static void test_prepend()
{
  const chain *T;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  chain_init(&i1.chn);
  assert_eq_zu(1, chain_count(&i1.chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(2, x);

  chain_prepend(&i1.chn, &i2.chn);
  assert_eq_zu(2, chain_count(&i1.chn));
  assert_eq_zu(2, chain_count(&i2.chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(3, x);

  chain_prepend(&i1.chn, &i3.chn);
  assert_eq_zu(3, chain_count(&i1.chn));
  assert_eq_zu(3, chain_count(&i2.chn));
  assert_eq_zu(3, chain_count(&i3.chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(4, x);
}

static void test_splice()
{
  const chain *T;
  item *e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };
  item i4 = { .x = 4 };
  item i5 = { .x = 5 };
  item i6 = { .x = 6 };
  item i7 = { .x = 7 };

  chain_init(&i1.chn);
  assert_eq_zu(1, chain_count(&i1.chn));

  chain_init(&i2.chn);
  assert_eq_zu(1, chain_count(&i2.chn));

  chain_init(&i3.chn);
  chain_init(&i4.chn);
  chain_splice(&i1.chn, &i2.chn);

  assert_eq_zu(2, chain_count(&i1.chn));
  assert_eq_zu(2, chain_count(&i2.chn));

  // forward traversal
  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(3, x);

  chain_init(&i3.chn);
  chain_prepend(&i3.chn, &i4.chn);

  chain_splice(&i1.chn, &i3.chn);
  assert_eq_zu(4, chain_count(&i1.chn));
  assert_eq_zu(4, chain_count(&i2.chn));
  assert_eq_zu(4, chain_count(&i3.chn));
  assert_eq_zu(4, chain_count(&i4.chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(5, x);

  chain_init(&i5.chn);
  chain_prepend(&i5.chn, &i6.chn);
  chain_prepend(&i5.chn, &i7.chn);
  chain_splice(&i1.chn, &i5.chn);
  assert_eq_zu(7, chain_count(&i1.chn));
  assert_eq_zu(7, chain_count(&i2.chn));
  assert_eq_zu(7, chain_count(&i3.chn));
  assert_eq_zu(7, chain_count(&i4.chn));
  assert_eq_zu(7, chain_count(&i5.chn));
  assert_eq_zu(7, chain_count(&i6.chn));
  assert_eq_zu(7, chain_count(&i7.chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(8, x);
}

static void test_cursor()
{
  const chain *c;
  item *e;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  chain_init(&i1.chn);

  c = 0;
  e = chain_next(&i1, &c, chn); assert_eq_p(&i1, e);
  assert_eq_b(false, chain_has_next(&i1.chn, c));
  e = chain_next(&i1, &c, chn); assert_null(e);
  e = chain_next(&i1, &c, chn); assert_null(e); // idempotent

  chain_prepend(&i1.chn, &i2.chn);
  chain_prepend(&i1.chn, &i3.chn);

  c = 0;
  e = chain_next(&i1, &c, chn); assert_eq_p(&i1, e);
  assert_eq_b(true, chain_has_next(&i1.chn, c));
  e = chain_next(&i1, &c, chn); assert_eq_p(&i2, e);
  assert_eq_b(true, chain_has_next(&i1.chn, c));
  e = chain_next(&i1, &c, chn); assert_eq_p(&i3, e);
  assert_eq_b(false, chain_has_next(&i1.chn, c));
  e = chain_next(&i1, &c, chn); assert_null(e);
  e = chain_next(&i1, &c, chn); assert_null(e); // idempotent
}

static void test_safe()
{
  const chain *T[2];
  item *e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  chain_init(&i1.chn);
  assert_eq_zu(1, chain_count(&i1.chn));
  x = 1;
  chain_foreach_safe(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(2, x);

  chain_prepend(&i1.chn, &i2.chn);
  assert_eq_zu(2, chain_count(&i1.chn));
  x = 1;
  chain_foreach_safe(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(3, x);

  chain_prepend(&i1.chn, &i3.chn);
  assert_eq_zu(3, chain_count(&i1.chn));
  x = 1;
  chain_foreach_safe(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(4, x);
}

int main()
{
  test_append();
  test_prepend();
  test_splice();
  test_cursor();
  test_safe();

  summarize;
  return 0;
}
