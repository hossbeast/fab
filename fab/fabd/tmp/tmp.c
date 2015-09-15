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

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "xlinux.h"

#include "tmp.h"
#include "params.h"

#include "logs.h"
#include "global.h"
#include "dirutil.h"
#include "macros.h"
#include "args.h"

#define restrict __restrict

/*
** time after which fabd will delete unknown entries in the tmp directory
** directories for related processes are (also) deleted on another schedule
*/
#define EXPIRATION_POLICY (60 * 60 * 24) 		// 24 hours

//
// public
//

int tmp_cleanup(
	  const pid_t * const restrict not_expired
	, size_t not_expiredl
	, const pid_t * const restrict consider_expired
	, size_t consider_expiredl
)
{
	char space[512];

	time_t now = time(0);

	//
	// TMPDIR/pid
	//
	fatal(mkdirp, XQUOTE(FABTMPDIR) "/pid", S_IRWXU | S_IRWXG | S_IRWXO); 

	// cleanup the tmp dir
	int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		if(typeflag != FTW_D)
			fails(FAB_BADTMP, "not a directory");

		pid_t pid = 0;
		int n = 0;
		if(sscanf(fpath + ftwbuf->base, "%d%n", &pid, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
		{
			fails(FAB_BADTMP, "not numeric"); // dirname consists of something other than <pid>
		}

		// not presently executing
		if(kill(pid, 0))
		{
			int r = 1;
			int x;

			for(x = 0; x < not_expiredl; x++)
			{
				if(pid == not_expired[x])
					break;
			}

			// pid is not expired
			if(x < not_expiredl)
				r = 0;
			else
			{
				for(x = 0; x < consider_expiredl; x++)
				{
					if(pid == consider_expired[x])
						break;
				}
				
				// not in consider_expired
				if(x == consider_expiredl)
				{
					// check stamp file
					snprintf(space, sizeof(space), "%s/%s", fpath, "stamp");
					
					struct stat stb;
					fatal(uxstat, space, &stb, &r);
					
					// directory does not contain a stamp file, or the stamp file is older than the expiration policy
					if(r)
						r = 1;
					
					// the access time of the stamp file is older than the expiration policy
					else if((now - stb.st_atim.tv_sec) > EXPIRATION_POLICY)
						r = 1;
				}
				
				// the process is not executing, and the directory
				//     is in consider_expired
				//  or does not contain a stamp file
				//  or the stamp file is older than the expiration policy
				if(r)
				{
					fatal(rmdir_recursive, fpath, 1, FAB_BADTMP);
				}
			}
		}

	finally :
		XAPI_INFOF("path", "%s", fpath);
	coda;
	};

	fatal(xnftw_nth, XQUOTE(FABTMPDIR) "/pid", fn, 32, 0, 1);

	finally : coda;
}
