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

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dirutil.h"

#include "log.h"
#include "fab_control.h"

int xnftw(const char *dirpath, int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags)
{
	// depth-first
	int r;
	if((r = nftw(dirpath, fn, nopenfd, flags | FTW_ACTIONRETVAL)) == FTW_STOP)
	{
		qfail();	// fn should have called error()
	}
	else if(r != 0)
	{
		fail("nftw failed with: [%d]", r);
	}

	finally : coda;
}

int rmdir_recursive(const char * const dirpath, int rmself)
{
	int fn(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		if(typeflag == FTW_F || typeflag == FTW_SL)
		{
			if(unlink(fpath) != 0)
			{
				error("unlink(%s)=[%d][%s]", fpath, errno, strerror(errno));
				return FTW_STOP;
			}
			return FTW_CONTINUE;
		}
		else if(typeflag == FTW_DP)
		{
			if(ftwbuf->level > 0 || rmself)
			{
				if(rmdir(fpath) != 0)
				{
					error("rmdir(%s)=[%d][%s]", fpath, errno, strerror(errno));
					return FTW_STOP;
				}
			}
			return FTW_CONTINUE;
		}

		error("unexpected %s", fpath);
		return FTW_STOP;
	};
	
	return xnftw(dirpath, fn, 32, FTW_DEPTH | FTW_PHYS);
}

int mkdirp(const char * const path, mode_t mode)
{
	char space[256];

	const char * t = path;
	const char * e = path + strlen(path);

	while(t != e)
	{
		t++;
		while(*t != '/' && t != e)
			t++;

		memcpy(space, path, t - path);
		space[t - path] = 0;

		if(mkdir(space, mode) == -1 && errno != EEXIST)
			fail("mkdir(%s)=%s", space, strerror(errno));
	}

	finally : coda;
}
