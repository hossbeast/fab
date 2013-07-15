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
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tmp.h"
#include "args.h"
#include "identity.h"

#include "log.h"
#include "control.h"
#include "dirutil.h"
#include "macros.h"

/// minmodify
//
// get the minimum modify time of all files and directories in the specified directory
//
// *minmod should have been initialized to the modify time of the directory itself
//
// symbolic links are not followed
//
static int minmodify(const char* dirpath, time_t * minmod)
{
	int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		(*minmod) = MIN((*minmod), sb->st_mtime);
		return FTW_CONTINUE;
	};

	return nftw(dirpath, fn, 32, FTW_PHYS) == 0;
}

//
// public
//

int tmp_setup()
{
	char space[512];

	// temporarily assume fabsys identity
	fatal(identity_assume_fabsys);

	//
	// TMPDIR/pid
	//
	fatal(mkdirp, XQUOTE(FABTMPDIR) "/pid", S_IRWXU | S_IRWXG | S_IRWXO); 
	{
		int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
		{
			if(typeflag != FTW_D)
			{
				// something other than a directory
				log(L_ERROR, "unexpected file %s", fpath);
				return FTW_STOP;
			}
			else if(ftwbuf->level == 0)
			{
				return FTW_CONTINUE;
			}

			pid_t pid = 0;
			int n = 0;
			if(sscanf(fpath + ftwbuf->base, "%d%n", &pid, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
			{
				// dirname consists of something other than <pid>
				log(L_ERROR, "unexpected file %s", fpath);
				return FTW_STOP;
			}

			// pid is myself, or it is unkillable
			if(pid == g_args.pid || kill(pid, 0))
			{
				if(rmdir_recursive(fpath, 1) == 0)
					return FTW_STOP;
			}

			// process only toplevel files
			return FTW_SKIP_SUBTREE;
		};

		fatal_os(nftw, XQUOTE(FABTMPDIR) "/pid", fn, 32, FTW_ACTIONRETVAL);
	}

	// ensure directories for my own pid exist
	snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/fml", g_args.pid);
	fatal(mkdirp, space, S_IRWXU | S_IRWXG | S_IRWXO);

	//
	// CACHEDIR/INIT
	//
	fatal(mkdirp, XQUOTE(FABCACHEDIR) "/INIT", S_IRWXU | S_IRWXG | S_IRWXO); 
	{
		int lvl;
		int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
		{
			if(ftwbuf->level == lvl)
			{
				if(typeflag != FTW_D)
				{
					// something other than a directory
					log(L_ERROR, "unexpected file %s", fpath);
					return FTW_STOP;
				}

				// get the min modify time of everything in the directory
				time_t minmod = sb->st_mtime;
				if(minmodify(fpath, &minmod) == 0)
					return FTW_STOP;

				// minimum modify time older than expiration
				if((time(0) - minmod) > EXPIRATION_POLICY)
				{
					if(rmdir_recursive(fpath, 1) == 0)
						return FTW_STOP;
				}

				return FTW_SKIP_SUBTREE;
			}

			return FTW_CONTINUE;
		};

		lvl = 3; fatal_os(nftw, XQUOTE(FABCACHEDIR) "/INIT", fn, 32, FTW_ACTIONRETVAL);
		lvl = 1; fatal_os(nftw, XQUOTE(FABCACHEDIR) "/INIT", fn, 32, FTW_ACTIONRETVAL);
	}

	// resume user identity
	fatal(identity_assume_user);

	finally : coda;
}
