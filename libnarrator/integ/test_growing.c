/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

  narrator_seek(N, 2, NARRATOR_SEEK_SET, 0);
  xsays("L");

  narrator_seek(N, 1, NARRATOR_SEEK_SET, 0);
  xsays("E");

  narrator_seek(N, 4, NARRATOR_SEEK_SET, 0);
  xsays("O");

  narrator_seek(N, 3, NARRATOR_SEEK_SET, 0);
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
  free(buf);
}

int main()
{
  test_basic();
  test_seek();
  test_init();

  summarize;

  return 0;
}
