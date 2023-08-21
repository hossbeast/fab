/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <stdarg.h>


#include "narrator.h"
#include "narrator/multi.h"
#include "narrator/growing.h"

#include "unit.h"

static void validate(int count, ...)
{
  va_list va;
  va_start(va, count);

  int x;
  for(x = 0; x < count; x++)
  {
    narrator_growing * n = va_arg(va, narrator_growing*);
    assert_eq_s("hello world", n->s);
  }

  va_end(va);
}

static void test_basic()
{
  narrator_multi * N = 0;
  narrator_growing * N0 = 0;
  narrator_growing * N1 = 0;
  narrator_multi_create(&N);
  narrator_growing_create(&N0);
  narrator_growing_create(&N1);

  narrator_multi_add(N, &N0->base);
  narrator_multi_add(N, &N1->base);

  narrator_says(&N->base, "hello world");

  validate(2, N0, N1);

  narrator_multi_free(N);
  narrator_growing_free(N0);
  narrator_growing_free(N1);
}

int main()
{
  test_basic();

  summarize;
  return 0;
}
