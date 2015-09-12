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

int delta()
{
  enter;

  fail(SYS_ENOMEM);

  finally : coda;
}

int beta()
{
  enter;

  fail(SYS_ERESTART);

  finally : coda;
}

static int alpha_dead_count;
int alpha()
{
  enter;

#if XAPI_MODE_STACKTRACE
  char space[4096];
  size_t z;
#endif

  int exit;
  if((exit = invoke(beta)))
  {
    assert_exit(perrtab_SYS, SYS_ERESTART);

#if XAPI_MODE_STACKTRACE
    z = xapi_trace_full(space, sizeof(space));
    write(1, space, z);
    write(1, "\n", 1);

    // disard
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
    int exit = alpha();

    assert_exit(perrtab_SYS, SYS_ENOMEM);

    // dead area should have been skipped
    assert(alpha_dead_count == 0
      , "expected alpha-dead-count : 0, actual alpha-dead-count : %d"
      , alpha_dead_count
    );
  }

  // victory
  succeed;
}