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

#include <ftw.h>
#include <errno.h>

#include "internal.h"
#include "xftw/xftw.h"
#include "errtab/XLINUX.errtab.h"

//
// static
//

struct {
  xapi (* xfn)(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf, void * arg);
  void * arg;
} xnftw_context;

static int xnftw_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
  enter;

  fatal(xnftw_context.xfn, fpath, sb, typeflag, ftwbuf, xnftw_context.arg);

  xapi R = 0;
  finally : conclude(&R);

  if(R == 0)
    return FTW_CONTINUE;

  return FTW_STOP;
};

struct {
  xapi (* xfn)(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf, void * arg);
  void * arg;
  int level;
} xnftw_nth_context;

static int xnftw_nth_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
  enter_nochecks;

  if(ftwbuf->level == xnftw_nth_context.level)
    fatal(xnftw_nth_context.xfn, fpath, sb, typeflag, ftwbuf, xnftw_nth_context.arg);

  xapi R;
  finally : conclude(&R);

  if(R == 0)
  {
    if(ftwbuf->level == xnftw_nth_context.level)
      return FTW_SKIP_SUBTREE;  // process only nth-level files
    else
      return FTW_CONTINUE;
  }

  return FTW_STOP;
};

//
// api
//

API xapi xnftw(
    const char * restrict dirpath
  , xapi (* xfn)(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf, void * arg)
  , int nopenfd
  , int flags
  , void * arg
)
{
  enter;

  xnftw_context.xfn = xfn;
  xnftw_context.arg = arg;

  // depth-first
  if(nftw(dirpath, xnftw_callback, nopenfd, flags | FTW_ACTIONRETVAL) != 0)
  {
    fail(0);  // propagate error raised by callback
  }

finally:
  xapi_infof("path", "%s", dirpath);
coda;
}

API xapi xnftw_nth(
    const char * restrict dirpath
  , xapi (* xfn)(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf, void * arg)
  , int nopenfd
  , int flags
  , int level
  , void * arg
)
{
  enter;

  xnftw_nth_context.xfn = xfn;
  xnftw_nth_context.arg = arg;
  xnftw_nth_context.level = level;

  // depth-first
  if(nftw(dirpath, xnftw_nth_callback, nopenfd, flags | FTW_ACTIONRETVAL) != 0)
  {
    fail(0);  // user callback raised an error
  }

finally:
  xapi_infof("path", "%s", dirpath);
coda;
}
