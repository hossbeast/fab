/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>


#include "narrator.h"
#include "narrator/fixed.h"

#include "unit.h"

static void say(narrator * const N)
{
  // says the string : 40 41 42 43 44
  xsayf("%d", 40);
  xsays(" 41");
  xsayw(" 42", 3);
  xsayf(" %d", 43);
  xsayf(" %d", 4);
  xsayc('4');
  narrator_flush(N);
}

static void test_basic()
{
  char s[64];
  narrator_fixed nf;
  narrator *N;

  narrator_fixed_init(&nf, s, sizeof(s));
  N = &nf.base;

  say(N);

  char * expected = "40 41 42 43 44";
  size_t expectedl = strlen(expected);

  assert_eq_s(expected, nf.s);
  assert_eq_zu(expectedl, nf.l);
}

static void test_constrained()
{
  char s[7];
  narrator_fixed nf;

  narrator * N = narrator_fixed_init(&nf, s, sizeof(s));
  say(N);

  char * expected = "40 41 ";
  size_t expectedl = 6;

  assert_eq_zu(expectedl, nf.l);
  assert_eq_s(expected, nf.s);
}

int main()
{
  test_basic();
  test_constrained();

  summarize;
}
