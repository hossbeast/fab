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
 test calling a top-level xapi function directly and capturing its exit status

*/

xapi beta()
{
  enter;

  fail(XAPI_ILLFATAL);

  finally : coda;
}

xapi alpha()
{
  enter;

  fatal(beta);

  finally : coda;
}

xapi foo()
{
  enter;

  fatal(alpha);

  finally : coda;
}

int main()
{
  // invoke the function, collect its exit status
  int exit = foo();

  // assertions
  assert_exit(exit, perrtab_XAPI, XAPI_ILLFATAL);

  // victory
  succeed;
}
