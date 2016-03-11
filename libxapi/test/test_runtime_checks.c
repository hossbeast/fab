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
 verify that runtime checks catch ILLFATAL/NOFATAL/ILLFAIL errors

*/

xapi gamma_xapi()
{
  enter;

  finally : coda;
}

xapi beta_xapi()
{
  enter;

  gamma_xapi();

  finally : coda;
}

int alpha_notxapi()
{
  return 1;
}

xapi foo_illfatal()
{
  enter;

  fatal(alpha_notxapi);

  finally : coda;
}

xapi foo_nofatal()
{
  enter;

  fatal(beta_xapi);

  finally : coda;
}

#if XAPI_MODE_STACKTRACE && XAPI_RUNTIME_CHECKS
xapi foo_illfail_noetab()
{
  enter;

  tfail(0, XAPI_ILLFATAL);

  finally : coda;
}

xapi foo_illfail_nocode()
{
  enter;

  tfail(perrtab_XAPI, 0);

  finally : coda;
}
#endif

int main()
{
#if XAPI_MODE_STACKTRACE && XAPI_RUNTIME_CHECKS
  // verify NOFATAL
  xapi exit = foo_nofatal();
  assert_exit(exit, perrtab_XAPI, XAPI_NOFATAL);

  // verify ILLFATAL
  exit = foo_illfatal();
  assert_exit(exit, perrtab_XAPI, XAPI_ILLFATAL);

  exit = foo_illfail_noetab();
  assert_exit(exit, perrtab_XAPI, XAPI_ILLFAIL);

  exit = foo_illfail_nocode();
  assert_exit(exit, perrtab_XAPI, XAPI_ILLFAIL);
#endif

  // victory
  succeed;
}
