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
  // this frame is not failing
  xapi_infos("epsilon", "epsilon");
coda;
}

static xapi delta()
{
  enter;

  fatal(epsilon);
  fail(TEST_ERROR_TWO);

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

  // delta fails, alpha terminates
  fatal(delta);

  // this line is not reached
  xapi_infos("alpha2", "alpha2");
coda;
}

static xapi zeta()
{
  enter;

#if XAPI_STACKTRACE
  char space[2048];
#endif

  fatal(alpha);

finally:
  xapi_infos("zeta", "zeta");

#if XAPI_STACKTRACE
  size_t info_len = xapi_trace_info("epsilon", space, sizeof(space));
  assert_eq_zu(0, info_len);

  info_len = xapi_trace_info("alpha2", space, sizeof(space));
  assert_eq_zu(0, info_len);

  xapi_trace_full(space, sizeof(space));
  assertf(sameline(space, "ERROR_ONE", "alpha1=alpha1"), "expected kvp alpha1=alpha1, actual trace\n**\n%s\n**\n", space);
  assertf(sameline(space, "ERROR_ONE", "beta=beta"), "expected kvp beta=beta, actual trace\n**\n%s\n**\n", space);
  assertf(sameline(space, "ERROR_ONE", "zeta=zeta"), "expected kvp zeta=zeta, actual trace\n**\n%s\n**\n", space);
  assertf(sameline(space, "ERROR_TWO", "delta=delta"), "expected kvp delta=delta, actual trace\n**\n%s\n**\n", space);
#endif
coda;
}

int main()
{
  // alpha should propagate the error from beta
  xapi exit = zeta();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  succeed;
}
