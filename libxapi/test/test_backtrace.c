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

/*

SUMMARY
 call fail in a finally block during unwinding

*/

int delta_count;
int delta(int num)
{
  enter;

  delta_count++;
  fail(SYS_ERESTART);

finally:
  XAPI_INFOF("num", "%d", num);
coda;
}

int beta_count;
int beta_dead_count;
int beta()
{
  enter;

  beta_count++;
	fatal(delta, __LINE__);

finally:
  // delta should fail, and beta should terminate
  fatal(delta, __LINE__);

  // this line should not be hit
  beta_dead_count = 1;
coda;
}

int alpha()
{
  enter;

  char space[4096];
  size_t z;

	fatal(beta);

finally:
  z = xapi_trace_full(space, sizeof(space));
  write(1, space, z);
  write(1, "\n", 1);
coda;
}

int main()
{
  // alpha should fail
  int exit = alpha();
  assert_etab(perrtab_SYS);
  assert_code(SYS_ERESTART);

  // dead area should have been skipped
  assert(beta_dead_count == 0
    , "expected beta-dead-count : 0, actual beta-dead-count : %d"
    , beta_dead_count
  );

  // beta should have been run once
  assert(beta_count == 1
    , "expected beta-count : 1, actual beta-count : %d"
    , beta_count
  );

  // delta should have been run twice
  assert(delta_count == 2
    , "expected delta-count : 2, actual delta-count : %d"
    , delta_count
  );

  succeed;
}
