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

#include "narrator/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"

#include "internal.h"
#include "load.internal.h"
#include "logging.internal.h"

//
// api
//

static int handles;

API xapi moria_load()
{
  enter;

  if(handles++ == 0)
  {
    // dependencies
    fatal(xlinux_load);
    fatal(narrator_load);
    fatal(valyria_load);

    // modules
    fatal(logging_setup);
  }

  finally : coda;
}

API xapi moria_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    // dependencies
    fatal(xlinux_unload);
    fatal(narrator_unload);
    fatal(valyria_unload);
  }

  finally : coda;
}
