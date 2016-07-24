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
#include "xlinux/LIB.errtab.h"
#include "logger.h"
#include "narrator.h"

#include "internal.h"
#include "params.internal.h"
#include "logging.internal.h"
#include "identity.internal.h"
#include "FAB.errtab.h"

static int handles;

//
// api
//

API xapi fabcore_load()
{
  enter;

  if(handles == 0)
  {
printf("fabcore_load\n");
    // dependencies
    fatal(xlinux_load);
    fatal(logger_load);
    fatal(narrator_load);

    // modules
    fatal(params_setup);
    fatal(identity_setup);
    fatal(logging_setup);
#if XAPI_STACKTRACE
    fatal(xapi_errtab_register, perrtab_FAB);
#endif
  }
  handles++;

  finally : coda;
}

API xapi fabcore_unload()
{
  enter;

  if(--handles == 0)
  {
printf("fabcore_unload\n");
    // modules
    params_teardown();

    // dependencies
    fatal(xlinux_unload);
    fatal(logger_unload);
    fatal(narrator_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_LIB, LIB_AUNLOAD, "library", "libfabcore");
  }

  finally : coda;
}
