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
#include "pstring.h"

#include "test.h"
#include "internal.h"

#include "narrator.h"
#include "narrator/growing.h"

xapi say(narrator * const N)
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

xapi test_basic()
{
  enter;

  narrator * N = 0;
  fatal(narrator_growing_create, &N);
  fatal(say, N);

  char * expected = "40 41 42 43 44";
  size_t expectedl = strlen(expected);

  assertf(strcmp(N->growing.ps->s, expected) == 0, "expected '%s', actual '%s'", expected, N->growing.ps->s);
  assertf(N->growing.ps->l == expectedl, "expected written : %zu, actual : %zu", expectedl, N->fixed.bz);

finally:
  narrator_free(N);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_basic);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
conclude(&R);

  return !!R;
}
