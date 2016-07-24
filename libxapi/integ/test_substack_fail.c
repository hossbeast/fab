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
 while unwinding, fatal call a function that itself fails, assert the proper call path

*/

static xapi ababab()
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi baz()
{
  enter;

  fatal(ababab);

  finally : coda;
}

static xapi bar()
{
  enter;

  fatal(baz);

  finally : coda;
}

int delta_count;
static xapi delta()
{
  enter;

  delta_count++;
  fatal(bar);

  finally : coda;
}

static xapi qux()
{
  enter;

  fail(TEST_ERROR_TWO);

  finally : coda;
}

int epsilon_count;
static xapi epsilon()
{
  enter;

  char name[333];
  sprintf(name, "epsilon_%d", epsilon_count);

  epsilon_count++;
  fatal(qux);

finally:
  fatal(qux);
coda;
}

int beta_count;
static xapi beta()
{
  enter;

  beta_count++;
  fatal(delta);

  finally : coda;
}

int alpha_dead_count;
static xapi alpha()
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

static xapi zeta()
{
  enter;

  fatal(alpha);

  finally : coda;
}

int main()
{
#if XAPI_STACKTRACE
  xapi_errtab_register(perrtab_TEST);
#endif

  // alpha should propagate the error from epsilon
  xapi exit = zeta();
  assert_exit(exit, perrtab_TEST, TEST_ERROR_ONE);

  // alpha dead area should have been skipped
  assertf(alpha_dead_count == 0
    , "expected alpha-dead-count : 0, actual alpha-dead-count : %d"
    , alpha_dead_count
  );

  // beta should have been run once
  assertf(beta_count == 1
    , "expected beta-count : 1, actual beta-count : %d"
    , beta_count
  );

  // delta should have been run once
  assertf(delta_count == 1
    , "expected delta-count : 1, actual delta-count : %d"
    , delta_count
  );

  // epsilon should have been run once
  assertf(epsilon_count == 1
    , "expected epsilon-count : 1, actual epsilon-count : %d"
    , epsilon_count
  );

  succeed;
}
