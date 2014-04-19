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

#include "args.h"

#include "logs.h"
#include "global.h"
#include "xlinux.h"
#include "unitstring.h"
#include "canon.h"
#include "macros.h"
#include "selector.h"
#include "params.h"

struct g_args_t g_args;

struct optypes
{
	int _1;
	int _2;
	int _3;
	int _4;
	int _5;
	int _6;
	int _7;
	int _8;
	int _9;
	int _a;
	int _b;
	int _c;
	int _d;
};

//
// [[ static ]]
//

static int selector_parse(char * const s, uint32_t * const lists, uint8_t * const base, int * const mode)
{
	if(s)
	{
		if(s[0] == '-' || s[0] == '+')
		{
			if((s[1] >= 'a' && s[1] <= 'z') || (s[1] >= 'A' && s[1] <= 'Z'))
			{
				(*mode) = s[0];
				(*lists) = 0;
				(*base) = SELECTOR_BASE_CWD;
				
				if(strchr(s, 'd'))
					(*lists) |= SELECTOR_DISCOVERY;
				if(strchr(s, 'i'))
					(*lists) |= SELECTOR_INSPECT;
				if(strchr(s, 't'))
					(*lists) |= SELECTOR_FABRICATE;
				if(strchr(s, 'x'))
					(*lists) |= SELECTOR_FABRICATEX;
				if(strchr(s, 'n'))
					(*lists) |= SELECTOR_FABRICATEN;
				if(strchr(s, 'b'))
					(*lists) |= SELECTOR_INVALIDATE;
				if(strchr(s, 'q'))
					(*lists) |= SELECTOR_QUERY;

				if(strchr(s, 'C'))
					(*base) = SELECTOR_BASE_CWD;
				else if(strchr(s, 'F'))
					(*base) = SELECTOR_BASE_FABFILE_DIR;
			}
			else
			{
				failf(FAB_BADARGS, "unknown : %s", s);
			}
		}
		else
		{
			if(g_args.selectorsl == g_args.selectorsa)
			{
				int newa = g_args.selectorsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args.selectors, sizeof(g_args.selectors[0]), newa, g_args.selectorsa);
				g_args.selectorsa = newa;
			}

			if((*lists) & SELECTOR_QUERY)
				g_args.selectors_arequery = 1;

			g_args.selectors[g_args.selectorsl++] = (selector){
					.mode = (*mode)
				, .lists = (*lists)
				, .base = (*base)
				, .s = strdup(s)
			};
		}
	}

	finally : coda;
}

