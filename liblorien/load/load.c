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
#include "logger/load.h"
#include "valyria/load.h"
#include "narrator/load.h"

#include "load.internal.h"

//
// api
//

static int handles;

xapi API lorien_load()
{
  enter;

  if(handles++ == 0)
  {
    // dependencies
    fatal(xlinux_load);
    fatal(logger_load);
    fatal(valyria_load);
    fatal(narrator_load);
  }

  finally : coda;
}

xapi API lorien_unload()
{
  enter;

  if(--handles == 0)
  {
    // dependencies
    fatal(xlinux_unload);
    fatal(logger_unload);
    fatal(valyria_unload);
    fatal(narrator_unload);
  }

  finally : coda;
}
