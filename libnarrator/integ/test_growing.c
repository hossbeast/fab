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

#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "narrator/growing.h"

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
}

static void test_basic()
{
  narrator_growing * N = 0;
  narrator_growing_create(&N);
  say(&N->base);

  char * expected = "40 41 42 43 44";
  size_t expectedl = strlen(expected);

  assert_eq_s(expected, N->s);
  assert_eq_zu(expectedl, N->l);

  narrator_growing_free(N);
}

static void test_seek()
{
  narrator_growing * ng = 0;
  narrator *N = 0;
  narrator_growing_create(&ng);
  N = &ng->base;

  xsays("H");

  narrator_xseek(N, 2, NARRATOR_SEEK_SET, 0);
  xsays("L");

  narrator_xseek(N, 1, NARRATOR_SEEK_SET, 0);
  xsays("E");

  narrator_xseek(N, 4, NARRATOR_SEEK_SET, 0);
  xsays("O");

  narrator_xseek(N, 3, NARRATOR_SEEK_SET, 0);
  xsays("L");

  assert_eq_s("HELLO", ng->s);

  narrator_growing_free(ng);
}

static void test_init()
{
  narrator * N = 0;
  narrator_growing ng = { 0 };
  char * buf = 0;

  N = narrator_growing_init(&ng);

  xsays("hello");
  xsays(" ");
  xsays("world");

  narrator_growing_claim_buffer(&ng, &buf, 0);

  assert_eq_s("hello world", buf);

  narrator_growing_destroy(&ng);
  wfree(buf);
}

int main()
{
  test_basic();
  test_seek();
  test_init();

  summarize;

  return 0;
}
