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

#include "xapi.h"
#include "xlinux.h"

#include "fabcore/dirutil.h"

#include "global.h"
#include "logging.h"
#include "tmp.h"

#include "macros.h"

/*
** time after which fabd will delete unknown entries in the tmp directory
** directories for related processes are (also) deleted on another schedule
*/
#define EXPIRATION_POLICY (60 * 60 * 24) 		// 24 hours

//
// public
//

xapi tmp_cleanup(pid_t * pids, size_t pidsl)
{
  enter;

	char space[512];

	time_t now = time(0);

	//
	// TMPDIR/pid
	//
	fatal(mkdirps, S_IRWXU | S_IRWXG | S_IRWXO, XQUOTE(FABTMPDIR) "/pid");

	// cleanup the tmp dir
	xapi fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
    enter;

		if(typeflag != FTW_D)
			fails(FAB_BADTMP, "error", "not a directory");

		pid_t pid = 0;
		int n = 0;
		if(sscanf(fpath + ftwbuf->base, "%d%n", &pid, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
		{
			fails(FAB_BADTMP, "error", "not numeric"); // dirname consists of something other than <pid>
		}

		// not presently executing
		if(kill(pid, 0) == 0)
		{
			int r = 1;
			int x;
			for(x = 0; x < pidsl; x++)
			{
				if(pid == pids[x])
					break;
			}

			if(x == pidsl)	// not in pids
			{
				// check stamp file
				snprintf(space, sizeof(space), "%s/%s", fpath, "stamp");

				struct stat stb;
				fatal(uxstat, space, &stb, &r);

				// directory does not contain a stamp file, or the stamp file is older than the expiration policy
				if(r == 0 && ((now - stb.st_atim.tv_sec) > EXPIRATION_POLICY))
					r = 1;
			}

			// directory is for del, does not contain a stamp file, or the stamp file is older than the expiration policy
			if(r)
			{
				fatal(rmdir_recursive, fpath, 1);
			}
		}

	finally :
		xapi_infof("path", "%s", fpath);
	coda;
	};

	fatal(xnftw_nth, XQUOTE(FABTMPDIR) "/pid", fn, 32, 0, 1);

	finally : coda;
}
