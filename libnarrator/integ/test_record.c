/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <stdarg.h>

#include "narrator.h"
#include "narrator/record.h"
#include "narrator/growing.h"

#include "test_util.h"

static void test_fatal()
{
  narrator * N;
  narrator_growing * ng = 0;
  narrator_record *nr = 0;

  narrator_growing_create(&ng);
  narrator_record_create(&nr, &ng->base);
  N = &nr->base;

  xsays("hello");
  xsayc(' ');
  xsays("world");
  narrator_record_flush(nr);

  assert_eq_s("hello world", ng->s);

  narrator_record_free(nr);
  narrator_growing_free(ng);
}

int main()
{
  test_fatal();
  summarize;
}
