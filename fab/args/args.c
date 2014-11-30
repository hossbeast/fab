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
#include "FAB.errtab.h"

#include "logs.h"
#include "selector.h"
#include "params.h"

#include "unitstring.h"
#include "canon.h"
#include "macros.h"

#include "memblk.h"

struct g_args_t * g_args;

//
// [[ static ]]
//

static path * o_init_fabpath;

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
			if(g_args->selectorsl == g_args->selectorsa)
			{
				int newa = g_args->selectorsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args->selectors, sizeof(g_args->selectors[0]), newa, g_args->selectorsa);
				g_args->selectorsa = newa;
			}

			if((*lists) & SELECTOR_QUERY)
				g_args->selectors_arequery = 1;

			g_args->selectors[g_args->selectorsl++] = (selector){
					.mode = (*mode)
				, .lists = (*lists)
				, .base = (*base)
				, .s = strdup(s)
				, .sl = strlen(s)
			};
		}
	}

	finally : coda;
}

static void usage(int valid, int version, int help, int logcats, int operators, uint64_t opmask)
{
	printf(
"fab : parallel and incremental builds, integrated dependency discovery\n"
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
"usage : fab [ [ selection modifier ] | [ selector ] | [ option ] | [ logexpr ] | [ varexpr ] ] ...\n"
"\n"
"        fab --help      : this message\n"
"        fab --version   : version information\n"
"        fab --logcats   : logging category listing\n"
"        fab --operators : listwise operator listing (including fab-specific operators)\n"
"\n"
"----------- [ selectors ] ----------------------------------------------------------------\n"
"\n"
" (+/-)t              (default) apply selections to fabricate-list\n"
" (+/-)x                        apply selections to fabricate-exact-list\n"
" (+/-)n                        apply selections to fabricate-nofile-list\n"
" (+/-)d                        apply selections to discovery-list\n"
" (+/-)b                        apply selections to invalidate-list\n"
" (+/-)i                        apply selections to inspect-list\n"
" (+/-)q                        apply selections to query-list\n"
" (+/-)C              (default) resolve selectors against current working directory\n"
" (+/-)F                        resolve selectors against init-fabfile-dir\n"
"\n"
"----------- [ options ] ------------------------------------------------------------------\n"
"\n"
" -p                            create buildplan, but do not execute it\n"
" -B                            invalidate all, equivalent to +b [ $! ]\n"
" -j <number>                   concurrency limit (0=unbounded, -1=based on detected CPUs)\n"
" -k <path>                     create bakescript from buildplan instead of executing it\n"
" -K <varname>                  varname is settable at bakescript execution time\n"
#if DEVEL
" --bslic-standard    (default) bakescripts have the standard license\n"
" --bslic-fab                   bakescripts have the fab distribution license\n"
#endif
" --gnid-relative-cwd (default) identify nodes in log messages by path relative to current working directory\n"
" --gnid-relative-fabfile-dir   identify nodes in log messages by path relative to init-fabfile-dir\n"
" --gnid-absolute               identify nodes in log messages by absolute path\n"
" --gnid-canon                  identify nodes in log messages by canonical path\n"
#if DEBUG || DEVEL
" --logtrace-no       (default) do not include file/function/line in log messages\n"
" --logtrace                    include file/function/line in log messages\n"
" --backtrace-pithy   (default) produce a summary of the callstack upon failure\n"
" --backtrace-full              produce a complete description of the callstack upon failure\n"
#endif
" -f <path>                     locate the initial fabfile at <path> rather than ./fabfile\n"
" -I <path>                     append <path> to the list of directories for invocation resolution\n"
" -v <varexpr>                  scope-zero variable definition\n"
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
" +<logcat> to enable logging category\n"  
" -<logcat> to disable logging category\n"  
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
" --o<N>          0 < N < d      only list operators having property <N>\n"
" --o <opname>                   only list operators having the properties of operator <opname>\n"
"\n"
"%d operators matching 0x%"PRIx64"\n"
" 1  2  3  4  5  6  7  8  9  A  B  C  D  name     description\n"
		, i
		, opmask
	);

	for(x = 0; x < g_ops_l; x++)
	{
		if((g_ops[x]->optype & opmask) == opmask)
		{
			printf("[%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c] %6s - %s"
	/* effectual */
				, g_ops[x]->optype & LWOPT_SELECTION_STAGE				? 'x' : ' '
				, g_ops[x]->optype & LWOPT_SELECTION_ACTIVATE			? 'x' : ' '
				, g_ops[x]->optype & LWOPT_SELECTION_RESET				? 'x' : ' '
				, g_ops[x]->optype & LWOPT_WINDOWS_STAGE					? 'x' : ' '
				, g_ops[x]->optype & LWOPT_WINDOWS_ACTIVATE				? 'x' : ' '
				, g_ops[x]->optype & LWOPT_WINDOWS_RESET					? 'x' : ' '
				, g_ops[x]->optype & LWOPT_ARGS_CANHAVE						? 'x' : ' '
				, g_ops[x]->optype & LWOPT_EMPTYSTACK_YES					? 'x' : ' '

	/* informational */
				, g_ops[x]->optype & LWOPT_STACKOP								? 'x' : ' '
				, g_ops[x]->optype & LWOPT_MODIFIERS_CANHAVE			? 'x' : ' '
				, g_ops[x]->optype & LWOPT_OPERATION_PUSHBEFORE		? 'x' : ' '
				, g_ops[x]->optype & LWOPT_OPERATION_INPLACE			? 'x' : ' '
				, g_ops[x]->optype & LWOPT_OPERATION_FILESYSTEM		? 'x' : ' '
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

	printf(
" 1  2  3  4  5  6  7  8  9  A  B  C  D\n"
"\n"
"properties\n"
" 1  SELECTION_STAGE\n"
" 2  SELECTION_ACTIVATE\n"
" 3  SELECTION_RESET\n"
" 4  WINDOWS_STAGE\n"
" 5  WINDOWS_ACTIVATE\n"
" 6  WINDOWS_RESET\n"
" 7  ARGS_CANHAVE\n"
" 8  EMPTYSTACK_YES\n"
" 9  STACKOP\n"
" A  MODIFIERS_CANHAVE\n"
" B  OPERATION_PUSHBEFORE\n"
" C  OPERATION_INPLACE\n"
" D  OPERATION_FILESYSTEM\n"
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

int args_parse(memblk * mb)
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
				, { "version"											, no_argument	, &version, 1 }
				, { "logcats"											, no_argument	, &logcats, 1 }
				, { "logs"												, no_argument	, &logcats, 1 }
				, { "operators"										, no_argument	, &operators, 1 }
				, { "oplist"											, no_argument	, &operators, 1 }
				, { "ops"													, no_argument	, &operators, 1 }
				, { "o"														, required_argument	, 0, 0 }
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
				, { "ob"													, no_argument	, 0, 0 }
				, { "oc"													, no_argument	, 0, 0 }
				, { "od"													, no_argument	, 0, 0 }


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
				, { "bslic-standard"							, no_argument	, &g_args->mode_bslic		, MODE_BSLIC_STD }
				, { "bslic-fab"										, no_argument	, &g_args->mode_bslic		, MODE_BSLIC_FAB }
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
	fatal(path_create_init, &o_init_fabpath, g_params.cwd, "%s", DEFAULT_INIT_FABFILE);
	fatal(path_copy, &g_args->init_fabfile_path, o_init_fabpath);

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
/*
printf("\n");
printf("x=%d %c\n", x, x);
printf("optarg=%s\n", optarg);
printf("optind=%d\n", optind);
printf("optopt=%c\n", optopt);
*/

		if(x == 0)
		{
			if(strcmp(longopts[longindex].name, "o") == 0)
			{
				struct operator * op = 0;
				if((op = op_lookup(optarg, strlen(optarg))))
					opmask |= op->optype;

				operators = 1;
			}
			else if(longopts[longindex].name[0] == 'o' && strlen(longopts[longindex].name) == 2)
			{
				int off = atoi(longopts[longindex].name + 1);
				opmask |= (0x01ULL << (off - 1));
				operators = 1;
			}
		}
		else if(x == 'f')
		{
			path_xfree(&g_args->init_fabfile_path);
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
			g_args->mode_bplan = MODE_BPLAN_BAKE;
			ifree(&g_args->bakescript_path);
			g_args->bakescript_path = strdup(optarg);
		}
		else if(x == 'p')
		{
			g_args->mode_bplan = MODE_BPLAN_GENERATE;
		}
		else if(x == 'v')
		{
			if(g_args->rootvarsl == g_args->rootvarsa)
			{
				int newa = g_args->rootvarsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args->rootvars, sizeof(g_args->rootvars[0]), newa, g_args->rootvarsa);
				g_args->rootvarsa = newa;
			}

			fatal(ixsprintf, &g_args->rootvars[g_args->rootvarsl], "%s%c", optarg, 0);
			g_args->rootvarsl++;
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
			if(g_args->bakevarsl == g_args->bakevarsa)
			{
				int newa = g_args->bakevarsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args->bakevars, sizeof(g_args->bakevars[0]), newa, g_args->bakevarsa);
				g_args->bakevarsa = newa;
			}

			g_args->bakevars[g_args->bakevarsl++] = strdup(optarg);
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

	finally : coda;
}

int args_summarize()
{
	char space[512];
	int x;

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	// log execution parameters under PARAMS
	logf(L_PARAMS	, "%11ssid                    =%u"						, ""	, g_params.sid);
	logf(L_PARAMS	, "%11spid                    =%u"						, ""	, g_params.pid);
	logf(L_PARAMS	, "%11seid                    =%s/%d:%s/%d"		, ""	, g_params.euid_name, g_params.euid, g_params.egid_name, g_params.egid);
	logf(L_PARAMS	, "%11srid                    =%s/%d:%s/%d"		, ""	, g_params.ruid_name, g_params.ruid, g_params.rgid_name, g_params.rgid);
	logf(L_PARAMS	, "%11scwd                    =%s"						, ""	, g_params.cwd);
	logf(L_PARAMS	, "%11sexedir                 =%s"						, ""	, g_params.exedir);
	logf(L_PARAMS	, "%11sprocessors             =%ld"						, ""	, g_params.procs);
	logf(L_PARAMS	, "%11scachedir               =%s"						, ""	, XQUOTE(FABCACHEDIR));
	logf(L_PARAMS	, "%11stmpdir                 =%s"						, ""	,	XQUOTE(FABTMPDIR));
	logf(L_PARAMS	, "%11sipcdir                 =%s"						, ""	,	XQUOTE(FABIPCDIR));
	logf(L_PARAMS	, "%11slwopdir                =%s"						, ""	,	XQUOTE(FABLWOPDIR));
	logf(L_PARAMS	, "%11sinvokedir              =%s"						, ""	,	XQUOTE(FABINVOKEDIR));
	logf(L_PARAMS	, "%11sexpiration-policy      =%s"						, ""	, durationstring(EXPIRATION_POLICY));

	// log cmdline args under ARGS
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-can       =%s", path_cmp(g_args->init_fabfile_path, o_init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->can);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-abs       =%s", path_cmp(g_args->init_fabfile_path, o_init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->abs);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-cwd   =%s", path_cmp(g_args->init_fabfile_path, o_init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->rel_cwd);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-fab   =%s", path_cmp(g_args->init_fabfile_path, o_init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->rel_fab);
	logf(L_ARGS | L_PARAMS				, " %s (%5s) mode-bplan             =%s", g_args->mode_bplan == DEFAULT_MODE_BPLAN ? " " : "*", "k/p", MODE_STR(g_args->mode_bplan));

	if(g_args->mode_bplan == MODE_BPLAN_BAKE)
	{
		logf(L_ARGS | L_PARAMS				, " %s (  %c  ) bakescript-path        =%s", "*", 'k', g_args->bakescript_path);

		if(g_args->bakevarsl == 0)
			logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) bakevar(s)             =", " ", 'K');
		for(x = 0; x < g_args->bakevarsl; x++)
			logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) bakevar(s)             =%s", "*", 'K', g_args->bakevars[x]);
	}

#if DEBUG || DEVEL
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-backtrace         =%s", g_args->mode_backtrace == DEFAULT_MODE_BACKTRACE ? " " : "*", ' ', MODE_STR(g_args->mode_backtrace));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-logtrace          =%s", g_args->mode_logtrace == DEFAULT_MODE_LOGTRACE ? " " : "*", ' ', MODE_STR(g_args->mode_logtrace));
#endif
#if DEVEL
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-bslic             =%s", g_args->mode_bslic == DEFAULT_MODE_BSLIC ? " " : "*", ' ', MODE_STR(g_args->mode_bslic));
#endif
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-gnid              =%s", g_args->mode_gnid == DEFAULT_MODE_GNID ? " " : "*", ' ', MODE_STR(g_args->mode_gnid));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-paths             =%s", g_args->mode_paths == DEFAULT_MODE_PATHS ? " " : "*", ' ', MODE_STR(g_args->mode_paths));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-cycles            =%s", g_args->mode_cycles == DEFAULT_MODE_CYCLES ? " " : "*", ' ', MODE_STR(g_args->mode_cycles));

	if(g_args->concurrency == 0)
		snprintf(space, sizeof(space)  , "%s", "unbounded");
	else
		snprintf(space, sizeof(space)  , "%d", g_args->concurrency);

	logf(L_ARGS | L_PARAMS       , " %s (  %c  ) concurrency            =%s", g_args->concurrency == (int)((float)g_params.procs * 1.2f) ? " " : "*", 'j', space);

	for(x = 0; x < g_args->invokedirsl; x++)
	{
		int star = x && x != (g_args->invokedirsl - 1);
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) invokedirs(s)          =%s", star ? "*" : " ", 'I', g_args->invokedirs[x]);
	}

	if(g_args->rootvarsl == 0)
		logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)         =", " ", ' ');
	for(x = 0; x < g_args->rootvarsl; x++)
		logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)         =%s", "*", 'v', g_args->rootvars[x]);

	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) invalidate-all         =%s", g_args->invalidationsz == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args->invalidationsz ? "yes" : "no");

	if(g_args->selectorsl == 0)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =", " ", ' ');
	for(x = 0; x < g_args->selectorsl; x++)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =%s", "*", ' ', selector_string(&g_args->selectors[x], space, sizeof(space)));

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	finally : coda;
}

