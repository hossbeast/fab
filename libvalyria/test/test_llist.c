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

#include "llist.h"
#include "test_util.h"

typedef struct item
{
  int a;
  int x;
  llist lln;
} item;

static xapi test_append()
{
  enter;

  llist list;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  llist_append(&list, &i1, lln);
  assert_eq_zu(1, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_append(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };


  llist_append(&list, &i3, lln);
  assert_eq_zu(3, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };

  finally : coda;
}

static xapi test_prepend()
{
  enter;

  llist list;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  llist_prepend(&list, &i1, lln);
  assert_eq_zu(1, llist_count(&list));

  x = 1;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_prepend(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  x = 2;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x--;
  };

  llist_prepend(&list, &i3, lln);
  assert_eq_zu(3, llist_count(&list));

  x = 3;
  llist_foreach(&list, e, lln) {
    assert_eq_d(x, e->x);
    x--;
  };

  finally : coda;
}

static xapi test_delete()
{
  enter;

  llist list;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_prepend(&list, &i1, lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1, lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_prepend(&list, &i1, lln);
  llist_prepend(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i1, lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i2, lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_prepend(&list, &i1, lln);
  llist_prepend(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i2, lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1, lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_append(&list, &i1, lln);
  llist_prepend(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i2, lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1, lln);
  assert_eq_zu(0, llist_count(&list));

  //
  llist_append(&list, &i1, lln);
  llist_prepend(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  llist_delete(&i2, lln);
  assert_eq_zu(1, llist_count(&list));

  llist_delete(&i1, lln);
  assert_eq_zu(0, llist_count(&list));

  finally : coda;
}

static xapi test_splice_head()
{
  enter;

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
  llist_append(&A, &i1, lln);
  llist_append(&A, &i2, lln);
  llist_append(&A, &i3, lln);

  e = llist_first(&A, typeof(*e), lln);
  assert_eq_d(1, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(2, e->x);

  e = llist_next(&A, e, lln);
  assert_eq_d(3, e->x);

  B = LLIST_INITIALIZER(B);
  llist_append(&B, &i4, lln);
  llist_append(&B, &i5, lln);
  llist_append(&B, &i6, lln);

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

  finally : coda;
}

static xapi test_splice_tail()
{
  enter;

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
  llist_append(&A, &i1, lln);
  llist_append(&A, &i2, lln);
  llist_append(&A, &i3, lln);

  B = LLIST_INITIALIZER(B);
  llist_append(&B, &i4, lln);
  llist_append(&B, &i5, lln);
  llist_append(&B, &i6, lln);

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

  finally : coda;
}

static xapi test_safe()
{
  enter;

  llist list;
  llist *T;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  list = LLIST_INITIALIZER(list);
  assert_eq_zu(0, llist_count(&list));

  llist_append(&list, &i1, lln);
  assert_eq_zu(1, llist_count(&list));

  x = 1;
  llist_foreach_safe(&list, e, lln, T) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_append(&list, &i2, lln);
  assert_eq_zu(2, llist_count(&list));

  x = 1;
  llist_foreach_safe(&list, e, lln, T) {
    assert_eq_d(x, e->x);
    x++;
  };

  llist_append(&list, &i3, lln);
  assert_eq_zu(3, llist_count(&list));

  x = 1;
  llist_foreach_safe(&list, e, lln, T) {
    assert_eq_d(x, e->x);
    x++;
  };

  finally : coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_append);
  fatal(test_prepend);
  fatal(test_delete);
  fatal(test_splice_head);
  fatal(test_splice_tail);
  fatal(test_safe);

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
