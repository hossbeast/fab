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
#include "xapi/SYS.errtab.h"

#include "xlinux/load.h"
#include "logger/load.h"

#include "internal.h"
#include "load.internal.h"
#include "logging.internal.h"

//
// api
//

static int handles;

API xapi yyutil_load()
{
  enter;

  if(handles++ == 0)
  {
    // dependencies
    fatal(xlinux_load);
    fatal(logger_load);

    // modules
    fatal(logging_setup);
  }

  finally : coda;
}

API xapi yyutil_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    // dependencies
    fatal(xlinux_unload);
    fatal(logger_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_SYS, SYS_AUNLOAD, "library", "libyyutil");
  }

  finally : coda;
}
