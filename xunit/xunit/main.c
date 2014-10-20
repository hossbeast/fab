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

#include <dlfcn.h>

#include "xapi.h"
#include "xlinux/xdlfcn.h"
#include "xunit.h"

#include "args.h"

#include "logger.h"

int main(int g_argc, char** g_argv)
{
	char space[4096];
	size_t tracesz = 0;

	void *		object = 0;

	// initialize logger - prepare g_argc/g_argv
	fatal(log_init);

	// parse cmdline arguments
	fatal(args_parse);

	// configure logger
#if DEVEL
	if(g_args.mode_logtrace == MODE_LOGTRACE_FULL)
	{
		fatal(log_config_and_describe
			, L_LOGGER | L_LWTOKEN | L_LWSTATE | L_LWOPINFO		// prefix
			, L_LWOPINFO | L_LWTOKEN | L_LWSTATE							// trace
			, L_LOGGER																				// describe bits
		);
	}
	else
	{
		fatal(log_config_and_describe
			, L_LOGGER | L_LWTOKEN | L_LWSTATE | L_LWOPINFO
			, 0
			, L_LOGGER
		);
	}
#else
	fatal(log_config, 0);//L_LWOPINFO);		// prefix
#endif

	int total_success = 0;
	int total_failure = 0;

	int x;
	for(x = 0; x < g_args.test_objectsl; x++)
	{
		int success = 0;
		int failure = 0;

		// open the object
		fatal(xdlopen, g_args.test_objects[x], RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND, &object);

		// execute tests contained
		unittest test;
		fatal(xdlsym, object, "test", (void*)&test);

		total_success += success;
		total_failure += failure;
	}

finally:
	args_teardown();

	if(XAPI_UNWINDING)
	{
#if DEVEL
		if(g_args.mode_backtrace == MODE_BACKTRACE_PITHY)
		{
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEVEL
		}
		else
		{
			tracesz = xapi_trace_full(space, sizeof(space));
		}
#endif

		logw(L_RED, space, tracesz);
	}

	log_teardown();
coda;
}