static void usage(int valid, int version, int help, int logopts, int operators, struct optypes * optypes, char * fmt, ...)
{
	printf(
"fab : parallel and incremental builds, integrated dependency discovery\n"
);
if(version)
{
	printf(" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
	"+DEVEL"
#elif BETA
	"+BETA"
#endif
		" @ " XQUOTE(BUILDSTAMP)
		"\n"
	);
}

if(fmt)
{
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
}

if(help)
{
	printf(
"\n"
"usage : fab [ [ selection modifier ] | [ selector ] | [ option ] | [ logexpr ] | [ varexpr ] ] ...\n"
"\n"
"        fab --help      : this message\n"
"        fab --version   : version information\n"
"        fab --logopts   : logging category listing\n"
"        fab --operators : listwise operator listing (including fab-specific operators)\n"
"\n"
"----------- [ options ] ------------------------------------------------------------------\n"
"\n"
" selection modifiers may be clustered\n"
"  +|- t              (default)  apply selections to fabricate-list\n"
"  (+/-)x                        apply selections to fabricate-exact-list\n"
"  (+/-)n                        apply selections to fabricate-nofile-list\n"
"  (+/-)d                        apply selections to discovery-list\n"
"  (+/-)b                        apply selections to invalidate-list\n"
"  (+/-)i                        apply selections to inspect-list\n"
"  (+/-)q                        apply selections to query-list\n"
"  (+/-)C              (default) resolve selectors against current working directory\n"
"  (+/-)F                        resolve selectors against init-fabfile-dir\n"
"\n"
"  -p                            create buildplan, but do not execute it\n"
"  -B                            invalidate all, equivalent to +b [ $! ]\n"
"  -j <number>                   concurrency limit (0=unbounded, -1=based on detected CPUs)\n"
"  -k <path>                     create bakescript from buildplan instead of executing it\n"
"  -K <varname>                  varname is settable at bakescript execution time\n"
#if DEVEL
"  --bslic-standard    (default) bakescripts have the standard license\n"
"  --bslic-fab                   bakescripts have the fab distribution license\n"
#endif
"  --gnid-relative-cwd (default) identify nodes in log messages by path relative to current working directory\n"
"  --gnid-relative-fabfile-dir   identify nodes in log messages by path relative to init-fabfile-dir\n"
"  --gnid-absolute               identify nodes in log messages by absolute path\n"
"  --gnid-canon                  identify nodes in log messages by canonical path\n"
#if DEVEL
"  --logtrace-no       (default) do not include file/function/line in log messages\n"
"  --logtrace                    include file/function/line in log messages\n"
"  --backtrace-pithy   (default) produce a summary of the callstack upon failure\n"
"  --backtrace-full              produce a complete description of the callstack upon failure\n"
#endif
"  -f <path>                     locate the initial fabfile at <path> rather than ./fabfile\n"
"  -I <path>                     append <path> to the list of directories for invocation resolution\n"
"  -v <varexpr>                  scope-zero variable definition\n"
"  --cycles-warn       (default) warn when a cycle is detected (once per unique cycle)\n"
"  --cycles-fail                 fail when a cycle is detected\n"
"  --cycles-deal                 deal with cycles (by terminating the traversal)\n"
	);
}

if(logopts)
{
	printf(
"\n"
"----------- [ logopts ] ------------------------------------------------------------------\n"
"\n"
" +<log name> to enable logging category\n"  
" -<log name> to disable logging category\n"  
"\n"
);

	int x;
	for(x = 0; x < g_logs_l; x++)
		printf("  %-10s %s\n", g_logs[x].s, g_logs[x].d);
}

if(operators)
{
	if(!help)
		printf("\n");

	printf(
"\n"
"----------------- [ operators ] ------------------------------------------------\n"
"\n"
" 1  2  3  4  5  6  7  8  9  A  B  C  D  name     description\n"
	);

	uint64_t mask = 0;

	if(optypes && optypes->_1)
		mask |= 0x1ULL << (0x1 - 1);
	if(optypes && optypes->_2)
		mask |= 0x1ULL << (0x2 - 1);
	if(optypes && optypes->_3)
		mask |= 0x1ULL << (0x3 - 1);
	if(optypes && optypes->_4)
		mask |= 0x1ULL << (0x4 - 1);
	if(optypes && optypes->_5)
		mask |= 0x1ULL << (0x5 - 1);
	if(optypes && optypes->_6)
		mask |= 0x1ULL << (0x6 - 1);
	if(optypes && optypes->_7)
		mask |= 0x1ULL << (0x7 - 1);
	if(optypes && optypes->_8)
		mask |= 0x1ULL << (0x8 - 1);
	if(optypes && optypes->_9)
		mask |= 0x1ULL << (0x9 - 1);
	if(optypes && optypes->_a)
		mask |= 0x1ULL << (0xa - 1);
	if(optypes && optypes->_b)
		mask |= 0x1ULL << (0xb - 1);
	if(optypes && optypes->_c)
		mask |= 0x1ULL << (0xc - 1);
	if(optypes && optypes->_d)
		mask |= 0x1ULL << (0xd - 1);

	int i = 0;
	int x;
	for(x = 0; x < g_ops_l; x++)
	{
		if((g_ops[x]->optype & mask) == mask)
		{
			printf("[%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c] %6s - %s"
	/* effectual */
				, g_ops[x]->optype & LWOP_SELECTION_STAGE 			? 'x' : ' '
				, g_ops[x]->optype & LWOP_SELECTION_ACTIVATE		? 'x' : ' '
				, g_ops[x]->optype & LWOP_SELECTION_RESET				? 'x' : ' '
				, g_ops[x]->optype & LWOP_WINDOWS_STAGE					? 'x' : ' '
				, g_ops[x]->optype & LWOP_WINDOWS_ACTIVATE			? 'x' : ' '
				, g_ops[x]->optype & LWOP_WINDOWS_RESET					? 'x' : ' '
				, g_ops[x]->optype & LWOP_ARGS_CANHAVE					? 'x' : ' '
				, g_ops[x]->optype & LWOP_EMPTYSTACK_YES				? 'x' : ' '

	/* informational */
				, g_ops[x]->optype & LWOP_STACKOP								? 'x' : ' '
				, g_ops[x]->optype & LWOP_MODIFIERS_CANHAVE			? 'x' : ' '
				, g_ops[x]->optype & LWOP_OPERATION_PUSHBEFORE	? 'x' : ' '
				, g_ops[x]->optype & LWOP_OPERATION_INPLACE			? 'x' : ' '
				, g_ops[x]->optype & LWOP_OPERATION_FILESYSTEM	? 'x' : ' '
				, g_ops[x]->s
				, g_ops[x]->desc
			);

			if(g_ops[x]->mnemonic)
			{
				printf(" (%s)", g_ops[x]->mnemonic);
			}
			printf("\n");

			i++;
		}
	}

	printf("%d operators\n", i);

	printf(
		" 1. SELECTION_STAGE\n"
		" 2. SELECTION_ACTIVATE\n"
		" 3. SELECTION_RESET\n"
		" 4. WINDOWS_STAGE\n"
		" 5. WINDOWS_ACTIVATE\n"
		" 6. WINDOWS_RESET\n"
		" 7. ARGS_CANHAVE\n"
		" 8. EMPTYSTACK_YES\n"
		" 9. STACKOP\n"
		" A. MODIFIERS_CANHAVE\n"
		" B. OPERATION_PUSHBEFORE\n"
		" C. OPERATION_INPLACE\n"
		" D. OPERATION_FILESYSTEM\n"
	);
}

if(help || logopts || operators)
{
	printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
	);
}

	exit(!valid);
}

