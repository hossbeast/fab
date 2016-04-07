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

#include <errno.h>

#include "internal.h"
#include "xftw/xftw.h"
#include "errtab/XLINUX.errtab.h"

#undef perrtab
#define perrtab perrtab_XLINUX

API xapi xnftw(const char *dirpath, int (*xfn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags)
{
  enter;

  int callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
  {
    enter;

    fatal(xfn, fpath, sb, typeflag, ftwbuf);

    int R = 0;
    finally : conclude(&R);

    if(R == 0)
      return FTW_CONTINUE;

    return FTW_STOP;
  };

  // depth-first
  if(nftw(dirpath, callback, nopenfd, flags | FTW_ACTIONRETVAL) != 0)
  {
    fail(0);  // propagate error raised by callback
  }
  /*
  else
  {
    fail(XLINUX_FTWERROR);  // internal-to-ftw error
  }
  */
  
finally:
  xapi_infof("path", "%s", dirpath);
coda;
}

API xapi xnftw_nth(const char *dirpath, int (*xfn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags, int level)
{
  enter;

  int callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
  {
    enter;

    if(ftwbuf->level == level)
      fatal(xfn, fpath, sb, typeflag, ftwbuf);

    int R;
    finally : conclude(&R);

    if(R == 0)
    {
      if(ftwbuf->level == level)
        return FTW_SKIP_SUBTREE;  // process only nth-level files
      else
        return FTW_CONTINUE;
    }

    return FTW_STOP;
  };

  // depth-first
  if(nftw(dirpath, callback, nopenfd, flags | FTW_ACTIONRETVAL) != 0)
  {
    fail(0);  // user callback raised an error
  }
  /*
  else
  {
    fail(XLINUX_FTWERROR);  // internal-to-ftw error
  }
  */
  
finally:
  xapi_infof("path", "%s", dirpath);
coda;
}
