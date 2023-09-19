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
#include <string.h>

#include "pstring.internal.h"

#include "test_util.h"

static void validate(pstring * ps)
{
  assert_gt_zu(ps->size, ps->a);
}

static void assert_contents(pstring * ps)
{
  validate(ps);
  assert_eq_zu(10, ps->size);

  assert_eq_s("1234567890", ps->s);
}

static void test_basic()
{
  pstring * ps = 0;

  pscreate(&ps);
  psloads(ps, "1234567890");
  assert_contents(ps);

  psclear(ps);
  psloadc(ps, '1');
  pscatf(ps, "%s", "234567890");
  assert_contents(ps);

  psfree(ps);
}

static void test_load()
{
  pstring * ps = 0;

  pscreate(&ps);

  int x;
  for(x = 0; x < 25; x++)
  {
    pscats(ps, " "); validate(ps);
    pscatc(ps, ' '); validate(ps);
    pscatf(ps, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); validate(ps);
    pscatf(ps, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); validate(ps);
    pscatf(ps, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); validate(ps);
  }

  psfree(ps);
}

int main()
{
  test_basic();
  test_load();
  summarize;
  return 0;
}
