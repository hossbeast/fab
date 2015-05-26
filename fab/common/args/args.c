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
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xlinux.h"

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/operators.h"
#include "listwise/transform.h"

#include "args.h"
#include "args.def.h"
#include "FAB.errtab.h"
#include "selector.def.h"

#include "logs.h"
#include "params.h"
#include "path.h"

#include "unitstring.h"
#include "canon.h"
#include "macros.h"

#define restrict __restrict

#undef perrtab
#define perrtab perrtab_FAB

//
// [[ static ]]
//

static void usage(int valid, int version, int help, int logcats, int operators, uint64_t opmask)
{
	printf(
"fab : build optimally\n"
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
"usage : fab [ [ <modifier> ] [ <selector> ] [ <option> ] [ <logexpr> ] ] ...\n"
"\n"
" --help     this message\n"
" --version  version information\n"
" --logs     logexpr listing\n"
" --ops      listwise operator listing (including fab-specific operators)\n"
"\n"
"----------- [ modifier ] -----------------------------------------------------------------\n"
"\n"
" +t|-t               (default) apply selectors to fabricate-list\n"
" +x|-x                         apply selectors to fabricate-exact-list\n"
" +n|-n                         apply selectors to fabricate-nofile-list\n"
" +d|-d                         apply selectors to discovery-list\n"
" +b|-b                         apply selectors to invalidate-list\n"
" +i|-i                         apply selectors to inspect-list\n"
" +q|-q                         apply selectors to query-list\n"
"\n"
" +C|-C               (default) resolve selectors against current working directory\n"
" +F|-F                         resolve selectors against init-fabfile-dir\n"
"\n"
"----------- [ options ] ------------------------------------------------------------------\n"
"\n"
" -p                            create buildplan only, implies +BPDUMP\n"
" -B                            kill fabd instance, if any. consider +b [ $! ]\n"
" -j <number>                   concurrency limit (0=unbounded, -1=based on detected CPUs)\n"
" -k <path>                     create buildscript at <path>\n"
" -K <varname>                  varname is settable at buildscript execution time\n"
#if DEVEL
" --bslic-standard    (default) buildscripts have the standard license\n"
" --bslic-fab                   buildscripts have the fab distribution license\n"
#endif
" --gnid-relative-cwd (default) identify nodes in log messages by path relative to current working directory\n"
" --gnid-relative-fabfile-dir   identify nodes in log messages by path relative to init-fabfile-dir\n"
" --gnid-absolute               identify nodes in log messages by absolute path\n"
" --gnid-canon                  identify nodes in log messages by canonical path\n"
#if DEBUG || DEVEL
" --logtrace-no       (default) do not include file/function/line in log messages\n"
" --logtrace                    include file/function/line in log messages\n"
" --backtrace-pithy   (default) produce a summary of the callstack on error\n"
" --backtrace-full              produce a complete description of the callstack on error\n"
#endif
" -f <path>                     locate the initial fabfile at <path> rather than ./fabfile\n"
" -I <path>                     append <path> to the list of directories for invocation resolution\n"
" --cycles-warn       (default) warn when a cycle is detected (once per unique cycle)\n"
" --cycles-fail                 fail when a cycle is detected\n"
" --cycles-deal                 deal with cycles (by terminating the traversal)\n"
	);
}

if(logcats)
{
	printf(
"\n"
"----------- [ logexpr ] ------------------------------------------------------------------\n"
"\n"
" +<logcat> to enable logging category (use TAG for all)\n"
" -<logcat> to disable logging category (use TAG for all)\n"
"\n"
);

	int x;
	for(x = 0; x < g_logs_l; x++)
		printf("  %-10s %s\n", g_logs[x].s, g_logs[x].d);
}

if(operators)
{
	int i = 0;
	int x;
	for(x = 0; x < g_ops_l; x++)
	{
		if((g_ops[x]->optype & opmask) == opmask)
			i++;
	}

	printf(
"\n"
"----------------- [ operators ] ---------------------------------------------------------\n"
"\n"
"options\n"
" --o<N>          0 < N < f      only list operators having property <N>\n"
" --o <opname>                   only list operators having the properties of operator <opname>\n"
"\n"
"%d operators matching 0x%"PRIx64"\n"
" 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  name     description\n"
		, i
		, opmask
	);

	for(x = 0; x < g_ops_l; x++)
	{
		if((g_ops[x]->optype & opmask) == opmask)
		{
			printf("[%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c] %6s - %s"
/* effectual */
				, g_ops[x]->optype & (0x01ULL << 0x00)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x01)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x02)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x03)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x04)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x05)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x06)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x07)		? 'x' : ' '

