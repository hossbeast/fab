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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "xapi.h"
#include "xlinux/xunistd.h"
#include "xlinux/xftw.h"
#include "xlinux/xstat.h"

#include "internal.h"
#include "rmdirp.h"

//
// api
//

API xapi rmdirp(const char * const dirpath, int rmself)
{
  enter;

  xapi fn(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
  {
    enter;

    if(typeflag == FTW_F || typeflag == FTW_SL)
    {
      fatal(xunlinks, fpath);
    }
    else if(typeflag == FTW_DP)
    {
      if(ftwbuf->level > 0 || rmself)
      {
        fatal(xrmdir, fpath);
      }
    }
    else
    {
      // WTF
    }

  finally:
    xapi_infos("path", fpath);
  coda;
  };

  fatal(xnftw, dirpath, fn, 32, FTW_DEPTH | FTW_PHYS);

  finally : coda;
}
