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

#include "test.h"
#include "xapi.h"
#include "xapi/trace.h"

/*

BEHAVIORS - info staging

 1. staged infos are only applied to the immediate frame and not to future frames
 2. staged infos may be applied to nested frames

*/

static xapi zeta()
{
  enter;

  xapi_info_pushs("zeta", "zeta");
  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi theta()
{
  enter;

  xapi_info_pushs("theta", "theta");
  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi beta()
{
  enter;

  fatal(theta);

finally:
  fatal(zeta);
coda;
}

static xapi alpha()
{
  enter;

  fatal(beta);

  finally : coda;
}

static xapi test_fail()
{
  enter;

  fatal(alpha);

finally:
#if XAPI_STACKTRACE
  {
    char bar[64];
    xapi_trace_info("theta", bar, sizeof(bar));
    assert_eq_s("theta", bar);

    size_t len = xapi_trace_info("zeta", bar, sizeof(bar));
    assert_eq_zu(0, len);
  }
#endif
coda;
}

int main()
{
  // invoke the function, collect its exit status
  xapi exit = test_fail();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  // victory
  succeed;
}
