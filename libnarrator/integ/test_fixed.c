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

#include "xapi.h"
#include "xapi/trace.h"

#include "internal.h"
#include "narrator/fixed.h"

#include "test_util.h"

static xapi say(narrator * const N)
{
  enter;

  // says the string : 40 41 42 43 44
  sayf("%d", 40);
  says(" 41");
  sayw(" 42", 3);
  sayf(" %d", 43);
  sayf(" %d", 4);
  sayc('4');

  finally : coda;
}

static xapi test_basic()
{
  enter;

  narrator_fixed_storage fixed;
  fixed.s = (char[64]) { };
  fixed.a = 64;

  narrator * N = narrator_fixed_init(&fixed);
  fatal(say, N);

  char * expected = "40 41 42 43 44";
  size_t expectedl = strlen(expected);

  assert_eq_s(expected, N->fixed.s);
  assert_eq_zu(expectedl, N->fixed.l);

  finally : coda;
}

static xapi test_constrained()
{
  enter;

  narrator_fixed_storage fixed;
  fixed.s = (char[7]) { };
  fixed.a = 7;

  narrator * N = narrator_fixed_init(&fixed);
  fatal(say, N);

  char * expected = "40 41 ";
  size_t expectedl = 6;

  assert_eq_s(expected, N->fixed.s);
  assert_eq_zu(expectedl, N->fixed.l);

  finally : coda;
}

int main()
{
  enter;

  xapi R = 0;

  fatal(test_basic);
  fatal(test_constrained);

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
