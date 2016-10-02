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
#include "xlinux.h"
#include "xlinux/LIB.errtab.h"

#include "internal.h"
#include "load.internal.h"

//
// api
//

static int handles;

API xapi valyria_load()
{
  enter;

  if(handles++ == 0)
  {
    // dependencies
    fatal(xlinux_load);

    // modules
  }

  finally : coda;
}

API xapi valyria_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    // dependencies
    fatal(xlinux_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_LIB, LIB_AUNLOAD, "library", "libvalyria");
  }

  finally : coda;
}
