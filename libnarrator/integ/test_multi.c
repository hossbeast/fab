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

#include "xapi.h"
#include "xapi/trace.h"

#include "narrator.h"
#include "narrator/multi.h"
#include "narrator/growing.h"

#include "test_util.h"

static xapi validate(int count, ...)
{
  enter;

  va_list va;
  va_start(va, count);

  int x;
  for(x = 0; x < count; x++)
  {
    narrator_growing * n = va_arg(va, narrator_growing*);
    assert_eq_s("hello world", n->s);
  }

finally:
  va_end(va);
  xapi_infof("narrator", "%d", x);
coda;
}

static xapi test_basic()
{
  enter;

  narrator_multi * N = 0;
  narrator_growing * N0 = 0;
  narrator_growing * N1 = 0;
  fatal(narrator_multi_create, &N);
  fatal(narrator_growing_create, &N0);
  fatal(narrator_growing_create, &N1);

  fatal(narrator_multi_add, N, &N0->base);
  fatal(narrator_multi_add, N, &N1->base);

  fatal(narrator_xsays, &N->base, "hello world");

  fatal(validate, 2, N0, N1);

finally:
  fatal(narrator_multi_free, N);
  fatal(narrator_growing_free, N0);
  fatal(narrator_growing_free, N1);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_basic);

finally:
  summarize;
  if(XAPI_UNWINDING)
  {
    xapi_backtrace(2, 0);
  }
conclude(&R);

  xapi_teardown();
  return !!R;
}
