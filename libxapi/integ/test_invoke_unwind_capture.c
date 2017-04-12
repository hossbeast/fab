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
#include "xapi/calltree.h"

/*

SUMMARY
 use invoke/unwind to capture, inspect, and discard an error

*/

static xapi delta()
{
  enter;

  fail(TEST_ERROR_TWO);

  finally : coda;
}

static xapi beta()
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

static int alpha_dead_count;
static xapi alpha()
{
  enter;

  int exit;
  if((exit = invoke(beta)))
  {
    assert_eq_exit(TEST_ERROR_ONE, exit);

#if XAPI_STACKTRACE
    // discard
    xapi_calltree_unwind();
#endif
  }

  fatal(delta);

  alpha_dead_count++;

  finally : coda;
}

int main()
{
  int x;
  for(x = 0; x < 3; x++)
  {
    // alpha calls beta, unwinds, and calls delta, which fails with NOFATAL
    xapi exit = alpha();
    assert_eq_exit(TEST_ERROR_TWO, exit);

    // dead area should have been skipped
    assert_eq_d(0, alpha_dead_count);
  }

  // victory
  succeed;
}
