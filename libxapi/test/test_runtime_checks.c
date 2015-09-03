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
#include "xapi/XAPI.errtab.h"

/*

SUMMARY
 verify that runtime checks catch ILLFATAL/NOFATAL errors

*/

int gamma_xapi()
{
  enter;

  finally : coda;
}

int beta_xapi()
{
  enter;

  gamma_xapi();

  finally : coda;
}

int alpha_notxapi()
{
  return 1;
}

int foo_illfatal()
{
  enter;

	fatal(alpha_notxapi);

  finally : coda;
}

int foo_nofatal()
{
  enter;

  fatal(beta_xapi);

  finally : coda;
}

int main()
{
#if XAPI_MODE_STACKTRACE && XAPI_RUNTIME_CHECKS
  // verify NOFATAL
  int exit = foo_nofatal();
  assert_exit(perrtab_XAPI, XAPI_NOFATAL);

  // verify ILLFATAL
  exit = foo_illfatal();
  assert_exit(perrtab_XAPI, XAPI_ILLFATAL);
#endif

  // victory
  succeed;
}
