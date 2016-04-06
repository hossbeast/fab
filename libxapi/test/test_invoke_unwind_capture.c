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

xapi delta()
{
  enter;

  fail(TEST_ERROR_TWO);

  finally : coda;
}

xapi beta()
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

static int alpha_dead_count;
xapi alpha()
{
  enter;

  int exit;
  if((exit = invoke(beta)))
  {
    assert_exit(exit, perrtab_TEST, TEST_ERROR_ONE);

#if XAPI_STACKTRACE_INCL
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
#if XAPI_STACKTRACE_INCL
  xapi_errtab_register(perrtab_TEST);
#endif

  int x;
  for(x = 0; x < 3; x++)
  {
    // alpha calls beta, unwinds, and calls delta, which fails with NOFATAL
    xapi exit = alpha();
    assert_exit(exit, perrtab_TEST, TEST_ERROR_TWO);

    // dead area should have been skipped
    assertf(alpha_dead_count == 0
      , "expected alpha-dead-count : 0, actual alpha-dead-count : %d"
      , alpha_dead_count
    );
  }

  // victory
  succeed;
}
