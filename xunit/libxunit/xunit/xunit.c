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
#include "xapi/XAPI.errtab.h"

#include "internal.h"
#include "XUNIT.errtab.h"
#include "logs.internal.h"

static int handles;

//
// api
//

API xapi xunit_load()
{
  enter;

  if(handles == 0)
  {
    // dependencies
    fatal(logger_load);

    // modules
    fatal(logs_setup);
    fatal(xapi_errtab_register, perrtab_XUNIT);
  }
  handles++;

  finally : coda;
}

API xapi xunit_unload()
{
  enter;

  if(--handles == 0)
  {
    fatal(logger_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_XAPI, XAPI_AUNLOAD, "library", "libxunit");
  }

  finally : coda;
}