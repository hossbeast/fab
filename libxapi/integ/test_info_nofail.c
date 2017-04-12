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

#include <unistd.h>

#include "test.h"

#include "xapi/trace.h"

/*

SUMMARY
 while unwinding, fatal call a function that itself fails, assert the proper infos in the callstack

*/

static xapi epsilon()
{
  enter;

finally:
  xapi_infos("epsilon", "epsilon");
coda;
}

static xapi delta()
{
  enter;

  fatal(epsilon);

finally:
  xapi_infos("delta", "delta");
coda;
}

static xapi beta()
{
  enter;

  fail(TEST_ERROR_ONE);

finally:
  xapi_infos("beta", "beta");
coda;
}

static xapi alpha()
{
  enter;

  // beta fails
  fatal(beta);

finally :
  xapi_infos("alpha1", "alpha1");

  // delta succeeds
  fatal(delta);

  xapi_infos("alpha2", "alpha2");
coda;
}

static xapi zeta()
{
  enter;

#if XAPI_STACKTRACE
  char space[2048];
  size_t z;
#endif

  fatal(alpha);

finally:
  xapi_infos("zeta", "zeta");

#if XAPI_STACKTRACE
  z = xapi_trace_full(space, sizeof(space));

  assertf(strstr(space, "epsilon") == 0, "expected no epsilon, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(strstr(space, "delta") == 0, "expected no delta, actual trace\n**\n%.*s\n**\n", (int)z, space);

  assertf(sameline(space, "ERROR_ONE", "beta=beta"), "expected kvp beta=beta, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(sameline(space, "ERROR_ONE", "alpha1=alpha1"), "expected kvp alpha1=alpha1, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(sameline(space, "ERROR_ONE", "alpha2=alpha2"), "expected kvp alpha2=alpha2, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(sameline(space, "ERROR_ONE", "zeta=zeta"), "expected kvp zeta=zeta, actual trace\n**\n%.*s\n**\n", (int)z, space);
#endif
coda;
}

int main()
{
  // alpha should propagate the error from epsilon
  xapi exit = zeta();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  succeed;
}
