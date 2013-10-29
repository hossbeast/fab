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

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/ops.h"
#include "listwise/generator.h"

#include "params.h"

struct g_params_t g_params;

//
// [[ public ]]
//

int params_setup()
{
	//
	// parameters
	//
	g_params.pid = getpid();
	g_params.ppid = getppid();
	g_params.sid = getsid(0);
	g_params.cwd = getcwd(0, 0);

	return 0;
}

void params_teardown()
{
	free(g_params.cwd);

	free(g_params.ruid_name);
	free(g_params.euid_name);
	free(g_params.rgid_name);
	free(g_params.egid_name);

	path_free(g_params.init_fabfile_path);
}
