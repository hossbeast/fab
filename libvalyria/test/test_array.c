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

#include "xapi.h"
#include "xapi/trace.h"

#include "array.internal.h"
#include "test_util.h"

typedef struct item
{
  int a;
  int x;
} item;

static xapi validate(array * ar)
{
  enter;

  int x;

  for(x = 1; x < ar->size; x++)
  {
    item * A = array_get(ar, x - 1);
    item * B = array_get(ar, x);

    assert_ge_d(A->x, B->x);
  }

  finally : coda;
}

static xapi validate_contents(array * ar, size_t num, ...)
{
  enter;

  va_list va;
  va_start(va, num);

  assert_eq_zu(ar->size, num);

  int x;
  for(x = 0; x < num; x++)
  {
    int el = va_arg(va, typeof(el));
    assert_eq_d(el, ((item*)array_get(ar, x))->x);
  }

finally:
  va_end(va);
coda;
}

static xapi test_basic()
{
  enter;

  array * ar = 0;
  fatal(array_create, &ar, sizeof(item));

  item * itemp = 0;
  fatal(array_unshift, ar, &itemp);
  itemp->x = 6;
  fatal(array_unshift, ar, &itemp);
  itemp->x = 5;
  fatal(array_unshift, ar, &itemp);
  itemp->x = 4;

  fatal(validate, ar);

  item * itemps[2];
  fatal(array_insert_range, ar, 3, itemps, 2);
  itemps[0]->x = 7;
  itemps[1]->x = 8;

  fatal(validate, ar);

  fatal(array_insert, ar, 0, &itemp);
  itemp->x = 1;
  fatal(array_insert, ar, 1, &itemp);
  itemp->x = 2;
  fatal(array_insert, ar, 2, &itemp);
  itemp->x = 3;

  fatal(validate, ar);

finally:
  fatal(array_xfree, ar);
coda;
}

static xapi test_update()
{
  enter;

  array * ar = 0;
  fatal(array_create, &ar, sizeof(item));

  item * itemps[3];
  fatal(array_push_range, ar, itemps, sizeof(itemps) / sizeof(itemps[0]));
  itemps[0]->x = 1;
  itemps[1]->x = 2;
  itemps[2]->x = 3;

  fatal(validate, ar);

  item * itemp;
  fatal(array_update, ar, 2, &itemp);
  itemp->x = 3;
  fatal(array_update, ar, 0, &itemp);
  itemp->x = 1;
  fatal(array_update, ar, 1, &itemp);
  itemp->x = 2;

  fatal(validate, ar);

  fatal(array_update_range, ar, 0, itemps, sizeof(itemps) / sizeof(itemps[0]));
  itemps[0]->x = 10;
  itemps[1]->x = 20;
  itemps[2]->x = 30;

  fatal(validate, ar);

finally:
  fatal(array_xfree, ar);
coda;
}

static xapi test_delete()
{
  enter;

  array * ar = 0;
  item * itemps[4];

  fatal(array_create, &ar, sizeof(item));

  fatal(array_push_range, ar, itemps, sizeof(itemps) / sizeof(itemps[0]));
  itemps[0]->x = 1;
  itemps[1]->x = 2;
  itemps[2]->x = 3;
  itemps[3]->x = 4;
  fatal(validate_contents, ar, 4, 1, 2, 3, 4);

  fatal(array_delete, ar, 1);
  fatal(validate_contents, ar, 3, 1, 3, 4);
  fatal(array_delete, ar, 2);
  fatal(validate_contents, ar, 2, 1, 3);
  fatal(array_delete, ar, 0);
  fatal(validate_contents, ar, 1, 3);
  fatal(array_delete, ar, 0);
  fatal(validate_contents, ar, 0);

  fatal(array_unshift_range, ar, itemps, sizeof(itemps) / sizeof(itemps[0]));
  itemps[0]->x = 1;
  itemps[1]->x = 2;
  itemps[2]->x = 3;
  itemps[3]->x = 4;
  fatal(validate_contents, ar, 4, 1, 2, 3, 4);

  fatal(array_delete_range, ar, 1, 3);
  fatal(validate_contents, ar, 1, 1);
  fatal(array_delete, ar, 0);
  fatal(validate_contents, ar, 0);

finally:
  fatal(array_xfree, ar);
coda;
}

static xapi test_splice()
{
  enter;

  array * A = 0;
  array * B = 0;
  item * itemp = 0;

  fatal(array_create, &A, sizeof(item));
  fatal(array_create, &B, sizeof(item));

  fatal(array_push, A, &itemp);
  itemp->x = 1;
  fatal(validate_contents, A, 1, 1);

  fatal(array_push, A, &itemp);
  itemp->x = 4;
  fatal(validate_contents, A, 2, 1, 4);

  fatal(array_push, B, &itemp);
  itemp->x = 2;
  fatal(validate_contents, B, 1, 2);

  fatal(array_push, B, &itemp);
  itemp->x = 3;
  fatal(validate_contents, B, 2, 2, 3);

  fatal(array_splice, A, 0, B, 0, 2);
  fatal(validate_contents, A, 4, 2, 3, 1, 4);
  fatal(validate_contents, B, 0);

finally:
  fatal(array_xfree, A);
  fatal(array_xfree, B);
coda;
}