/* informational */
				, g_ops[x]->optype & (0x01ULL << 0x08) 		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x09) 		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x0a)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x0b) 		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x0c)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x0d)		? 'x' : ' '
				, g_ops[x]->optype & (0x01ULL << 0x0e)		? 'x' : ' '
				, g_ops[x]->s
				, g_ops[x]->desc
			);

			if(g_ops[x]->mnemonic)
			{
				printf(" (%s)", g_ops[x]->mnemonic);
			}
			printf("\n");
		}
	}

// from LWOPT_TABLE in liblistwise/operator/operator.h
	printf(
" 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n"
"\n"
"properties\n"
" 1  SELECTION_STAGE\n"
" 2  SELECTION_ACTIVATE\n"
" 3  SELECTION_RESET\n"
" 4  WINDOWS_STAGE\n"
" 5  WINDOWS_ACTIVATE\n"
" 6  WINDOWS_RESET\n"
" 7  AGGREGATE\n"
" 8  ACTIVATION_OVERRIDE\n"
" 9  ARGS_CANHAVE\n"
" A  EMPTYSTACK_YES\n"
" B  STACKOP\n"
" C  MODIFIERS_CANHAVE\n"
" D  OPERATION_PUSHBEFORE\n"
" E  OPERATION_INPLACE\n"
" F  OPERATION_FILESYSTEM\n"
	);
}

printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
);

	exit(!valid);
}

//
// [[ public ]]
//

