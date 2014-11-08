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
#include <dlfcn.h>

#include "xapi.h"
#include "xlinux/xdlfcn.h"
#include "xunit.h"
#include "xunit/error.h"

#include "args.h"
#include "logs.h"

int main(int g_argc, char** g_argv)
{
	char space[4096];
	size_t tracesz = 0;

	void *		object = 0;

	int total_pass = 0;
	int total_fail = 0;

	// link in libxunit
	xunit_errcode(1234);

	// initialize logger - prepare g_argc/g_argv
	fatal(log_init);

	// parse cmdline arguments
	fatal(args_parse);

	// configure logger
#if DEVEL
	if(g_args.mode_logtrace == MODE_LOGTRACE_FULL)
	{
		fatal(log_config_and_describe
			, L_TAG												// prefix
			, 0														// trace
			, L_LOGGER										// describe bits
		);
	}
	else
	{
		fatal(log_config_and_describe
			, L_TAG
			, 0
			, L_LOGGER
		);
	}
#else
	fatal(log_config, L_TAG);
#endif

	// default logging categories, with lower precedence than cmdline logexprs
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|XUNIT", 0, 1, L_INFO);

	// summarize
	fatal(args_summarize);

	int x;
	for(x = 0; x < g_args.test_objectsl; x++)
	{
		// open the object
#if XUNIT_DEBUG
printf("open(%s)\n", g_args.test_objects[x]);
#endif
		fatal(xdlopen, g_args.test_objects[x], RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND, &object);

		// execute tests contained
		xunit_unit * xunit;
		fatal(uxdlsym, object, "xunit", (void*)&xunit);
#if XUNIT_DEBUG
printf("xunit %p = { setup : %p, teardown : %p, tests : %p }\n"
	, xunit
	, xunit->setup
	, xunit->teardown
	, xunit->tests
);
#endif
		if(xunit)
		{
			if(xunit->setup)
				fatal(xunit->setup, xunit);

			xunit_test ** test = xunit->tests;

			int pass = 0;
			int fail = 0;

			while(*test)
			{
#if XUNIT_DEBUG
printf("test %p = { unit : %p, name : %p, entry : %p }\n"
	, *test
	, (*test)->unit
	, (*test)->name
	, (*test)->entry
);
#endif

				if((*test)->name)
					logf(L_XUNIT, "%p %p %s[%d] %s", object, (*test)->entry, g_args.test_objects[x], pass + fail, (*test)->name);
				else
					logf(L_XUNIT, "%p %p %s[%d]", object, (*test)->entry, g_args.test_objects[x], pass + fail);

				// convenience
				(*test)->unit = xunit;

				int frame;
				if(invoke(&frame, (*test)->entry, (*test)))
				{
					// propagate non-unit-testing errors
					if(XAPI_ERRTAB != perrtab_XUNIT || XAPI_ERRCODE != XUNIT_FAIL)
						fail(0);

					// for unit-testing errors, log the error
					size_t z = xapi_trace_pithy(space, sizeof(space));
					logf(L_XUNIT | L_RED, " %.*s", (int)z, space);

					// discard the error frame(s)
					xapi_frame_unwindto(frame);
					
					fail++;
				}
				else
				{
					pass++;
				}

				test++;
			}

			if(xunit->teardown)
				fatal(xunit->teardown, xunit);

/*
			uint64_t p = 0;
			uint64_t f = 0;

			if(fail == 0)
				p |= L_GREEN;
			else
				f |= L_RED;		

			logf(L_XUNIT | p, " pass : %d", pass);
			logf(L_XUNIT | f, " fail : %d", fail);
*/

			logf(L_XUNIT | (fail == 0 ? L_GREEN : L_RED), " %-9s : %d / %d or %%%5.2f", "pass", pass, (pass + fail), (double)pass / (double)(pass + fail));

			total_pass += pass;
			total_fail += fail;

			if(xunit->teardown)
				fatal(xunit->teardown, xunit);
		}
		else
		{
			logf(L_XUNIT, " %s", g_args.test_objects[x]);
		}
	}

/*
	uint64_t p = 0;
	uint64_t f = 0;

	if(total_fail == 0)
		p |= L_GREEN;
	else
		f |= L_RED;		

	logf(L_XUNIT | p, "total pass : %d", total_pass);
	logf(L_XUNIT | f, "total fail : %d", total_fail);
*/

	logf(L_XUNIT | (total_fail == 0 ? L_GREEN : L_RED), "%-10s : %d / %d or %%%5.2f", "total pass", total_pass, (total_pass + total_fail), (double)total_pass / (double)(total_pass + total_fail));

finally:
	args_teardown();

	if(XAPI_UNWINDING)
	{
# if DEVEL
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

int R;
conclude(&R);

return R || total_fail;
}
