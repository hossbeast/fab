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
#include <fcntl.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "internal.h"
#include "narrator/rolling.h"

static xapi exercise_rolling()
{
  enter;

  narrator * N = 0;
  fatal(narrator_rolling_create, &N, "/tmp/foo", S_IRUSR | S_IWUSR, 45, 2);

  int x;
  for(x = 0; x < 100; x++)
  {
    xsays("foo bar baz qux");
  }

finally:
  fatal(narrator_release, N);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(exercise_rolling);

finally:
  if(XAPI_UNWINDING)
  {
//    xapi_backtrace();
  }
conclude(&R);

  xapi_teardown();
  return !!R;
}
