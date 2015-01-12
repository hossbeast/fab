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

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xapi.h"
#include "xlinux.h"

#include "params.h"
#include "FAB.errtab.h"

#include "canon.h"

struct g_params_t g_params;

//
// [[ public ]]
//

int params_setup()
{
	char space[512];
	char dst[512];

	//
	// parameters
	//
	g_params.pid = getpid();
	g_params.ppid = getppid();
	g_params.cwd = getcwd(0, 0);
	g_params.cwdl = strlen(g_params.cwd);

	// canonical path to executing fab binary
	ssize_t r = 0;
	fatal(xreadlink, "/proc/self/exe", space, sizeof(space), &r);

	r += snprintf(space + r, sizeof(space) - r, "/..");

	fatal(canon, space, r, g_params.cwd, g_params.cwdl, dst, sizeof(dst), 0, CAN_REALPATH);
	fatal(ixstrdup, &g_params.exedir, dst);

	if((g_params.exedirl = strlen(g_params.exedir)) < 1)
		fail(FAB_NXPARAMS);

	if((g_params.procs = sysconf(_SC_NPROCESSORS_ONLN)) == -1)
	{
		// unable to determine available CPU count
		g_params.procs = 0;
	}
	else if(g_params.procs < 1)
	{
		// shenanigans
		g_params.procs = 0;
	}

	finally : coda;
}

void params_teardown()
{
	free(g_params.cwd);
	free(g_params.exedir);

	free(g_params.ruid_name);
	free(g_params.euid_name);
	free(g_params.rgid_name);
	free(g_params.egid_name);
}
