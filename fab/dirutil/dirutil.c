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
#include <string.h>

#include "dirutil.h"

#include "logs.h"
#include "global.h"

#include "xlinux.h"

int rmdir_recursive(const char * const dirpath, int rmself, int errcode)
{
	int fn(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		if(typeflag == FTW_F || typeflag == FTW_SL)
		{
			fatal(xunlink, fpath, 0);
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
			fail(errcode);
		}

	finally:
		XAPI_INFOS("path", fpath);
	coda;
	};
	
	fatal(xnftw, dirpath, fn, 32, FTW_DEPTH | FTW_PHYS);

	finally : coda;
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

		fatal(uxmkdir, space, mode);
	}

finally:
	XAPI_INFOS("path", space);
coda;
}
