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
static xapi validate(list * listp)
{
  enter;

  int x;
  for(x = 1; x < listp->size; x++)
  {
    item * A = list_get(listp, x - 1);
    item * B = list_get(listp, x);

    assert_gt_d(A->x, B->x);
  }

  finally : coda;
}

static xapi validate_contents(list * listp, size_t num, ...)
{
  enter;

  va_list va;
  va_start(va, num);

  assert_eq_zu(listp->size, num);

  int x;
  for(x = 0; x < num; x++)
  {
    int el = va_arg(va, typeof(el));
    assert_eq_d(el, ((item*)list_get(listp, x))->x);
  }

finally:
  va_end(va);
coda;
}

static xapi test_basic()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  item * itemps[2];

  fatal(list_createx, &listp, 0, 0, wfree, 0);

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 4;
  fatal(list_push, listp, itemp, 0);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 5;
  fatal(list_push, listp, itemp, 0);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 6;
  fatal(list_push, listp, itemp, 0);
  itemp = 0;

  assert_eq_d(3, listp->size);
  fatal(validate, listp);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(list_unshift_range, listp, itemps, 0, 2);

  assert_eq_d(5, listp->size);
  fatal(validate, listp);

  fatal(list_pop, listp);
  fatal(list_shift, listp);

  assert_eq_d(3, listp->size);
  fatal(validate, listp);

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 3;
  fatal(list_insert, listp, 1, itemp, 0);
  itemp = 0;

  assert_eq_d(4, listp->size);
  fatal(validate, listp);

finally:
  fatal(list_xfree, listp);
  wfree(itemp);
coda;
}

static xapi test_load()
{
  enter;

  list * listp = 0;
  fatal(list_create, &listp);

  int x;
  for(x = 0; x < 50; x++)
    fatal(list_push, listp, (void*)0x42, 0);

  for(x = 0; x < 300; x++)
    fatal(list_unshift, listp, (void*)0x42, 0);

  for(x = 0; x < 35; x++)
    fatal(list_insert, listp, x, (void*)0x42, 0);

finally:
  fatal(list_xfree, listp);
coda;
}

static xapi test_insertion_sort()
{
  enter;

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
    fatal(list_ixfree, &listp);
    fatal(list_createx, &listp, 0, 0, wfree, 0);

    int y;
    for(y = 0; y < sentinel(items[x]); y++)
    {
      fatal(xmalloc, &itemp, sizeof(*itemp));
      itemp->x = items[x][y];

      list_search(listp, itemp, sizeof(item), 0, 0, 0, &lx, &lc);
      if(lc <= 0)
        fatal(list_insert, listp, lx, itemp, sizeof(item));
      else if(lc > 0)
        fatal(list_insert, listp, lx + 1, itemp, sizeof(item));

      itemp = 0;
    }

    fatal(validate, listp);
  }

finally:
  fatal(list_xfree, listp);
  wfree(itemp);
coda;
}

static xapi test_search()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  size_t lx;
  int lc;
  item * p;
  bool r;

  fatal(list_createx, &listp, 0, 0, wfree, 0);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 1;
  fatal(list_push, listp, itemp, sizeof(item));
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 7;
  fatal(list_push, listp, itemp, sizeof(item));
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 32;
  fatal(list_push, listp, itemp, sizeof(item));
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 101;
  fatal(list_push, listp, itemp, sizeof(item));

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

finally:
  fatal(list_xfree, listp);
coda;
}

static xapi test_search_range()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  size_t lx;
  int lc;
  bool r;
  item * p;

  fatal(list_createx, &listp, 0, 0, wfree, 0);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 1;
  fatal(list_push, listp, itemp, sizeof(item));
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 7;
  fatal(list_push, listp, itemp, sizeof(item));
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 32;
  fatal(list_push, listp, itemp, sizeof(item));
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 101;
  fatal(list_push, listp, itemp, sizeof(item));

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

finally:
  fatal(list_xfree, listp);
coda;
}

static xapi test_update()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  item * itemps[3];

  fatal(list_createx, &listp, 0, 0, wfree, 0);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(xmalloc, &itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  fatal(list_insert_range, listp, 0, itemps, 0, sizeof(itemps) / sizeof(itemps[0]));

  assert_eq_d(3, listp->size);
  fatal(validate, listp);

  int x;
  for(x = 2; x <= 0; x--)
  {
    fatal(xmalloc, &itemp, sizeof(*itemp));
    itemp->x = x + 3;
    fatal(list_update, listp, x, itemp, 0);
    itemp = 0;

    assert_eq_d(3, listp->size);
    fatal(validate, listp);
  }

  assert_eq_d(3, listp->size);

finally:
  fatal(list_xfree, listp);
  wfree(itemp);
coda;
}

