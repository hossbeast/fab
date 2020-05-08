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

#include "internal.h"
#include "narrator/record.h"
#include "narrator/growing.h"

#include "test_util.h"

static xapi test_fatal()
{
  enter;

  narrator * N = 0;
  narrator * N0 = 0;
  fatal(narrator_growing_create, &N0);
  fatal(narrator_record_create, &N, N0);

  xsays("hello");
  xsayc(' ');
  xsays("world");
  fatal(narrator_record_flush, N);

  assert_eq_s("hello world", N0->growing.s);

finally:
  fatal(narrator_xfree, N);
  fatal(narrator_xfree, N0);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_fatal);

finally:
  summarize;
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
conclude(&R);

  xapi_teardown();
  return !!R;
}
