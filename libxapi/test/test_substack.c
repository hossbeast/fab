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

/*

SUMMARY
 call fail in a finally block during unwinding

*/

int delta_count;
int delta()
{
  enter;

  delta_count++;
  fail(SYS_ERESTART);

  finally : coda;
}

int beta_count;
int beta()
{
  enter;

  beta_count++;
	fatal(delta);

  finally : coda;
}

int alpha_dead_count;
int alpha()
{
  enter;

	fatal(beta);

finally :
  // delta should fail, and alpha should terminate
	fatal(delta);

  // this line should not be hit
  alpha_dead_count = 1;
coda;
}

int main()
{
  // alpha should fail
  int exit = alpha();
  assert_etab(perrtab_SYS);
  assert_code(SYS_ERESTART);

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

  // delta should have been run twice
  assert(delta_count == 2
    , "expected delta-count : 2, actual delta-count : %d"
    , delta_count
  );

  succeed;
}
