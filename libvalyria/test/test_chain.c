/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include "chain.h"
#include "test_util.h"

typedef struct item
{
  int a;
  int x;
  chain chn;
} item;

static xapi test_add()
{
  enter;

  const chain *T;
  item * e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  chain_init(&i1, chn);
  assert_eq_zu(1, chain_count(&i1, chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(2, x);

  chain_add_head(&i1, &i2, chn);
  assert_eq_zu(2, chain_count(&i1, chn));
  assert_eq_zu(2, chain_count(&i2, chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(3, x);

  chain_add_head(&i1, &i3, chn);
  assert_eq_zu(3, chain_count(&i1, chn));
  assert_eq_zu(3, chain_count(&i2, chn));
  assert_eq_zu(3, chain_count(&i3, chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(4, x);

  finally : coda;
}

static xapi test_splice()
{
  enter;

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

  chain_init(&i1, chn);
  assert_eq_zu(1, chain_count(&i1, chn));

  chain_init(&i2, chn);
  assert_eq_zu(1, chain_count(&i2, chn));

  chain_init(&i3, chn);
  chain_init(&i4, chn);
  chain_splice_tail(&i1, &i2, chn);

  assert_eq_zu(2, chain_count(&i1, chn));
  assert_eq_zu(2, chain_count(&i2, chn));

  // forward traversal
  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(3, x);

  chain_init(&i3, chn);
  chain_add(&i3, &i4, chn);

  chain_splice_tail(&i1, &i3, chn);
  assert_eq_zu(4, chain_count(&i1, chn));
  assert_eq_zu(4, chain_count(&i2, chn));
  assert_eq_zu(4, chain_count(&i3, chn));
  assert_eq_zu(4, chain_count(&i4, chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(5, x);

  chain_init(&i5, chn);
  chain_add(&i5, &i6, chn);
  chain_add(&i5, &i7, chn);
  chain_splice_tail(&i1, &i5, chn);
  assert_eq_zu(7, chain_count(&i1, chn));
  assert_eq_zu(7, chain_count(&i2, chn));
  assert_eq_zu(7, chain_count(&i3, chn));
  assert_eq_zu(7, chain_count(&i4, chn));
  assert_eq_zu(7, chain_count(&i5, chn));
  assert_eq_zu(7, chain_count(&i6, chn));
  assert_eq_zu(7, chain_count(&i7, chn));

  x = 1;
  chain_foreach(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(8, x);

  finally : coda;
}

static xapi test_cursor()
{
  enter;

  const chain *c;
  item *e;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  chain_init(&i1, chn);

  c = 0;
  e = chain_next(&i1, &c, chn); assert_eq_p(&i1, e);
  assert_eq_b(false, chain_has_next(&i1, c, chn));
  e = chain_next(&i1, &c, chn); assert_null(e);
  e = chain_next(&i1, &c, chn); assert_null(e); // idempotent

  chain_add(&i1, &i2, chn);
  chain_add(&i1, &i3, chn);

  c = 0;
  e = chain_next(&i1, &c, chn); assert_eq_p(&i1, e);
  assert_eq_b(true, chain_has_next(&i1, c, chn));
  e = chain_next(&i1, &c, chn); assert_eq_p(&i2, e);
  assert_eq_b(true, chain_has_next(&i1, c, chn));
  e = chain_next(&i1, &c, chn); assert_eq_p(&i3, e);
  assert_eq_b(false, chain_has_next(&i1, c, chn));
  e = chain_next(&i1, &c, chn); assert_null(e);
  e = chain_next(&i1, &c, chn); assert_null(e); // idempotent

  finally : coda;
}

static xapi test_safe()
{
  enter;

  const chain *T[2];
  item *e;
  int x;

  item i1 = { .x = 1 };
  item i2 = { .x = 2 };
  item i3 = { .x = 3 };

  chain_init(&i1, chn);
  assert_eq_zu(1, chain_count(&i1, chn));
  x = 1;
  chain_foreach_safe(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(2, x);

  chain_add(&i1, &i2, chn);
  assert_eq_zu(2, chain_count(&i1, chn));
  x = 1;
  chain_foreach_safe(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(3, x);

  chain_add(&i1, &i3, chn);
  assert_eq_zu(3, chain_count(&i1, chn));
  x = 1;
  chain_foreach_safe(T, e, chn, &i1) {
    assert_eq_d(x, e->x);
    x++;
  }
  assert_eq_d(4, x);

  finally : coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_add);
  fatal(test_splice);
  fatal(test_cursor);
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
    xapi_backtrace();
conclude(&R);
  xapi_teardown();

  return !!R;
}