//
// [[ public ]]
//

int args_parse()
{
	char space[512];

	path * fabpath = 0;

	int help = 0;
	int version = 0;
	int logopts = 0;
	int operators = 0;

	struct optypes optypes = {};

	struct option longopts[] = {
/* informational */
				  { "help"												, no_argument	, &help, 1 }
				, { "version"											, no_argument	, &version, 1 }
				, { "logopts"											, no_argument	, &logopts, 1 }
				, { "operators"										, no_argument	, &operators, 1 }
				, { "o1"													, no_argument	, &optypes._1, 1 }
				, { "o2"													, no_argument	, &optypes._2, 1 }
				, { "o3"													, no_argument	, &optypes._3, 1 }
				, { "o4"													, no_argument	, &optypes._4, 1 }
				, { "o5"													, no_argument	, &optypes._5, 1 }
				, { "o6"													, no_argument	, &optypes._6, 1 }
				, { "o7"													, no_argument	, &optypes._7, 1 }
				, { "o8"													, no_argument	, &optypes._8, 1 }
				, { "o9"													, no_argument	, &optypes._9, 1 }
				, { "oa"													, no_argument	, &optypes._a, 1 }
				, { "ob"													, no_argument	, &optypes._b, 1 }
				, { "oc"													, no_argument	, &optypes._c, 1 }
				, { "od"													, no_argument	, &optypes._d, 1 }


/* program longopts */
				, { "cycles-warn"									, no_argument	, &g_args.mode_cycles		, MODE_CYCLES_WARN }
				, { "cycles-fail"									, no_argument	, &g_args.mode_cycles		, MODE_CYCLES_FAIL }
				, { "cycles-deal"									, no_argument	, &g_args.mode_cycles		, MODE_CYCLES_DEAL }
				, { "paths-relative"							, no_argument	, &g_args.mode_paths		, MODE_RELATIVE_FABFILE_DIR }
				, { "paths-absolute"							, no_argument	, &g_args.mode_paths		, MODE_ABSOLUTE }
				, { "gnid-relative-cwd"						, no_argument	, &g_args.mode_gnid			, MODE_RELATIVE_CWD }
				, { "gnid-relative-fabfile-dir"		, no_argument	, &g_args.mode_gnid			, MODE_RELATIVE_FABFILE_DIR }
				, { "gnid-absolute"								, no_argument	, &g_args.mode_gnid			, MODE_ABSOLUTE }
				, { "gnid-canon"									, no_argument	, &g_args.mode_gnid			, MODE_CANONICAL }

#if DEVEL
				, { "backtrace-pithy"							, no_argument	, &g_args.mode_backtrace, MODE_BACKTRACE_PITHY }
				, { "backtrace-full"							, no_argument	, &g_args.mode_backtrace, MODE_BACKTRACE_FULL }
				, { "logtrace-no"									, no_argument	, &g_args.mode_logtrace	, MODE_LOGTRACE_NONE }
				, { "logtrace"										, no_argument	, &g_args.mode_logtrace	, MODE_LOGTRACE_FULL }
				, { "bslic-standard"							, no_argument	, &g_args.mode_bslic		, MODE_BSLIC_STD }
				, { "bslic-fab"										, no_argument	, &g_args.mode_bslic		, MODE_BSLIC_FAB }
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
/* k */	, { 0	, required_argument	, 0			, 'k'	}		// bakescript output path
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
/* K */ , { 0	, required_argument	, 0			, 'K' }		// baked variables
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
	g_args.concurrency			= DEFAULT_CONCURRENCY_LIMIT;
	g_args.mode_bplan				= DEFAULT_MODE_BPLAN;
	g_args.mode_gnid				= DEFAULT_MODE_GNID;
	g_args.mode_cycles			= DEFAULT_MODE_CYCLES;
	g_args.mode_paths				= DEFAULT_MODE_PATHS;
#if DEVEL
	g_args.mode_backtrace		= DEFAULT_MODE_BACKTRACE;
	g_args.mode_logtrace		= DEFAULT_MODE_LOGTRACE;
	g_args.mode_bslic				= DEFAULT_MODE_BSLIC;
#endif
	g_args.invalidationsz	= DEFAULT_INVALIDATE_ALL;
	fatal(path_create_init, &fabpath, g_params.cwd, "%s", DEFAULT_INIT_FABFILE);
	fatal(path_copy, &g_params.init_fabfile_path, fabpath);

	// default invokedirs - head of list
	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(ixstrdup, &g_args.invokedirs[g_args.invokedirsl++], XQUOTE(FABINVOKEDIR));

	// selectors apply to the following list(s)
	uint32_t selector_lists = SELECTOR_FABRICATE;
	uint8_t selector_base = SELECTOR_BASE_CWD;
	int selector_mode = '+';

	// initialize logger
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|BPEXEC|DSCINFO", 0, L_INFO);
	fatal(log_init_and_describe, L_INFO);

	int x;

/*
for(x = 0; x < g_argc; x++)
	printf("%d '%s'\n", x, g_argv[x]);
*/

	opterr = 0;
	while((x = getopt_long(g_argc, g_argv, switches, longopts, 0)) != -1)
	{
/*
printf("\n");
printf("x=%d %c\n", x, x);
printf("optarg=%s\n", optarg);
printf("optind=%d\n", optind);
printf("optopt=%c\n", optopt);
*/

		if(x == 0)
		{
			// recognized longopts
		}
		else if(x == 'f')
		{
			path_xfree(&g_params.init_fabfile_path);
			fatal(path_create_init, &g_params.init_fabfile_path, g_params.cwd, "%s", optarg);
		}
		else if(x == 'j')
		{
			int n = 0;
			if(sscanf(optarg, "%d%n", &g_args.concurrency, &n) != 1 || n < -1)
			{
				failf(FAB_BADARGS, "badly formed option for -j : '%s'", optarg);
			}
		}
		else if(x == 'k')
		{
			g_args.mode_bplan = MODE_BPLAN_BAKE;
			ifree(&g_args.bakescript_path);
			g_args.bakescript_path = strdup(optarg);
		}
		else if(x == 'p')
		{
			g_args.mode_bplan = MODE_BPLAN_GENERATE;
		}
		else if(x == 'v')
		{
			if(g_args.rootvarsl == g_args.rootvarsa)
			{
				int newa = g_args.rootvarsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args.rootvars, sizeof(g_args.rootvars[0]), newa, g_args.rootvarsa);
				g_args.rootvarsa = newa;
			}

			fatal(ixsprintf, &g_args.rootvars[g_args.rootvarsl], "%s%c", optarg, 0);
			g_args.rootvarsl++;
		}
		else if(x == 'B')
		{
			g_args.invalidationsz = 1;
		}
		else if(x == 'I')
		{
			fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
			fatal(ixstrdup, &g_args.invokedirs[g_args.invokedirsl++], optarg);
		}
		else if(x == 'K')
		{
			if(g_args.bakevarsl == g_args.bakevarsa)
			{
				int newa = g_args.bakevarsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args.bakevars, sizeof(g_args.bakevars[0]), newa, g_args.bakevarsa);
				g_args.bakevarsa = newa;
			}

			g_args.bakevars[g_args.bakevarsl++] = strdup(optarg);
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

	// configure logger prefix and trace
#if !DEVEL
	uint64_t trace = 0;
	if(g_args.mode_logtrace == MODE_LOGTRACE_FULL)
		log_config(L_TAG, L_LWOPINFO | L_LWTOKEN | L_LWSTATE | L_FFTOKEN | L_FFSTATE);
	else
		log_config(L_TAG, 0);
#else
	log_config(L_TAG, 0);
#endif

	if(help || version || logopts || operators)
	{
		usage(1, 1, help, logopts, operators, &optypes, 0);
	}

	// default invokedirs - tail of list
	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(ixstrdup, &g_args.invokedirs[g_args.invokedirsl++], g_params.init_fabfile_path->abs_dir);

	// CPU count heuristic
	long procs = -1;
	if(g_args.concurrency == -1)
	{
		if((procs = sysconf(_SC_NPROCESSORS_ONLN)) == -1)
		{
			// unable to determine available CPU count
		}
		else if(procs < 1)
		{
			// shenanigans
		}
		else
		{
			g_args.concurrency = (float)procs * 1.2f;
		}
	}

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	// log execution parameters under PARAMS
	logf(L_PARAMS	, "%11spid                    =%u"						, ""	, g_params.pid);
	logf(L_PARAMS	, "%11ssid                    =%u"						, ""	, g_params.sid);
	logf(L_PARAMS	, "%11seid                    =%s/%d:%s/%d"		, ""	, g_params.euid_name, g_params.euid, g_params.egid_name, g_params.egid);
	logf(L_PARAMS	, "%11srid                    =%s/%d:%s/%d"		, ""	, g_params.ruid_name, g_params.ruid, g_params.rgid_name, g_params.rgid);
	logf(L_PARAMS	, "%11scwd                    =%s"						, ""	, g_params.cwd);
	logf(L_PARAMS	, "%11scachedir               =%s"						, ""	, XQUOTE(FABCACHEDIR));
	logf(L_PARAMS	, "%11stmpdir                 =%s"						, ""	,	XQUOTE(FABTMPDIR));
	logf(L_PARAMS	, "%11slwopdir                =%s"						, ""	,	XQUOTE(FABLWOPDIR));
	logf(L_PARAMS	, "%11sinvokedir              =%s"						, ""	,	XQUOTE(FABINVOKEDIR));
	logf(L_PARAMS	, "%11sexpiration-policy      =%s"						, ""	, durationstring(EXPIRATION_POLICY));

	// log cmdline args under ARGS
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-can       =%s", path_cmp(g_params.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_params.init_fabfile_path->can);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-abs       =%s", path_cmp(g_params.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_params.init_fabfile_path->abs);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-cwd   =%s", path_cmp(g_params.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_params.init_fabfile_path->rel_cwd);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-fab   =%s", path_cmp(g_params.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_params.init_fabfile_path->rel_fab);
	logf(L_ARGS | L_PARAMS				, " %s (%5s) mode-bplan             =%s", g_args.mode_bplan == DEFAULT_MODE_BPLAN ? " " : "*", "k/p", MODE_STR(g_args.mode_bplan));

	if(g_args.mode_bplan == MODE_BPLAN_BAKE)
	{
		logf(L_ARGS | L_PARAMS				, " %s (  %c  ) bakescript-path        =%s", "*", 'k', g_args.bakescript_path);

		if(g_args.bakevarsl == 0)
			logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) bakevar(s)             =", " ", 'K');
		for(x = 0; x < g_args.bakevarsl; x++)
			logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) bakevar(s)             =%s", "*", 'K', g_args.bakevars[x]);
	}