void args_freeze(char * const p)
{
	FREEZE(p, g_args->bakescript_path);

	int x;
	for(x = 0; x < g_args->rootvarsl; x++)
		FREEZE(p, g_args->rootvars[x]);
	FREEZE(p, g_args->rootvars);

	for(x = 0; x < g_args->bakevarsl; x++)
		FREEZE(p, g_args->bakevars[x]);
	FREEZE(p, g_args->bakevars);

	for(x = 0; x < g_args->invokedirsl; x++)
		FREEZE(p, g_args->invokedirs[x]);
	FREEZE(p, g_args->invokedirs);

	for(x = 0; x < g_args->selectorsl; x++)
		selector_freeze(p, &g_args->selectors[x]);
	FREEZE(p, g_args->selectors);

	path_freeze(p, g_args->init_fabfile_path);
	FREEZE(p, g_args->init_fabfile_path);
}

void args_thaw(char * const p)
{
	THAW(p, g_args->bakescript_path);

	THAW(p, g_args->rootvars);
	int x;
	for(x = 0; x < g_args->rootvarsl; x++)
		THAW(p, g_args->rootvars[x]);

	THAW(p, g_args->bakevars);
	for(x = 0; x < g_args->bakevarsl; x++)
		THAW(p, g_args->bakevars[x]);

	THAW(p, g_args->invokedirs);
	for(x = 0; x < g_args->invokedirsl; x++)
		THAW(p, g_args->invokedirs[x]);

	THAW(p, g_args->selectors);
	for(x = 0; x < g_args->selectorsl; x++)
		selector_thaw(p, &g_args->selectors[x]);

	THAW(p, g_args->init_fabfile_path);
	path_thaw(p, g_args->init_fabfile_path);
}
