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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "xapi.h"
#include "error/XUNIT.errtab.h"
#undef perrtab
#define perrtab perrtab_XUNIT
#include "xlinux.h"

#include "args.h"
#include "logs.h"

#include "macros.h"

struct g_args_t g_args;

static void usage(int valid, int version, int help, int logcats)
{
	printf(
"xunit : unit test runner\n"
);
if(version)
{
	printf(" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
	"+DEVEL"
#elif DEBUG
	"+DEBUG"
#endif
		" @ " XQUOTE(BUILDSTAMP)
		"\n"
	);
}
if(help)
{
	printf(
"\n"
"usage : xunit [ [ option ] [ logexpr ] [ test .so's ] ] ...\n"
"\n"
"        xunit --help|-h   : this message\n"
"        xunit --version   : version information\n"
"        xunit --logcats   : logging category listing\n"
"\n"
"----------------- [ options ] ------------------------------------------------------------\n"
"\n"
" -a                             placeholder\n"
#if DEBUG || DEVEL
"  --logtrace-no       (default) do not include file/function/line in log messages\n"
"  --logtrace                    include file/function/line in log messages\n"
"  --backtrace-pithy   (default) produce a summary of the callstack upon failure\n"
"  --backtrace-full              produce a complete description of the callstack upon failure\n"
#endif
	);
}

if(logcats)
{
	printf(
"\n"
"----------------- [ logexpr  ] -----------------------------------------------------------\n"
"\n"
" +<logcat> to enable logging category\n"  
" -<logcat> to disable logging category\n"  
"\n"
);

	int x;
	for(x = 0; x < g_logs_l; x++)
		printf("  %-10s %s\n", g_logs[x].s, g_logs[x].d);
}

printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
);

	exit(!valid);
}

static xapi addtest(char * test, int * test_objectsa)
{
  enter;

	if(g_args.test_objectsl == *test_objectsa)
	{
		int ns = *test_objectsa ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, &g_args.test_objects, sizeof(*g_args.test_objects), ns, *test_objectsa);
		*test_objectsa = ns;
	}

	fatal(ixstrdup, &g_args.test_objects[g_args.test_objectsl++], test);

	finally : coda;
}

xapi args_parse()
{
  enter;

	int help = 0;
	int version = 0;
	int	logcats = 0;
	int test_objectsa = 0;

	struct option longopts[] = {
		  { "help"												, no_argument				, &help, 1 } 
		, { "version"											, no_argument				, &version, 1 } 
		, { "logcats"											, no_argument				, &logcats, 1 } 

#if DEBUG || DEVEL
		, { "backtrace-pithy"							, no_argument				, &g_args.mode_backtrace, MODE_BACKTRACE_PITHY }
		, { "backtrace-full"							, no_argument				, &g_args.mode_backtrace, MODE_BACKTRACE_FULL }
		, { "logtrace-no"									, no_argument				, &g_args.mode_logtrace	, MODE_LOGTRACE_NONE }
		, { "logtrace"										, no_argument				, &g_args.mode_logtrace	, MODE_LOGTRACE_FULL }
#endif
		, { }
	};

	char * switches =
		// getopt options
		"-"

		// no-argument switches
		""

		// with-argument switches
		""
	;

	//
	// args:defaults
	//
#if DEBUG || DEVEL
	g_args.mode_backtrace		= DEFAULT_MODE_BACKTRACE;
	g_args.mode_logtrace		= DEFAULT_MODE_LOGTRACE;
#endif

	int x;
	int indexptr;
	opterr = 0;
	while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], switches, longopts, &indexptr)) != -1)
	{
		if(x == 0)
		{
			// longopts - placeholder
		}
		else if(x == 'a')
		{
			// placeholder
		}
		else if(x == '?')
		{
			// unrecognized argv element
			if(optopt)
			{
				failf(XUNIT_BADARGS, "unknown switch : -%c", optopt);
			}
			else
			{
				failf(XUNIT_BADARGS, "unknown argument : %s", g_argv[optind-1]);
			}
		}
		else
		{
			// non-option argv elements
			fatal(addtest, optarg, &test_objectsa);
		}
	}

	for(; optind < g_argc; optind++)
	{
		// options following --
		fatal(addtest, g_argv[optind], &test_objectsa);
	}

	if(help || version || logcats)
	{
		usage(1, 1, help, logcats);
	}

	finally : coda;
}

xapi args_summarize()
{
  enter;

	char space[512];
	int x;

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	// log execution parameters under PARAMS
	logf(L_PARAMS	, "%11sprocessors             =%ld"						, ""	, g_args.procs);

#if DEBUG || DEVEL
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-backtrace         =%s", g_args.mode_backtrace == DEFAULT_MODE_BACKTRACE ? " " : "*", ' ', MODE_STR(g_args.mode_backtrace));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-logtrace          =%s", g_args.mode_logtrace == DEFAULT_MODE_LOGTRACE ? " " : "*", ' ', MODE_STR(g_args.mode_logtrace));
#endif

	if(g_args.concurrency == 0)
		snprintf(space, sizeof(space)  , "%s", "unbounded");
	else
		snprintf(space, sizeof(space)  , "%d", g_args.concurrency);

	logf(L_ARGS | L_PARAMS       , " %s (  %c  ) concurrency            =%s", g_args.concurrency == (int)((float)g_args.procs * 1.2f) ? " " : "*", 'j', space);

	if(g_args.test_objectsl == 0)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) test_object(s)         =", " ", ' ');
	for(x = 0; x < g_args.test_objectsl; x++)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) test_object(s)         =%s", "*", ' ', g_args.test_objects[x]);

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	finally : coda;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.test_objectsl; x++)
		free(g_args.test_objects[x]);

	free(g_args.test_objects);
}
