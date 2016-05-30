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

xapi epsilon()
{
  enter;

finally:
  xapi_infos("epsilon", "epsilon");
coda;
}

xapi delta()
{
  enter;

  fatal(epsilon);
  fail(TEST_ERROR_TWO);

finally:
  xapi_infos("delta", "delta");
coda;
}

xapi beta()
{
  enter;

  fail(TEST_ERROR_ONE);

finally:
  xapi_infos("beta", "beta");
coda;
}

xapi alpha()
{
  enter;

  // beta calls delta, which fails
  fatal(beta);

finally :
  xapi_infos("alpha1", "alpha1");

  // delta fails, alpha terminates
  fatal(delta);

  xapi_infos("alpha2", "alpha2");
coda;
}

xapi zeta()
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
  assertf(strstr(space, "alpha2") == 0, "expected no alpha2, actual trace\n**\n%.*s\n**\n", (int)z, space);

  assertf(strstr(space, "delta(delta=delta)"), "expected kvp delta=delta, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(strstr(space, "beta(beta=beta)"), "expected kvp beta=beta, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(strstr(space, "alpha1=alpha1"), "expected kvp alpha1=alpha1, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(strstr(space, "zeta(zeta=zeta)"), "expected kvp delta=delta, actual trace\n**\n%.*s\n**\n", (int)z, space);
#endif
coda;
}

int main()
{
#if XAPI_STACKTRACE
  xapi_errtab_register(perrtab_TEST);
#endif

  // alpha should propagate the error from beta
  xapi exit = zeta();
  assert_exit(exit, perrtab_TEST, TEST_ERROR_ONE);

  succeed;
}