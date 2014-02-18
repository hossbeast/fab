/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#include "xftw.h"

#undef perrtab
#define perrtab perrtab_XLINUX

int API xnftw(const char *dirpath, int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags)
{
  int callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
  {
		int _xapi_r;
    fatal(fn, fpath, sb, typeflag, ftwbuf);

		// conclude sets _xapi_r to the return value
		finally : conclude;

		if(_xapi_r == 0)
			return FTW_CONTINUE;

		return FTW_STOP;
  };

  // depth-first
  int r;
  if((r = nftw(dirpath, callback, nopenfd, flags | FTW_ACTIONRETVAL)) == FTW_STOP || r != 0)
		fail(XLINUX_FTWERROR);
	
finally:
	XAPI_INFOF("path", "%s", dirpath);
coda;
}

int API xnftw_nth(const char *dirpath, int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags, int level)
{
  int callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
  {
		int _xapi_r;
		if(ftwbuf->level == level)
			fatal(fn, fpath, sb, typeflag, ftwbuf);

		// coda custom sets _xapi_r to the return value
		finally : conclude;

		if(_xapi_r == 0)
		{
			if(ftwbuf->level == level)
				return FTW_SKIP_SUBTREE;	// process only nth-level files
			else
				return FTW_CONTINUE;
		}

		return FTW_STOP;
  };

  // depth-first
  int r;
  if((r = nftw(dirpath, callback, nopenfd, flags | FTW_ACTIONRETVAL)) == FTW_STOP || r != 0)
		fail(XLINUX_FTWERROR);
	
finally:
	XAPI_INFOF("path", "%s", dirpath);
coda;
}