#if DEVEL
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-backtrace         =%s", g_args.mode_backtrace == DEFAULT_MODE_BACKTRACE ? " " : "*", ' ', MODE_STR(g_args.mode_backtrace));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-logtrace          =%s", g_args.mode_logtrace == DEFAULT_MODE_LOGTRACE ? " " : "*", ' ', MODE_STR(g_args.mode_logtrace));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-bslic             =%s", g_args.mode_bslic == DEFAULT_MODE_BSLIC ? " " : "*", ' ', MODE_STR(g_args.mode_bslic));
#endif
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-gnid              =%s", g_args.mode_gnid == DEFAULT_MODE_GNID ? " " : "*", ' ', MODE_STR(g_args.mode_gnid));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-paths             =%s", g_args.mode_paths == DEFAULT_MODE_PATHS ? " " : "*", ' ', MODE_STR(g_args.mode_paths));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-cycles            =%s", g_args.mode_cycles == DEFAULT_MODE_CYCLES ? " " : "*", ' ', MODE_STR(g_args.mode_cycles));

	if(procs > 0)
		snprintf(space, sizeof(space)  , "%d (heuristic)", g_args.concurrency);
	else if(g_args.concurrency == 0)
		snprintf(space, sizeof(space)  , "%s", "unbounded");
	else
		snprintf(space, sizeof(space)  , "%d", g_args.concurrency);

	logf(L_ARGS | L_PARAMS       , " %s (  %c  ) concurrency            =%s", procs > 0 ? " " : "*", 'j', space);

	for(x = 0; x < g_args.invokedirsl; x++)
	{
		int star = x && x != (g_args.invokedirsl - 1);
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) invokedirs(s)          =%s", star ? "*" : " ", 'I', g_args.invokedirs[x]);
	}

	if(g_args.rootvarsl == 0)
		logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)         =", " ", ' ');
	for(x = 0; x < g_args.rootvarsl; x++)
		logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)         =%s", "*", 'v', g_args.rootvars[x]);

	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) invalidate-all         =%s", g_args.invalidationsz == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidationsz ? "yes" : "no");

	if(g_args.selectorsl == 0)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =", " ", ' ');
	for(x = 0; x < g_args.selectorsl; x++)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =%s", "*", ' ', selector_string(&g_args.selectors[x], space, sizeof(space)));

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

exit(0);

finally:
	path_free(fabpath);
coda;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.selectorsl; x++)
		free(g_args.selectors[x].s);
	free(g_args.selectors);

	for(x = 0; x < g_args.rootvarsl; x++)
		free(g_args.rootvars[x]);
	free(g_args.rootvars);

	for(x = 0; x < g_args.bakevarsl; x++)
		free(g_args.bakevars[x]);
	free(g_args.bakevars);

	for(x = 0; x < g_args.invokedirsl; x++)
		free(g_args.invokedirs[x]);
	free(g_args.invokedirs);

	free(g_args.bakescript_path);
}
