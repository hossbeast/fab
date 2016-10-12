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
#include "xlinux/load.h"
#include "narrator/load.h"
#include "valyria/load.h"
#include "yyutil/load.h"

#include "xapi/SYS.errtab.h"
#include "xapi/errtab.h"

#include "internal.h"
#include "load.internal.h"
#include "VALUE.errtab.h"

static int handles;

API xapi value_load()
{
  enter;

  if(handles == 0)
  {
    // dependencies
    fatal(xlinux_load);
    fatal(narrator_load);
    fatal(valyria_load);
    fatal(yyutil_load);

    // modules
#if XAPI_STACKTRACE
    fatal(xapi_errtab_register, perrtab_VALUE);
#endif
  }
  handles++;

  finally : coda;
}

API xapi value_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    // dependencies
    fatal(xlinux_unload);
    fatal(valyria_unload);
    fatal(narrator_unload);
    fatal(yyutil_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_SYS, SYS_AUNLOAD, "library", "libvalue");
  }

  finally : coda;
}
