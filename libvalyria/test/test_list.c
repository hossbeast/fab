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

struct item;
#define LIST_ELEMENT_TYPE struct item
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

    if(B->x <= A->x)
      tfail(perrtab_TEST, TEST_FAIL);
  }

  finally : coda;
}

static xapi test_basic()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  item * itemps[2];

  fatal(list_create, &listp, (void*)free);

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
  list_free(listp);
  free(itemp);
coda;
}

static xapi test_load()
{
  enter;

  list * listp = 0;
  fatal(list_create, &listp, 0);

  int x;
  for(x = 0; x < 50; x++)
    fatal(list_push, listp, (void*)listp);

  for(x = 0; x < 300; x++)
    fatal(list_unshift, listp, (void*)listp);

  for(x = 0; x < 35; x++)
    fatal(list_insert, listp, x, (void*)listp);

finally:
  list_free(listp);
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
    list_ifree(&listp);
    fatal(list_create, &listp, (void*)free);

    int y;
    for(y = 0; y < sentinel(items[x]); y++)
    {
      size_t lx = 0;
      int lc = 0;

      int compar(void * ud, const item * el, size_t idx)
      {
        lx = idx;
        return lc = itemp->x - el->x;
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
  list_free(listp);
  free(itemp);
coda;
}

/// test_set
//
// SUMMARY
//  tests list_set
//
static xapi test_set()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  item * itemps[3];

  fatal(list_create, &listp, (void*)free);

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
  list_free(listp);
  free(itemp);
coda;
}

int main()
{
  enter;
  xapi r;

  fatal(test_basic);
  fatal(test_load);
  fatal(test_insertion_sort);
  fatal(test_set);

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }

conclude(&r);

  xapi_teardown();
  return !!r;
}
