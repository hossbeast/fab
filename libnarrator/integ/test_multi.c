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
#include "narrator/multi.h"
#include "narrator/fixed.h"

#include "test_util.h"

xapi validate(int count, ...)
{
  enter;

  va_list va;
  va_start(va, count);

  int x;
  for(x = 0; x < count; x++)
  {
    narrator * n = va_arg(va, narrator*);

    assertf(strcmp("hello world", n->fixed.s) == 0, "%s", "%s", "hello world", n->fixed.s);
  }

finally:
  va_end(va);
  xapi_infof("narrator", "%d", x);
coda;
}

xapi test_basic()
{
  enter;

  narrator * N = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;
  fatal(narrator_multi_create, &N);
  fatal(narrator_fixed_create, &N0, 2048);
  fatal(narrator_fixed_create, &N1, 2048);

  fatal(narrator_multi_add, N, N0);
  fatal(narrator_multi_add, N, N1);

  says("hello world");

  fatal(validate, 2, N0, N1);

finally:
  narrator_free(N);
  narrator_free(N0);
  narrator_free(N1);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_basic);

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
conclude(&R);

  xapi_teardown();
  return !!R;
}
