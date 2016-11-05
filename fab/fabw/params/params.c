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

#include <unistd.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "params.h"

struct g_params g_params;

//
// public
//

xapi params_setup()
{
  enter;

  char space[512];

  // exedir is the path to directory containing the executing binary
  ssize_t r = 0;
  fatal(xreadlinks, "/proc/self/exe", space, sizeof(space), &r);
  while(space[r] != '/')
    r--;

  fatal(ixstrndup, &g_params.exedir, space, r);

  finally : coda;
}

void params_teardown()
{
  iwfree(&g_params.exedir);
}
