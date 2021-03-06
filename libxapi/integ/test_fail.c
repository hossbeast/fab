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

SUMMARY
 fail, unwind, ensure the error code and table are propagated correctly

*/

static xapi alpha()
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi beta()
{
  enter;

  fails(TEST_ERROR_ONE, "foo", "bar");

  finally : coda;
}

static xapi zeta()
{
  enter;

  failf(TEST_ERROR_ONE, "foo", "%s", "bar");

  finally : coda;
}

static xapi test_fail()
{
  enter;

  fatal(alpha);

  finally : coda;
}

static xapi test_fails()
{
  enter;

  fatal(beta);

  finally : coda;
}

static xapi test_failf()
{
  enter;

  fatal(zeta);

  finally : coda;
}

static xapi test_fail_intent()
{
  enter;

#if XAPI_STACKTRACE
  char space[2048];
#endif

  xapi_info_pushs("foo", "bar");
  xapi_info_pushs("baz", "qux");
  fail(TEST_ERROR_ONE);

finally:
#if XAPI_STACKTRACE
  // ensure that the trace contains both infos
  xapi_trace_info("foo", space, sizeof(space));
  assert_eq_s("bar", space);

  xapi_trace_info("baz", space, sizeof(space));
  assert_eq_s("qux", space);
#endif
coda;
}

int main()
{
  // invoke the function, collect its exit status
  xapi exit = test_fail();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  exit = test_fails();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  exit = test_failf();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  exit = test_fail_intent();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  // victory
  succeed;
}
