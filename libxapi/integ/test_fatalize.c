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
 invoke a function with fatalize

*/

xapi err;
static int beta()
{
  err = TEST_ERROR_ONE;
  return 42;
}

static xapi alpha()
{
  enter;

  tfatalize(perrtab_TEST, err, beta);

  finally : coda;
}

static xapi test_fatalize()
{
  enter;

  fatal(alpha);

  finally : coda;
}

int main()
{
  // invoke the function, collect its exit status
  xapi exit = test_fatalize();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  // victory
  succeed;
}
