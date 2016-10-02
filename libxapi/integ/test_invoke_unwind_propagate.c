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

#include "xapi.h"
#include "xapi/trace.h"

#include "test.h"

/*

SUMMARY
 use invoke/unwind to capture, inspect, and propagate an error

*/

static xapi beta()
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi alpha()
{
  enter;

#if XAPI_STACKTRACE
  char space[4096];
  size_t z;
#endif

  int exit;
  if((exit = invoke(beta)))
  {
    assert_exit(TEST_ERROR_ONE, exit);

#if XAPI_STACKTRACE
    z = xapi_trace_full(space, sizeof(space));
    write(1, space, z);
    write(1, "\n", 1);
#endif

    // propagate
    fail(exit);
  }

  finally : coda;
}

int main()
{
#if XAPI_STACKTRACE
  xapi_errtab_register(perrtab_TEST);
#endif

  xapi exit = alpha();
  assert_exit(TEST_ERROR_ONE, exit);

  succeed;
}
