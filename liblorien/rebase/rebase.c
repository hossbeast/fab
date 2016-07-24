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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"
#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

#include "internal.h"
#include "rebase.internal.h"

#include "strutil.h"
#include "macros.h"


//
// api
//

API xapi rebase(
    const char * const path
  , size_t pathl
  , const char * const base
  , size_t basel
  , char * const dst
  , const size_t siz
  , size_t * z
)
{
  enter;

  pathl = pathl ?: strlen(path);
  basel = basel ?: strlen(base);

  size_t local_z;
  if(z == 0)
    z = &local_z;
  (*z) = 0;

  // match up leading segments between path and base
  int x;
  for(x = 0; x < pathl && x < basel; x++)
  {
    int y = 0;
    while((x + y) < pathl && (x + y) < basel && path[x + y] == base[x + y] && path[x + y] != '/')
      y++;

    if((path[x + y] == '/' || ((x + y) == pathl)) && (base[x + y] == '/' || ((x + y) == basel)))
    {
      x += y;
    }
    else
    {
      break;
    }
  }

  // for each segment in base beyond path, append ".."
  int i;
  for(i = x; i < basel; i++)
  {
    int y = 0;
    while((i + y) < basel && base[i + y] != '/')
      y++;

    if(y && estrcmp(base + i, y - i, ".", 1, 0))
    {
      if(i != x)
        (*z) += znloadf(dst + (*z), siz - (*z), "/");
      (*z) += znloadf(dst + (*z), siz - (*z), "..");
    }

    i += y;
  }

  // append the remainder of the path
  if((pathl - x) > 0)
  {
    if(*z)
      (*z) += znloadf(dst + (*z), siz - (*z), "/");

    (*z) += znloadf(dst + (*z), siz - (*z), "%.*s", (int)pathl - x, path + x);
  }

  finally : coda;
}