int args_parse()
{
	int help = 0;
	int version = 0;
	int logcats = 0;
	int operators = 0;
	uint64_t opmask = 0;

	fatal(xmalloc, &g_args, sizeof(*g_args));

	struct option longopts[] = {
/* informational */
				  { "help"												, no_argument	, &help, 1 }
				, { "args"												, no_argument	, &help, 1 }
				, { "params"											, no_argument	, &help, 1 }
				, { "options"											, no_argument	, &help, 1 }
				, { "opts"												, no_argument	, &help, 1 }
				, { "version"											, no_argument	, &version, 1 }
				, { "vrs"													, no_argument	, &version, 1 }
				, { "vers"												, no_argument	, &version, 1 }
				, { "log"													, no_argument	, &logcats, 1 }
				, { "logs"												, no_argument	, &logcats, 1 }
				, { "logcat"											, no_argument	, &logcats, 1 }
				, { "logcats"											, no_argument	, &logcats, 1 }
				, { "logexpr"											, no_argument	, &logcats, 1 }
				, { "logexprs"										, no_argument	, &logcats, 1 }
				, { "ops"													, no_argument	, &operators, 1 }
				, { "oplist"											, no_argument	, &operators, 1 }
				, { "oplists"											, no_argument	, &operators, 1 }
				, { "operator"										, no_argument	, &operators, 1 }
				, { "operators"										, no_argument	, &operators, 1 }
				, { "o"														, required_argument	, 0, 0 }
				, { "op"													, required_argument	, 0, 0 }
				, { "o1"													, no_argument	, 0, 0 }
				, { "o2"													, no_argument	, 0, 0 }
				, { "o3"													, no_argument	, 0, 0 }
				, { "o4"													, no_argument	, 0, 0 }
				, { "o5"													, no_argument	, 0, 0 }
				, { "o6"													, no_argument	, 0, 0 }
				, { "o7"													, no_argument	, 0, 0 }
				, { "o8"													, no_argument	, 0, 0 }
				, { "o9"													, no_argument	, 0, 0 }
				, { "oa"													, no_argument	, 0, 0 }
				, { "oA"													, no_argument	, 0, 0 }
				, { "ob"													, no_argument	, 0, 0 }
				, { "oB"													, no_argument	, 0, 0 }
				, { "oc"													, no_argument	, 0, 0 }
				, { "oC"													, no_argument	, 0, 0 }
				, { "od"													, no_argument	, 0, 0 }
				, { "oD"													, no_argument	, 0, 0 }
				, { "oe"													, no_argument	, 0, 0 }
				, { "oE"													, no_argument	, 0, 0 }
				, { "of"													, no_argument	, 0, 0 }
				, { "oF"													, no_argument	, 0, 0 }

/* program longopts */
				, { "cycles-warn"									, no_argument	, &g_args->mode_cycles		, MODE_CYCLES_WARN }
				, { "cycles-fail"									, no_argument	, &g_args->mode_cycles		, MODE_CYCLES_FAIL }
				, { "cycles-deal"									, no_argument	, &g_args->mode_cycles		, MODE_CYCLES_DEAL }
				, { "paths-relative"							, no_argument	, &g_args->mode_paths			, MODE_RELATIVE_FABFILE_DIR }
				, { "paths-absolute"							, no_argument	, &g_args->mode_paths			, MODE_ABSOLUTE }
				, { "gnid-relative-cwd"						, no_argument	, &g_args->mode_gnid			, MODE_RELATIVE_CWD }
				, { "gnid-relative-fabfile-dir"		, no_argument	, &g_args->mode_gnid			, MODE_RELATIVE_FABFILE_DIR }
				, { "gnid-absolute"								, no_argument	, &g_args->mode_gnid			, MODE_ABSOLUTE }
				, { "gnid-canon"									, no_argument	, &g_args->mode_gnid			, MODE_CANONICAL }

#if DEBUG || DEVEL
				, { "backtrace-pithy"							, no_argument	, &g_args->mode_backtrace	, MODE_BACKTRACE_PITHY }
				, { "backtrace-full"							, no_argument	, &g_args->mode_backtrace	, MODE_BACKTRACE_FULL }
				, { "backtrace"										, no_argument	, &g_args->mode_backtrace	, MODE_BACKTRACE_FULL }
				, { "logtrace-no"									, no_argument	, &g_args->mode_logtrace	, MODE_LOGTRACE_NONE }
				, { "logtrace"										, no_argument	, &g_args->mode_logtrace	, MODE_LOGTRACE_FULL }
#endif
#if DEVEL
				, { "bslic-standard"							, no_argument	, &g_args->mode_bslic			, MODE_BSLIC_STD }
				, { "bslic-fab"										, no_argument	, &g_args->mode_bslic			, MODE_BSLIC_FAB }
#endif

/* program switches */
// a
/* b - selection(s) apply to invalidate-list */
// c
/* d - selection(s) apply to inspect-list */
// e
/* f */ , { 0	, required_argument	, 0			, 'f' }		// init-fabfile-path
// g
/* h */
// i - selection(s) apply to inspect-list */
/* j */ , { 0	, required_argument	, 0			, 'j' }		// concurrency limit
/* k */	, { 0	, required_argument	, 0			, 'k'	}		// buildscript output path
// l 
// m
// n - selection(s) apply to fabricate-nofile-list */
// o
/* p */	, { 0	, no_argument				, 0			, 'p' } 	// implies BPDUMP
/* q - selection(s) apply to query-list */
// r
// s
/* t - selection(s) apply to fabricate-list */
// u
/* v */ , { 0	, required_argument	, 0			, 'v' }		// root-level variable definition
// w
/* x - selection(s) apply to fabricate-exact-list */
// y
// z
// A
/* B */ , { 0	, no_argument				, 0			, 'B' }		// global graph node invalidation
/* C - following selectors resolve against cwd */
// D
// E
/* F - following selectors resolve against init-fabfile-dir */
// G
// H
/* I */	, { 0	, required_argument	, 0			, 'I' }		// directory to search for invocations
// J
/* K */ , { 0	, required_argument	, 0			, 'K' }		// bs-runtime variables
// L
// M
// N
// O
// P
// Q
// R
// S
// T
// U
// V
// W
// X
// Y
// Z
				, { }
	};

	char* switches =
		// getopt options
		"-"

		// no-argument switches
		"cpB"

		// with-argument switches
		"f:j:k:v:I:K:"
	;

	//
	// args:defaults
	//
	g_args->concurrency			= DEFAULT_CONCURRENCY_LIMIT;
	g_args->mode_bplan			= DEFAULT_MODE_BPLAN;
	g_args->mode_gnid				= DEFAULT_MODE_GNID;
	g_args->mode_cycles			= DEFAULT_MODE_CYCLES;
	g_args->mode_paths			= DEFAULT_MODE_PATHS;
#if DEBUG || DEVEL
	g_args->mode_backtrace	= DEFAULT_MODE_BACKTRACE;
	g_args->mode_logtrace		= DEFAULT_MODE_LOGTRACE;
#endif
#if DEVEL
	g_args->mode_bslic			= DEFAULT_MODE_BSLIC;
#endif
	g_args->invalidationsz	= DEFAULT_INVALIDATE_ALL;
	fatal(path_create_init, &g_args->init_fabfile_path, g_params.cwd, "%s", DEFAULT_INIT_FABFILE);

	// default invokedirs - head of list
	fatal(xrealloc, &g_args->invokedirs, sizeof(g_args->invokedirs[0]), g_args->invokedirsl + 1, g_args->invokedirsl);
	fatal(ixstrdup, &g_args->invokedirs[g_args->invokedirsl++], XQUOTE(FABINVOKEDIR));

	// selectors apply to the following list(s)
	uint32_t selector_lists = SELECTOR_FABRICATE;
	uint8_t selector_base = SELECTOR_BASE_CWD;
	int selector_mode = '+';

	// disable getopt error messages
	opterr = 0;

	int x;
	int longindex;
	while((x = getopt_long(g_argc, g_argv, switches, longopts, &longindex)) != -1)
	{
		if(x == 0)
		{
			if(strcmp(longopts[longindex].name, "o") == 0 || strcmp(longopts[longindex].name, "op") == 0)
			{
				struct operator * op = 0;
				if((op = op_lookup(optarg, strlen(optarg))))
					opmask |= op->optype;

				operators = 1;
			}
			else if(longopts[longindex].name[0] == 'o' && strlen(longopts[longindex].name) == 2)
			{
				int off = 0;
				int c = longopts[longindex].name[1];
				if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				{
					if(c >= '0' && c <= '9')
						off = c - '0';
					else if(c >= 'a' && c <= 'f')
						off = 10 + (c - 'a');
					else if(c >= 'A' && c <= 'F')
						off = 10 + (c - 'A');

					opmask |= (0x01ULL << (off - 1));
					operators = 1;
				}
			}
		}
		else if(x == 'f')
		{
			fatal(path_create_init, &g_args->init_fabfile_path, g_params.cwd, "%s", optarg);
		}
		else if(x == 'j')
		{
			int n = 0;
			if(sscanf(optarg, "%d%n", &g_args->concurrency, &n) != 1 || n < -1)
			{
				failf(FAB_BADARGS, "badly formed option for -j : '%s'", optarg);
			}
		}
		else if(x == 'k')
		{
			g_args->mode_bplan = MODE_BPLAN_BUILDSCRIPT;
			ifree(&g_args->buildscript_path);
			fatal(ixstrdup, &g_args->buildscript_path, optarg);
		}
		else if(x == 'p')
		{
			g_args->mode_bplan = MODE_BPLAN_GENERATE;
		}
		else if(x == 'B')
		{
			g_args->invalidationsz = 1;
		}
		else if(x == 'I')
		{
			fatal(xrealloc, &g_args->invokedirs, sizeof(g_args->invokedirs[0]), g_args->invokedirsl + 1, g_args->invokedirsl);
			fatal(ixstrdup, &g_args->invokedirs[g_args->invokedirsl++], optarg);
		}
		else if(x == 'K')
		{
			if(g_args->bs_runtime_varsl == g_args->bs_runtime_varsa)
			{
				int newa = g_args->bs_runtime_varsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args->bs_runtime_vars, sizeof(g_args->bs_runtime_vars[0]), newa, g_args->bs_runtime_varsa);
				g_args->bs_runtime_varsa = newa;
			}

			fatal(ixstrdup, &g_args->bs_runtime_vars[g_args->bs_runtime_varsl++], optarg);
		}
		else if(x == '?')
		{
			if(optopt)
			{
				failf(FAB_BADARGS, "unknown switch : -%c", optopt);
			}
			else
			{
				failf(FAB_BADARGS, "unknown argument : %s", g_argv[optind-1]);
			}
		}
		else
		{
			fatal(selector_parse, optarg, &selector_lists, &selector_base, &selector_mode);
		}
	}

	for(; optind < g_argc; optind++)
	{
		// options following --
		fatal(selector_parse, g_argv[optind], &selector_lists, &selector_base, &selector_mode);
	}

	if(help || version || logcats || operators)
	{
		usage(1, 1, help, logcats, operators, opmask);
	}

	// default invokedirs - tail of list
	fatal(xrealloc, &g_args->invokedirs, sizeof(g_args->invokedirs[0]), g_args->invokedirsl + 1, g_args->invokedirsl);
	fatal(ixstrdup, &g_args->invokedirs[g_args->invokedirsl++], g_args->init_fabfile_path->abs_dir);

	// CPU count heuristic
	if(g_args->concurrency == -1)
	{
		if(g_params.procs)
		{
			g_args->concurrency = (float)g_params.procs * 1.2f;
		}
		else
		{
			g_args->concurrency = 8;
		}
	}

	// save args string, minus path to executable
	char * b = g_argvs;
	while(b[0] == ' ')
		b++;
	while(b[0] != ' ' && b[0])
		b++;
	while(b[0] == ' ')
		b++;

	fatal(ixstrdup, &g_args->argvs, b);

	finally : coda;
}
