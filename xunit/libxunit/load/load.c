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

#include "xunit.h"
#include "load.internal.h"
#include "XUNIT.errtab.h"

//
// api
//

static int handles;

xapi API xunit_load()
{
  enter;

  if(handles++ == 0)
  {
    // dependencies
    fatal(xlinux_load);
  }

  finally : coda;
}

xapi API xunit_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    // dependencies
    fatal(xlinux_unload);
  }

  finally : coda;
}
