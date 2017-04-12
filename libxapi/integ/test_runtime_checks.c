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
#include "xapi/XAPI.errtab.h"

#include "xapi/trace.h"

/*

SUMMARY
 verify that runtime checks catch ILLFATAL/NOFATAL/NOCODE/NOTABLE errors

*/

#if XAPI_MODE_STACKTRACE_CHECKS
static xapi gamma_xapi()
{
  enter;

  finally : coda;
}

static xapi beta_xapi()
{
  enter;

  gamma_xapi();

  finally : coda;
}

static int alpha_notxapi()
{
  return 1;
}

static xapi test_illfatal()
{
  enter;

  char space[2048];

  fatal(alpha_notxapi);

finally:
  // ensure that the trace contains the function name
  xapi_trace_full(space, sizeof(space));
  assertf(strstr(space, "function=alpha_notxapi"), "expected function, actual trace\n**\n%s\n**\n", space);

  xapi_trace_pithy(space, sizeof(space));
  assertf(strstr(space, "function=alpha_notxapi"), "expected function, actual trace\n**\n%s\n**\n", space);
coda;
}

static xapi test_nofatal()
{
  enter;

  char space[2048];

  fatal(beta_xapi);

finally:
  // ensure that the trace contains the function name
  xapi_trace_full(space, sizeof(space));
  assertf(strstr(space, "function=gamma_xapi"), "expected function, actual trace\n**\n%s\n**\n", space);

  xapi_trace_pithy(space, sizeof(space));
  assertf(strstr(space, "function=gamma_xapi"), "expected function, actual trace\n**\n%s\n**\n", space);
coda;
}
#endif

int main()
{
#if XAPI_MODE_STACKTRACE_CHECKS
  // verify NOFATAL
  xapi exit = test_nofatal();
  assert_eq_exit(XAPI_NOFATAL, exit);

  // verify ILLFATAL
  exit = test_illfatal();
  assert_eq_exit(XAPI_ILLFATAL, exit);
#endif

  // victory
  succeed;
}
