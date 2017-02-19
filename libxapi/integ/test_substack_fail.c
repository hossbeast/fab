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

static xapi kappa()
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi baz()
{
  enter;

  fatal(kappa);

  finally : coda;
}

static xapi bar()
{
  enter;

  fatal(baz);

  finally : coda;
}

static int delta_count;
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

static int epsilon_count;
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

static int beta_count;
static xapi beta()
{
  enter;

  beta_count++;
  fatal(delta);

  finally : coda;
}

static int alpha_dead;
static xapi alpha()
{
  enter;

  // beta calls delta, which fails
  fatal(beta);

finally :
  // epsilon fails, alpha terminates
  fatal(epsilon);

  // this line should not be hit
  alpha_dead = 1;
coda;
}

static xapi zeta()
{
  enter;

  fatal(alpha);

  finally : coda;
}

static int fi_live;
static int fi_dead;
static xapi fi()
{
  enter;

  fi_live++;
  fail(TEST_ERROR_TWO);
  fi_dead++;

  finally : coda;
}

static int lambda_live_one;
static int lambda_dead_one;
static int lambda_live_two;
static int lambda_dead_two;
static xapi lambda()
{
  enter;

  lambda_live_one++;
  fail(TEST_ERROR_ONE);
  lambda_dead_one++;

finally:
  lambda_live_two++;
  fatal(fi);
  lambda_dead_two++;
coda;
}

int main()
{
  // alpha should propagate the error from epsilon
  xapi exit = zeta();
  assert_exit(TEST_ERROR_ONE, exit);

  assertf(alpha_dead == 0, "expected alpha-dead : 0, actual : %d", alpha_dead);
  assertf(beta_count == 1, "expected beta-count : 1, actual : %d", beta_count);
  assertf(delta_count == 1, "expected delta-count : 1, actual : %d", delta_count);
  assertf(epsilon_count == 1, "expected epsilon-count : 1, actual : %d", epsilon_count);

  // lambda has a subsequence rooted at frame 0
  exit = lambda();
  assert_exit(TEST_ERROR_ONE, exit);

  assertf(fi_live == 1, "expected fi-live : 1, actual : %d", fi_live);
  assertf(fi_dead == 0, "expected fi-dead : 0, actual %d", fi_dead);
  assertf(lambda_live_one == 1, "expected lambda-live-one : 1, actual %d", lambda_live_one);
  assertf(lambda_dead_one == 0, "expected lambda-dead-one : 0, actual %d", lambda_dead_one);
  assertf(lambda_live_two == 1, "expected lambda-live-two : 1, actual %d", lambda_live_two);
  assertf(lambda_dead_two == 0, "expected lambda-dead-two : 0, actual %d", lambda_dead_two);

  succeed;
}
