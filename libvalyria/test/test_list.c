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
  for(x = 1; x < listp->l; x++)
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

  assert_eq_zu(listp->l, num);

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

  fatal(list_createx, &listp, wfree, 0, 0);

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 4;
  fatal(list_push, listp, itemp);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 5;
  fatal(list_push, listp, itemp);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 6;
  fatal(list_push, listp, itemp);
  itemp = 0;

  fatal(validate, listp);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(list_unshift_range, listp, itemps, 2);

  fatal(validate, listp);

  fatal(list_pop, listp, 0);
  fatal(list_shift, listp, 0);

  fatal(validate, listp);

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 3;
  fatal(list_insert, listp, 1, itemp);
  itemp = 0;

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
    fatal(list_push, listp, (void*)0x42);

  for(x = 0; x < 300; x++)
    fatal(list_unshift, listp, (void*)0x42);

  for(x = 0; x < 35; x++)
    fatal(list_insert, listp, x, (void*)0x42);

finally:
  fatal(list_xfree, listp);
coda;
}

static xapi test_insertion_sort()
{
  enter;

  list * listp = 0;
  item * itemp = 0;

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
    fatal(list_createx, &listp, (void*)wfree, 0, 0);

    int y;
    for(y = 0; y < sentinel(items[x]); y++)
    {
      size_t lx = 0;
      int lc = 0;

      int compar(void * ud, const void * el, size_t idx)
      {
        lx = idx;
        return lc = itemp->x - ((item*)el)->x;
      };

      fatal(xmalloc, &itemp, sizeof(*itemp));
      itemp->x = items[x][y];

      list_search(listp, 0, compar);
      if(lc <= 0)
        fatal(list_insert, listp, lx, itemp);
      else if(lc > 0)
        fatal(list_insert, listp, lx + 1, itemp);

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

  fatal(list_createx, &listp, (void*)wfree, 0, 0);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 1;
  fatal(list_push, listp, itemp);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 7;
  fatal(list_push, listp, itemp);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 32;
  fatal(list_push, listp, itemp);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 101;
  fatal(list_push, listp, itemp);

  size_t lx;
  int lc;

  int compar(void * ud, const void * el, size_t idx)
  {
    lx = idx;
    lc = (int)(intptr_t)ud - ((item*)el)->x;
    return lc;
  };

  item * p = list_search(listp, (void*)0, compar);
  assert_null(p);

  p = list_search(listp, (void*)1, compar);
  assert_notnull(p);
  assert_eq_d(1, p->x);
  assert_eq_zu(0, lx);
  assert_eq_d(0, lc);

  p = list_search(listp, (void*)7, compar);
  assert_notnull(p);
  assert_eq_d(7, p->x);
  assert_eq_zu(1, lx);
  assert_eq_d(0, lc);

  p = list_search(listp, (void*)32, compar);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  p = list_search(listp, (void*)101, compar);
  assert_notnull(p);
  assert_eq_d(101, p->x);
  assert_eq_zu(3, lx);
  assert_eq_d(0, lc);

  p = list_search(listp, (void*)102, compar);
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

  fatal(list_createx, &listp, (void*)wfree, 0, 0);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 1;
  fatal(list_push, listp, itemp);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 7;
  fatal(list_push, listp, itemp);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 32;
  fatal(list_push, listp, itemp);
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 101;
  fatal(list_push, listp, itemp);

  size_t lx;
  int lc;

  int compar(void * ud, const void * el, size_t idx)
  {
    lx = idx;
    lc = (int)(intptr_t)ud - ((item*)el)->x;
    return lc;
  };

  item * p = list_search_range(listp, 0, listp->l, (void*)0, compar);
  assert_null(p);

  p = list_search_range(listp, 1, 3, (void*)1, compar);
  assert_null(p);

  p = list_search_range(listp, 2, 2, (void*)7, compar);
  assert_null(p);

  p = list_search_range(listp, 0, 4, (void*)32, compar);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);

  p = list_search_range(listp, 1, 3, (void*)32, compar);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  p = list_search_range(listp, 2, 2, (void*)32, compar);
  assert_notnull(p);
  assert_eq_d(32, p->x);
  assert_eq_zu(2, lx);
  assert_eq_d(0, lc);

  p = list_search_range(listp, 3, 1, (void*)32, compar);
  assert_null(p);

finally:
  fatal(list_xfree, listp);
coda;
}

static xapi test_set()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  item * itemps[3];

  fatal(list_createx, &listp, (void*)wfree, 0, 0);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(xmalloc, &itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  fatal(list_push_range, listp, itemps, sizeof(itemps) / sizeof(itemps[0]));

  fatal(validate, listp);

  int x;
  for(x = 2; x <= 0; x--)
  {
    fatal(xmalloc, &itemp, sizeof(*itemp));
    itemp->x = x + 3;
    fatal(list_set, listp, x, itemp);
    itemp = 0;

    fatal(validate, listp);
  }

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

  fatal(list_createx, &listp, (void*)wfree, 0, 0);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(xmalloc, &itemps[2], sizeof(**itemps));
  itemps[2]->x = 3;
  fatal(xmalloc, &itemps[3], sizeof(**itemps));
  itemps[3]->x = 4;
  fatal(list_push_range, listp, itemps, sizeof(itemps) / sizeof(itemps[0]));
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
  fatal(list_unshift_range, listp, itemps, sizeof(itemps) / sizeof(itemps[0]));
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
  fatal(list_push, A, i1);
  fatal(validate_contents, A, 1, 1);

  fatal(xmalloc, &i2, sizeof(*i2));
  i2->x = 4;
  fatal(list_push, A, i2);
  fatal(validate_contents, A, 2, 1, 4);

  fatal(xmalloc, &i3, sizeof(*i3));
  i3->x = 2;
  fatal(list_push, B, i3);
  fatal(validate_contents, B, 1, 2);

  fatal(xmalloc, &i4, sizeof(*i4));
  i4->x = 3;
  fatal(list_push, B, i4);
  fatal(validate_contents, B, 2, 2, 3);

  fatal(list_splice, A, 0, B, 0, 2);
  fatal(validate_contents, A, 2, 2, 3);
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
  fatal(list_push, A, i1);
  fatal(validate_contents, A, 1, 1);

  fatal(xmalloc, &i2, sizeof(*i2));
  i2->x = 4;
  fatal(list_push, A, i2);
  fatal(validate_contents, A, 2, 1, 4);

  fatal(xmalloc, &i3, sizeof(*i3));
  i3->x = 2;
  fatal(list_push, B, i3);
  fatal(validate_contents, B, 1, 2);

  fatal(xmalloc, &i4, sizeof(*i4));
  i4->x = 3;
  fatal(list_push, B, i4);
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
  fatal(test_set);
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
    xapi_backtrace();
conclude(&R);
  xapi_teardown();

  return !!R;
}
