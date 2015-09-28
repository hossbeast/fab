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

#include "xapi.h"
#include "xapi/errtab.h"
#include "xlinux.h"

#include "internal.h"
#include "listwise/PCRE.errtab.h"
#include "listwise/LW.errtab.h"

#define restrict __restrict

//
// api
//

API xapi listwise_setup()
{
  enter;

  static int setup;

  if(!setup)
  {
    fatal(xapi_errtab_register, perrtab_PCRE);
    fatal(xapi_errtab_register, perrtab_LW);
    fatal(operators_setup);
    fatal(lwx_setup);

    setup = 1;
  }

  char * foo;
  fatal(xmalloc, &foo, 12);

  finally : coda;
}

API void listwise_teardown()
{
  operators_teardown();
  object_teardown();
  lwx_teardown();
}
