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

#include <stdio.h>


#include "narrator.h"
#include "narrator/fixed.h"

#include "test_util.h"

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
