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

#include <stdio.h>

#include "xapi.h"
#include "xapi/errtab.h"
#include "xapi/XAPI.errtab.h"
#include "xlinux.h"
#include "narrator.h"
#include "logger.h"

#include "internal.h"
#include "errtab/PCRE.errtab.h"
#include "errtab/LISTWISE.errtab.h"
#include "operators.internal.h"
#include "lwx.internal.h"
#include "object.internal.h"

#define restrict __restrict

static int handles;

//
// api
//

API xapi listwise_load()
{
  enter;

  if(handles == 0)
  {
    // dependencies
    fatal(xlinux_load);
    fatal(narrator_load);
    fatal(logger_load);

    // modules
    fatal(xapi_errtab_register, perrtab_PCRE);
    fatal(xapi_errtab_register, perrtab_LISTWISE);
    fatal(operators_setup);
    fatal(lwx_setup);
  }
  handles++;

  finally : coda;
}

API xapi listwise_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    object_teardown();
    lwx_teardown();
    fatal(operators_release);

    // dependencies
    fatal(xlinux_unload);
    fatal(narrator_unload);
    fatal(logger_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_XAPI, XAPI_AUNLOAD, "library", "liblistwise");
  }

  finally : coda;
}

// * my dependencies are not unloaded until I am unloaded
// * failure to load will lead will not increment handles, and unload will be no-op
// * I can only unload if I and my dependencies loaded successfully
