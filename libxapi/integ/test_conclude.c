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
  exercise the conclude macro

*/

static xapi beta()
{
  enter;

  finally : coda;
}

static xapi alpha()
{
  enter;

  fatal(beta);

  finally : coda;
}

static uint32_t r;
static int foo()
{
  enter;

  fatal(alpha);

  finally : conclude(&r);
  return r;
}

int main()
{
  xapi exit = foo();
  assert_eq_exit(0, exit);

  succeed;
}