static xapi test_delete()
{
  enter;

  list * listp = 0;
  item * itemps[4];

  fatal(list_createx, &listp, 0, 0, wfree, 0);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(xmalloc, &itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  fatal(xmalloc, &itemps[3], sizeof(**itemps));
  itemps[3]->x = 4;
  fatal(list_push_range, listp, itemps, 0, sizeof(itemps) / sizeof(itemps[0]));
  fatal(validate_contents, listp, 4, 1, 2, 3, 4);

  fatal(list_delete, listp, 1);
  fatal(validate_contents, listp, 3, 1, 3, 4);
  fatal(list_delete, listp, 2);
  fatal(validate_contents, listp, 2, 1, 3);
  fatal(list_delete, listp, 0);
  fatal(validate_contents, listp, 1, 3);
  fatal(list_delete, listp, 0);
  fatal(validate_contents, listp, 0);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(xmalloc, &itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  fatal(xmalloc, &itemps[3], sizeof(**itemps));
  itemps[3]->x = 4;
  fatal(list_unshift_range, listp, itemps, 0, sizeof(itemps) / sizeof(itemps[0]));
  fatal(validate_contents, listp, 4, 1, 2, 3, 4);

  fatal(list_delete_range, listp, 1, 3);
  fatal(validate_contents, listp, 1, 1);
  fatal(list_delete, listp, 0);
  fatal(validate_contents, listp, 0);

finally:
  fatal(list_xfree, listp);
coda;
}

static xapi test_splice()
{
  enter;

  list * A = 0;
  list * B = 0;
  item * i1 = 0;
  item * i2 = 0;
  item * i3 = 0;
  item * i4 = 0;

  fatal(list_create, &A);
  fatal(list_create, &B);

  fatal(xmalloc, &i1, sizeof(*i1));
  i1->x = 1;
  fatal(list_push, A, i1, 0);
  fatal(validate_contents, A, 1, 1);

  fatal(xmalloc, &i2, sizeof(*i2));
  i2->x = 4;
  fatal(list_push, A, i2, 0);
  fatal(validate_contents, A, 2, 1, 4);

  fatal(xmalloc, &i3, sizeof(*i3));
  i3->x = 2;
  fatal(list_push, B, i3, 0);
  fatal(validate_contents, B, 1, 2);

  fatal(xmalloc, &i4, sizeof(*i4));
  i4->x = 3;
  fatal(list_push, B, i4, 0);
  fatal(validate_contents, B, 2, 2, 3);

  fatal(list_splice, A, 0, B, 0, 2);
  fatal(validate_contents, A, 4, 2, 3, 1, 4);
  fatal(validate_contents, B, 0);

finally:
  fatal(list_xfree, A);
  fatal(list_xfree, B);
  fatal(wfree, i1);
  fatal(wfree, i2);
  fatal(wfree, i3);
  fatal(wfree, i4);
coda;
}

static xapi test_replicate()
{
  enter;

  list * A = 0;
  list * B = 0;
  item * i1 = 0;
  item * i2 = 0;
  item * i3 = 0;
  item * i4 = 0;

  fatal(list_create, &A);
  fatal(list_create, &B);

  fatal(xmalloc, &i1, sizeof(*i1));
  i1->x = 1;
  fatal(list_push, A, i1, 0);
  fatal(validate_contents, A, 1, 1);

  fatal(xmalloc, &i2, sizeof(*i2));
  i2->x = 4;
  fatal(list_push, A, i2, 0);
  fatal(validate_contents, A, 2, 1, 4);

  fatal(xmalloc, &i3, sizeof(*i3));
  i3->x = 2;
  fatal(list_push, B, i3, 0);
  fatal(validate_contents, B, 1, 2);

  fatal(xmalloc, &i4, sizeof(*i4));
  i4->x = 3;
  fatal(list_push, B, i4, 0);
  fatal(validate_contents, B, 2, 2, 3);

  fatal(list_replicate, A, 1, B, 0, 2);
  fatal(validate_contents, A, 4, 1, 2, 3, 4);
  fatal(validate_contents, B, 2, 2, 3);

finally:
  fatal(list_xfree, A);
  fatal(list_xfree, B);
  fatal(wfree, i1);
  fatal(wfree, i2);
  fatal(wfree, i3);
  fatal(wfree, i4);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_basic);
  fatal(test_load);
  fatal(test_insertion_sort);
  fatal(test_update);
  fatal(test_search);
  fatal(test_search_range);
  fatal(test_delete);
  fatal(test_splice);
  fatal(test_replicate);

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
    xapi_backtrace(2, 0);
conclude(&R);
  xapi_teardown();

  return !!R;
}
