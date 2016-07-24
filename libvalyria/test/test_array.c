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

#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

struct item;
#define ARRAY_ELEMENT_TYPE struct item
#include "array.h"

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
  for(x = 1; x < ar->l; x++)
  {
    item * A = array_get(ar, x - 1);
    item * B = array_get(ar, x);

    assertf(B->x >= A->x, "B >= A", "%d < %d", B->x, A->x);
  }

  finally : coda;
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
  fatal(array_insert_range, ar, 3, 2, itemps);
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
  array_free(ar);
coda;
}

static xapi test_set()
{
  enter;

  array * ar = 0;
  fatal(array_create, &ar, sizeof(item));

  item * itemps[3];
  fatal(array_push_range, ar, sizeof(itemps) / sizeof(itemps[0]), itemps);
  itemps[0]->x = 1;
  itemps[1]->x = 2;
  itemps[2]->x = 3;

  fatal(validate, ar);

  item * itemp;
  array_set(ar, 0, &itemp);
  itemp->x = 1;
  array_set(ar, 1, &itemp);
  itemp->x = 2;
  array_set(ar, 2, &itemp);
  itemp->x = 3;

  fatal(validate, ar);

  array_set_range(ar, 0, sizeof(itemps) / sizeof(itemps[0]), itemps);
  itemps[0]->x = 10;
  itemps[1]->x = 20;
  itemps[2]->x = 30;

  fatal(validate, ar);

finally:
  array_free(ar);
coda;
}

int main()
{
  enter;
  xapi r;

  fatal(test_basic);
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
