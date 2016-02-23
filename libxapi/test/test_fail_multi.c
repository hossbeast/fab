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
 call multiple top-level xapi functions in series with errors being thrown

*/

int beta_count;
xapi beta(int num)
{
  enter;

  beta_count++;
  fail(TEST_ERROR_ONE);

  finally : coda;
}

xapi alpha(int num)
{
  enter;

  fatal(beta, num);

  finally : coda;
}

xapi foo()
{
  enter;

  fatal(alpha, 125);

  finally : coda;
}

int main()
{
#if XAPI_MODE_STACKTRACE
  xapi_errtab_register(perrtab_TEST);
#endif

  int expected = 3;
  int x;
  for(x = 0; x < expected; x++)
  {
    int exit = foo();
    assert_exit(exit, perrtab_TEST, TEST_ERROR_ONE);
  }

  assert(beta_count == expected
    , "expected beta-count : %d, actual beta-count : %d"
    , expected, beta_count
  );

  succeed;
}

