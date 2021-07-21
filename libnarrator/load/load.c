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

#include "load.h"
#include "fd.internal.h"
#include "nullity.internal.h"

static int handles;

//
// api
//

void API narrator_load()
{
  if(handles++ == 0)
  {
    nullity_setup();
    fd_setup();
  }
}

void API narrator_unload()
{
  if(--handles == 0)
  {
    nullity_cleanup();
    fd_cleanup();
  }
}
