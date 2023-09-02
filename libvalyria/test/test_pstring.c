/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pstring.h"

#include "unit.h"

static void validate(char ** ps, size_t *len, size_t *a)
{
  assert_gt_zu(*len, *a);
}

static void assert_contents(char ** ps, size_t *len, size_t *a)
{
  validate(ps, len, a);
  assert_eq_zu(10, *len);

  assert_eq_s("1234567890", *ps);
}

static void test_basic()
{
  char *ps;
  size_t len;
  size_t a;

  psinit(&ps, &len, &a);
  psloads(&ps, &len, &a, "1234567890");
  assert_contents(&ps, &len, &a);

  psclear(&ps, &len, &a);
  psloadc(&ps, &len, &a, '1');
  pscatf(&ps, &len, &a, "%s", "234567890");
  assert_contents(&ps, &len, &a);

  psdestroy(&ps, &len, &a);
}

static void test_load()
{
  char *ps;
  size_t len;
  size_t a;
  int x;

  psinit(&ps, &len, &a);

  for(x = 0; x < 25; x++)
  {
    pscats(&ps, &len, &a, " "); validate(&ps, &len, &a);
    pscatc(&ps, &len, &a, ' '); validate(&ps, &len, &a);
    pscatf(&ps, &len, &a, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); validate(&ps, &len, &a);
    pscatf(&ps, &len, &a, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); validate(&ps, &len, &a);
    pscatf(&ps, &len, &a, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); validate(&ps, &len, &a);
  }

  psdestroy(&ps, &len, &a);
}

int main()
{
  test_basic();
  test_load();
  summarize;
  return 0;
}