static xapi test_replicate()
{
  enter;

  array * A = 0;
  array * B = 0;
  item * itemp = 0;

  fatal(array_create, &A, sizeof(item));
  fatal(array_create, &B, sizeof(item));

  fatal(array_push, A, &itemp);
  itemp->x = 1;
  fatal(validate_contents, A, 1, 1);

  fatal(array_push, A, &itemp);
  itemp->x = 4;
  fatal(validate_contents, A, 2, 1, 4);

  fatal(array_push, B, &itemp);
  itemp->x = 2;
  fatal(validate_contents, B, 1, 2);

  fatal(array_push, B, &itemp);
  itemp->x = 3;
  fatal(validate_contents, B, 2, 2, 3);

  fatal(array_replicate, A, 1, B, 0, 2);
  fatal(validate_contents, A, 4, 1, 2, 3, 4);
  fatal(validate_contents, B, 2, 2, 3);

finally:
  fatal(array_xfree, A);
  fatal(array_xfree, B);
coda;
}

static xapi test_sort()
{
  enter;

  array * ar = 0;
  fatal(array_create, &ar, sizeof(item));

  item * itemp = 0;
  fatal(array_unshift, ar, &itemp);
  itemp->x = 4;
  fatal(array_unshift, ar, &itemp);
  itemp->x = 5;
  fatal(array_unshift, ar, &itemp);
  itemp->x = 6;

  array_sort(ar, 0);
  fatal(validate, ar);

  item * itemps[2];
  fatal(array_insert_range, ar, 3, itemps, 2);
  itemps[0]->x = 9;
  itemps[1]->x = 8;

  array_sort(ar, 0);
  fatal(validate, ar);

  fatal(array_insert, ar, 0, &itemp);
  itemp->x = 10;
  fatal(array_insert, ar, 1, &itemp);
  itemp->x = 20;
  fatal(array_insert, ar, 2, &itemp);
  itemp->x = 3;

  array_sort(ar, 0);
  fatal(validate, ar);

finally:
  fatal(array_xfree, ar);
coda;
}

static xapi test_search()
{
  enter;

  array * ar = 0;
  item * itemps[4];
  item * p;
  bool r;
  size_t lx;
  int lc;

  fatal(array_create, &ar, sizeof(item));
  fatal(array_push_range, ar, itemps, sizeof(itemps) / sizeof(itemps[0]));
  itemps[0]->x = 1;
  itemps[1]->x = 7;
  itemps[2]->x = 32;
  itemps[3]->x = 101;

  r = array_search(ar, (item[]) {{ x : 0 }}, 0, &p, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  r = array_search(ar, (item[]) {{ x : 1 }}, 0, &p, &lx, &lc);
  assert_eq_b(true, r);
  assert_notnull(p);
  assert_eq_d(1, p->x);
  assert_eq_zu(0, lx);
  assert_eq_d(0, lc);

  array_search(ar, (item[]) {{ x : 7 }}, 0, &p, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(7, p->x);
  assert_eq_zu(1, lx);
  assert_eq_d(0, lc);

  array_search(ar, (item[]) {{ x : 32 }}, 0, &p, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  array_search(ar, (item[]) {{ x : 101 }}, 0, &p, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(101, p->x);
  assert_eq_zu(3, lx);
  assert_eq_d(0, lc);

  r = array_search(ar, (item[]) {{ x : 102 }}, 0, &p, &lx, &lc);
  assert_eq_b(false, r);
  assert_null(p);

finally:
  fatal(array_xfree, ar);
coda;
}

static xapi test_search_range()
{
  enter;

  array * ar = 0;
  item * itemps[4];
  item * p;
  bool r;
  size_t lx;
  int lc;

  fatal(array_create, &ar, sizeof(item));
  fatal(array_push_range, ar, itemps, sizeof(itemps) / sizeof(itemps[0]));
  itemps[0]->x = 1;
  itemps[1]->x = 7;
  itemps[2]->x = 32;
  itemps[3]->x = 101;

  r = array_search_range(ar, 0, ar->size, (item[]) {{ x : 0 }}, 0, &p, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  r = array_search_range(ar, 1, 3, (item[]) {{ x : 0 }}, 0, &p, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  r = array_search_range(ar, 2, 2, (item[]) {{ x : 7 }}, 0, &p, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

  array_search_range(ar, 0, 4, (item[]) {{ x : 32 }}, 0, &p, &lx, 0);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);

  array_search_range(ar, 1, 3, (item[]) {{ x : 32 }}, 0, &p, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  array_search_range(ar, 2, 2, (item[]) {{ x : 32 }}, 0, &p, &lx, &lc);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  r = array_search_range(ar, 3, 1, (item[]) {{ x : 32 }}, 0, &p, 0, 0);
  assert_eq_b(false, r);
  assert_null(p);

finally:
  fatal(array_xfree, ar);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_basic);
  fatal(test_update);
  fatal(test_delete);
  fatal(test_splice);
  fatal(test_replicate);
  fatal(test_sort);
  fatal(test_search);
  fatal(test_search_range);
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
