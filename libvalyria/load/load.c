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

#include "load.internal.h"

//
// api
//

static int handles;

xapi API valyria_load()
{
  enter;

  if(handles++ == 0)
  {
    fatal(xlinux_load);
  }

  finally : coda;
}

xapi API valyria_unload()
{
  enter;

  if(--handles == 0)
  {
    fatal(xlinux_unload);
  }

  finally : coda;
}
