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
#include "identity.h"

#include "logs.h"
#include "global.h"
#include "dirutil.h"
#include "macros.h"
#include "args.h"

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

		// pid is myself, or it is unkillable
		if(pid == g_params.pid || kill(pid, 0))
		{
			fatal(rmdir_recursive, fpath, 1, FAB_BADTMP);
		}

	finally :
		XAPI_INFOF("path", "%s", fpath);
	coda;
	};

	fatal(xnftw_nth, XQUOTE(FABTMPDIR) "/pid", fn, 32, 0, 1);

	// ensure directories for my own pid exist
	snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/fml", g_params.pid);
	fatal(mkdirp, space, S_IRWXU | S_IRWXG | S_IRWXO);

	// resume user identity
	fatal(identity_assume_user);

	finally : coda;
}
