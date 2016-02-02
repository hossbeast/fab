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
 fatal call a function while unwinding which fails

*/

int delta_count;
xapi delta()
{
  enter;

  delta_count++;
  fail(XAPI_ILLFATAL);

  finally : coda;
}

int epsilon_count;
xapi epsilon()
{
  enter;

  epsilon_count++;
  fail(XAPI_NOFATAL);

  finally : coda;
}

int beta_count;
xapi beta()
{
  enter;

  beta_count++;
  fatal(delta);

  finally : coda;
}

int alpha_dead_count;
xapi alpha()
{
  enter;

  // beta calls delta, which fails
  fatal(beta);

finally :
  // epsilon fails, alpha terminates
  fatal(epsilon);

  // this line should not be hit
  alpha_dead_count = 1;
coda;
}

xapi zeta()
{
  enter;

#if XAPI_MODE_STACKTRACE
  char space[4096];
  size_t z;
#endif

  fatal(alpha);

finally:
#if XAPI_MODE_STACKTRACE
  z = xapi_trace_full(space, sizeof(space));
  write(1, space, z);
  write(1, "\n", 1);
#endif
coda;
}

int main()
{
  // alpha should propagate the error from epsilon
  int exit = zeta();
  assert_exit(exit, perrtab_XAPI, XAPI_NOFATAL);

  // alpha dead area should have been skipped
  assert(alpha_dead_count == 0
    , "expected alpha-dead-count : 0, actual alpha-dead-count : %d"
    , alpha_dead_count
  );

  // beta should have been run once
  assert(beta_count == 1
    , "expected beta-count : 1, actual beta-count : %d"
    , beta_count
  );

  // delta should have been run once
  assert(delta_count == 1
    , "expected delta-count : 1, actual delta-count : %d"
    , delta_count
  );

  // epsilon should have been run once
  assert(epsilon_count == 1
    , "expected epsilon-count : 1, actual epsilon-count : %d"
    , epsilon_count
  );

  succeed;
}
