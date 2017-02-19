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
 fatal call a function while unwinding which does not fail, assert the proper call path

*/

int delta_count;
static xapi delta()
{
  enter;

  delta_count++;
  fail(TEST_ERROR_ONE);

  finally : coda;
}

int beta_count;
static xapi beta()
{
  enter;

  beta_count++;

  finally : coda;
}

int alpha_finally_count;
static xapi alpha()
{
  enter;

  // delta fails
  fatal(delta);

finally :
  // beta does not fail
  fatal(beta);

  // this line should be executed
  alpha_finally_count = 1;
coda;
}

static xapi zeta()
{
  enter;

  fatal(alpha);

  finally : coda;
}

int main()
{
  // zeta fails
  xapi exit = zeta();
  assert_exit(TEST_ERROR_ONE, exit);

  // alpha dead area should have been skipped
  assertf(alpha_finally_count == 1
    , "expected alpha-finally-count : 1, actual alpha-finally-count : %d"
    , alpha_finally_count
  );

  // beta should have been run once
  assertf(beta_count == 1
    , "expected beta-count : 1, actual beta-count : %d"
    , beta_count
  );

  // delta should have been run twice
  assertf(delta_count == 1
    , "expected delta-count : 1, actual delta-count : %d"
    , delta_count
  );

  succeed;
}
