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
