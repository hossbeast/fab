/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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
