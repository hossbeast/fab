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
 call fail in a finally block during unwinding, assert the proper call path

*/

int delta_live;
static xapi delta(int num)
{
  enter;

  delta_live++;
  fail(TEST_ERROR_ONE);

finally:
  xapi_infof("num", "%d", num);
coda;
}

int beta_live;
int beta_dead;
static xapi beta()
{
  enter;

  beta_live++;
  fatal(delta, __LINE__);

finally:
  // delta should fail, and beta should terminate
  fatal(delta, __LINE__);

  // this line should not be hit
  beta_dead = 1;
coda;
}

static xapi alpha()
{
  enter;

  fatal(beta);

  finally : coda;
}

int main()
{
  // alpha should fail
  xapi exit = alpha();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  // dead area should have been skipped
  assert_eq_d(0, beta_dead);

  // beta should have been run once
  assert_eq_d(1, beta_live);

  // delta should have been run twice
  assert_eq_d(2, delta_live);

  succeed;
}
