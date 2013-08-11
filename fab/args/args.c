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

#include <listwise.h>
#include <listwise/operator.h>
#include <listwise/ops.h>
#include <listwise/generator.h>
#include <listwise/lstack.h>

#include "args.h"

#include "log.h"
#include "fab_control.h"
#include "xmem.h"
#include "xstring.h"
#include "unitstring.h"
#include "canon.h"
#include "macros.h"
#include "selector.h"

struct g_args_t g_args;

//
// [[ static ]]
//

static void usage(int valid, int version, int help, int logopts, int operators, char * fmt, ...)
{
	printf(
"fab : parallel and incremental builds, integrated dependency discovery\n"
);
if(version)
{
	printf(" fab-"
#if DEVEL
	XQUOTE(FABVERSIONS) "+DEVEL"
#else
	XQUOTE(FABVERSIONS)
#endif
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
"usage : fab [[options] [logopts] [selectors]]*\n"
"  --help|-h   : this message\n"
"  --version   : version information\n"
"  --logopts   : logging category listing\n"
"  --operators : listwise operator listing (including fab-specific)\n"
"\n"
"----------- [ selectors ] ----------------------------------------------------------------\n"
"\n"
" <node selector> is one of: \n"
"  1.  text    : relative path match (see selector modifiers)\n"
"  3. /text    : canonical path match\n"
"  4. @text    : nofile match\n"
"  5. [ list ] : list selection, available vars - $! (all nodes)\n"
"   ex. [ $! ~ fg/p ] - select all PRIMARY nodes\n"
"   ex. [ $! ~ fg/s ] - select all SECONDARY nodes\n"
"\n"
"----------- [ options ] ------------------------------------------------------------------\n"
"\n"
" selection modifiers (may be clustered)\n"
"  (+/-)t              (default) add/remove following selection(s) to/from fabricate-list\n"
"  (+/-)x                        add/remove following selection(s) to/from fabricate-exact-list\n"
"  (+/-)n                        add/remove following selection(s) to/from fabricate-nofile-list\n"
"  (+/-)d                        add/remove following selection(s) to/from discovery-list\n"
"  (+/-)b                        add/remove following selection(s) to/from invalidate-list\n"
"  (+/-)i                        add/remove following selection(s) to/from inspect-list\n"
"  (+/-)q                        add/remove following selection(s) to/from query-list\n"
"\n"
"       C              (default) following selectors resolve against cwd (current working directory)\n"
"       F                        following selectors resolve against init-fabfile-dir\n"
"\n"
" execution modes\n"
"  -p                            create buildplan only - do not execute buildplan\n"
"\n"
" incremental builds\n"
"  -B                            invalidate all              equivalent to +b [ $! ]\n"
"\n"
" parallel builds\n"
"  -j <number>                   concurrency limit\n"
"\n"
" path generation\n"
"  --paths-relative    (default) generate paths relative to init-fabfile-dir\n"
"  --paths-absolute              generate absolute paths\n"
"\n"
" bakescript generation\n"
"  -k </path/to/output>          create bakescript - do not execute buildplan\n"
"  -K <variable name>            bakedvar (settable at runtime)\n"
"\n"
#if DEVEL
"  --bslic-standard    (default) bakescripts have the standard license\n"
"  --bslic-fab                   bakescripts have the fab distribution license\n"
"\n"
#endif
" logging\n"
"  --gnid-relative-cwd (default) nodes are identified by path relative to cwd (current working directory)\n"
"  --gnid-relative-fabfile-dir   nodes are identified by path relative to init-fabfile-dir\n"
"  --gnid-absolute               nodes are identified by absolute path\n"
"  --gnid-canon                  nodes are identified by canonical path\n"
"\n"
#if DEVEL
" error reporting\n"
"  --errors-unwind     (default) unwind error stack for error reporting\n"
"  --errors-immediate            report immediate site of error only\n"
"\n"
#endif
" fabfile processing\n"
"  -f <path/to/fabfile>          path to initial fabfile\n"
"  -I <path/to/directory>        directory for locating invocations\n"
"  -v $var=[list]                scope-zero variable definition\n"
"  -v $var+=[list]               scope-zero variable transform-addition\n"
"  -v $var-=[list]               scope-zero variable transform-subtraction\n"
"  -v $var~=generator            scope-zero variable transform-listwise\n"
"\n"
" handling cycles\n"
"  --cycles-warn       (default) warn when a cycle is detected (once per unique cycle)\n"
"  --cycles-fail                 fail when a cycle is detected\n"
"  --cycles-deal                 deal with cycles (halt traversal)\n"
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
	printf(
"\n"
"----------------- [ operators ] ------------------------------------------------------------\n"
"\n"
" 1  2  3  4  5  6  7  8  9    name  description\n"
	);

	int x;
	for(x = 0; x < g_ops_l; x++)
	{
		printf("[%c][%c][%c][%c][%c][%c][%c][%c][%c] %6s - %s\n"
			, g_ops[x]->optype & LWOP_SELECTION_READ				? 'x' : ' '
			, g_ops[x]->optype & LWOP_SELECTION_WRITE				? 'x' : ' '
			, g_ops[x]->optype & LWOP_SELECTION_RESET				? 'x' : ' '
			, g_ops[x]->optype & LWOP_MODIFIERS_CANHAVE			? 'x' : ' '
			, g_ops[x]->optype & LWOP_ARGS_CANHAVE					? 'x' : ' '
			, g_ops[x]->optype & LWOP_OPERATION_PUSHBEFORE	? 'x' : ' '
			, g_ops[x]->optype & LWOP_OPERATION_INPLACE			? 'x' : ' '
			, g_ops[x]->optype & LWOP_OPERATION_FILESYSTEM	? 'x' : ' '
			, g_ops[x]->optype & LWOP_OBJECT_NO							? 'x' : ' '
			, g_ops[x]->s
			, g_ops[x]->desc
		);
	}

	printf(
		" 1. SELECTION_READ\n"
		" 2. SELECTION_WRITE\n"
		" 3. SELECTION_RESET\n"
		" 4. MODIFIERS_CANHAVE\n"
		" 5. ARGS_CANHAVE\n"
		" 6. OPERATION_PUSHBEFORE\n"
		" 7. OPERATION_INPLACE\n"
		" 8. OPERATION_FILESYSTEM\n"
		" 9. OBJECT_NO\n"
	);
}

if(help || logopts || operators)
{
	printf(
"\n"
"For more information visit http://fabutil.org\n"
	);
}

	printf("\n");
	exit(!valid);
}

//
// [[ public ]]
//

int args_parse(int argc, char** argv)
{
	char space[512];

	path * fabpath = 0;

	int help = 0;
	int version = 0;
	int logopts = 0;
	int operators = 0;

	struct option longopts[] = {
/* informational */
				  { "help"												, no_argument	, &help, 1 }
				, { "version"											, no_argument	, &version, 1 }
				, { "logopts"											, no_argument	, &logopts, 1 }
				, { "operators"										, no_argument	, &operators, 1 }

/* program longopts */
				, { "cycles-warn"									, no_argument	, &g_args.mode_cycles	, MODE_CYCLES_WARN }
				, { "cycles-fail"									, no_argument	, &g_args.mode_cycles	, MODE_CYCLES_FAIL }
				, { "cycles-deal"									, no_argument	, &g_args.mode_cycles	, MODE_CYCLES_DEAL }
				, { "paths-relative"							, no_argument	, &g_args.mode_paths	, MODE_RELATIVE_FABFILE_DIR }
				, { "paths-absolute"							, no_argument	, &g_args.mode_paths	, MODE_ABSOLUTE }
				, { "gnid-relative-cwd"						, no_argument	, &g_args.mode_gnid		, MODE_RELATIVE_CWD }
				, { "gnid-relative-fabfile-dir"		, no_argument	, &g_args.mode_gnid		, MODE_RELATIVE_FABFILE_DIR }
				, { "gnid-absolute"								, no_argument	, &g_args.mode_gnid		, MODE_ABSOLUTE }
				, { "gnid-canon"									, no_argument	, &g_args.mode_gnid		, MODE_CANONICAL }

#if DEVEL
				, { "bslic-standard"							, no_argument	, &g_args.mode_bslic	, MODE_BSLIC_STD }
				, { "bslic-fab"										, no_argument	, &g_args.mode_bslic	, MODE_BSLIC_FAB }

				, { "errors-unwind"								, no_argument	, &g_args.mode_errors	, MODE_ERRORS_UNWIND }
				, { "errors-immediate"						, no_argument	, &g_args.mode_errors	, MODE_ERRORS_IMMEDIATE }
#endif

/* program switches */
// a
/* b - selection(s) apply to invalidate-list */
// c
/* d - selection(s) apply to inspect-list */
// e
/* f */ , { 0	, required_argument	, 0			, 'f' }		// init-fabfile-path
// g
/* h */ , { 0	, no_argument				, 0			, 'h' }		// help
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
		"chpB"

		// with-argument switches
		"f:j:k:v:I:K:"
	;

	//
	// parameters
	//
	g_args.pid						= getpid();
	g_args.sid						= getsid(0);
	g_args.cwd						= getcwd(0, 0);

	//
	// args:defaults
	//
	g_args.concurrency		= DEFAULT_CONCURRENCY_LIMIT;
	g_args.mode_bplan			= DEFAULT_MODE_BPLAN;
	g_args.mode_gnid			= DEFAULT_MODE_GNID;
	g_args.mode_cycles		= DEFAULT_MODE_CYCLES;
	g_args.mode_paths			= DEFAULT_MODE_PATHS;
	g_args.mode_errors		= DEFAULT_MODE_ERRORS;
	g_args.mode_bslic			= DEFAULT_MODE_BSLIC;
	g_args.invalidationsz	= DEFAULT_INVALIDATE_ALL;
	fatal(path_create_init, &fabpath, g_args.cwd, "%s", DEFAULT_INIT_FABFILE);
	fatal(path_copy, &g_args.init_fabfile_path, fabpath);

	// default invokedirs - head of list
	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], XQUOTE(FABINVOKEDIR));

	// selectors apply to the following list(s)
	uint32_t selector_lists = SELECTOR_FABRICATE;
	uint8_t selector_base = SELECTOR_BASE_CWD;
	int selector_mode = '+';

	int x;
	int indexptr = 0;
	opterr = 0;
	while(indexptr = 0, (x = getopt_long(argc, argv, switches, longopts, &indexptr)) != -1)
	{
		if(indexptr)
		{
			// longopts
		}
		else if(x == 1 || x == '?')
		{
			char * s = 0;
			if(x == 1)
				s = optarg;
			else if(x == '?')
				s = argv[optind-1];

			if(s)
			{
				if(s[0] == '-' || s[0] == '+')
				{
					if((s[1] >= 'a' && s[1] <= 'z') || (s[1] >= 'A' && s[1] <= 'Z'))
					{
						selector_mode = s[0];
						selector_lists = 0;
						selector_base = SELECTOR_BASE_CWD;
						
						if(strchr(s, 'd'))
							selector_lists |= SELECTOR_DISCOVERY;
						if(strchr(s, 'i'))
							selector_lists |= SELECTOR_INSPECT;
						if(strchr(s, 't'))
							selector_lists |= SELECTOR_FABRICATE;
						if(strchr(s, 'x'))
							selector_lists |= SELECTOR_FABRICATEX;
						if(strchr(s, 'n'))
							selector_lists |= SELECTOR_FABRICATEN;
						if(strchr(s, 'b'))
							selector_lists |= SELECTOR_INVALIDATE;
						if(strchr(s, 'q'))
							selector_lists |= SELECTOR_QUERY;

						if(strchr(s, 'C'))
							selector_base = SELECTOR_BASE_CWD;
						else if(strchr(s, 'F'))
							selector_base = SELECTOR_BASE_FABFILE_DIR;
					}
					else
					{
						usage(0, 1, 0, 0, 0, "unknown : %s", s);
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

					if(selector_lists & SELECTOR_QUERY)
						g_args.selectors_arequery = 1;

					g_args.selectors[g_args.selectorsl++] = (selector){
						  .mode = selector_mode
						, .lists = selector_lists
						, .base = selector_base
						, .s = strdup(s)
					};
				}
			}
		}
		else if(x == 'f')
		{
			path_xfree(&g_args.init_fabfile_path);
			fatal(path_create_init, &g_args.init_fabfile_path, g_args.cwd, "%s", optarg);
		}
		else if(x == 'h')
		{
			help = 1;
		}
		else if(x == 'j')
		{
			int n = 0;
			if(sscanf(optarg, "%d%n", &g_args.concurrency, &n) != 1 || n <= 0)
			{
				fail("badly formed option for -j : '%s'", optarg);
			}
		}
		else if(x == 'k')
		{
			g_args.mode_bplan = MODE_BPLAN_BAKE;
			xfree(&g_args.bakescript_path);
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

			fatal(xsprintf, &g_args.rootvars[g_args.rootvarsl], "%s%c", optarg, 0);
			g_args.rootvarsl++;
		}
		else if(x == 'B')
		{
			g_args.invalidationsz = 1;
		}
		else if(x == 'I')
		{
			fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
			fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], optarg);
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
		else
		{
			usage(0, 1, 0, 0, 0, "unknown : %c", x);
		}
	}

	if(help || version || logopts || operators)
	{
		usage(1, 1, help, logopts, operators, 0);
	}

	// default invokedirs - tail of list
	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], g_args.init_fabfile_path->abs_dir);

	// initialize logger
	fatal(log_init, "+ERROR|WARN|INFO|BPEXEC|DSCINFO");

	log(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	// log execution parameters under PARAMS
	log(L_PARAMS	, "%11spid                    =%u"						, ""	, g_args.pid);
	log(L_PARAMS	, "%11ssid                    =%u"						, ""	, g_args.sid);
	log(L_PARAMS	, "%11seid                    =%s/%d:%s/%d"		, ""	, g_args.euid_name, g_args.euid, g_args.egid_name, g_args.egid);
	log(L_PARAMS	, "%11srid                    =%s/%d:%s/%d"		, ""	, g_args.ruid_name, g_args.ruid, g_args.rgid_name, g_args.rgid);
	log(L_PARAMS	, "%11scwd                    =%s"						, ""	, g_args.cwd);
	log(L_PARAMS	, "%11scachedir               =%s"						, ""	, XQUOTE(FABCACHEDIR));
	log(L_PARAMS	, "%11stmpdir                 =%s"						, ""	,	XQUOTE(FABTMPDIR));
	log(L_PARAMS	, "%11slwopdir                =%s"						, ""	,	XQUOTE(FABLWOPDIR));
	log(L_PARAMS	, "%11sinvokedir              =%s"						, ""	,	XQUOTE(FABINVOKEDIR));
	log(L_PARAMS	, "%11sexpiration-policy      =%s"						, ""	, durationstring(EXPIRATION_POLICY));

	// log cmdline args under ARGS
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-can       =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->can);
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-abs       =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->abs);
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-cwd   =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->rel_cwd);
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-fab   =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->rel_fab);
	log(L_ARGS | L_PARAMS				, " %s (%5s) mode-bplan             =%s", g_args.mode_bplan == DEFAULT_MODE_BPLAN ? " " : "*", "k/p", MODE_STR(g_args.mode_bplan));

	if(g_args.mode_bplan == MODE_BPLAN_BAKE)
	{
		log(L_ARGS | L_PARAMS				, " %s (  %c  ) bakescript-path        =%s", "*", 'k', g_args.bakescript_path);

		if(g_args.bakevarsl == 0)
			log(L_ARGS | L_PARAMS 		, " %s (  %c  ) bakevar(s)             =", " ", 'K');
		for(x = 0; x < g_args.bakevarsl; x++)
			log(L_ARGS | L_PARAMS 		, " %s (  %c  ) bakevar(s)             =%s", "*", 'K', g_args.bakevars[x]);
	}

#if DEVEL
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-bslic             =%s", g_args.mode_bslic == DEFAULT_MODE_BSLIC ? " " : "*", ' ', MODE_STR(g_args.mode_bslic));
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-errors            =%s", g_args.mode_errors == DEFAULT_MODE_ERRORS ? " " : "*", ' ', MODE_STR(g_args.mode_errors));
#endif
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-gnid              =%s", g_args.mode_gnid == DEFAULT_MODE_GNID ? " " : "*", ' ', MODE_STR(g_args.mode_gnid));
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-paths             =%s", g_args.mode_paths == DEFAULT_MODE_PATHS ? " " : "*", ' ', MODE_STR(g_args.mode_paths));
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-cycles            =%s", g_args.mode_cycles == DEFAULT_MODE_CYCLES ? " " : "*", ' ', MODE_STR(g_args.mode_cycles));
	if(g_args.concurrency > 0)
		snprintf(space, sizeof(space)	, "%d", g_args.concurrency);
	else
		snprintf(space, sizeof(space)	, "%s", "unbounded");
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) concurrency            =%s", g_args.concurrency == DEFAULT_CONCURRENCY_LIMIT ? " " : "*", 'j', space);

	for(x = 0; x < g_args.invokedirsl; x++)
	{
		int star = x && x != (g_args.invokedirsl - 1);
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) invokedirs(s)          =%s", star ? "*" : " ", 'I', g_args.invokedirs[x]);
	}

	if(g_args.rootvarsl == 0)
		log(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)         =", " ", ' ');
	for(x = 0; x < g_args.rootvarsl; x++)
		log(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)         =%s", "*", 'v', g_args.rootvars[x]);

	log(L_ARGS | L_PARAMS				, " %s (  %c  ) invalidate-all         =%s", g_args.invalidationsz == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidationsz ? "yes" : "no");

	if(g_args.selectorsl == 0)
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =", " ", ' ');
	for(x = 0; x < g_args.selectorsl; x++)
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =%s", "*", ' ', selector_string(&g_args.selectors[x], space, sizeof(space)));

	log(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

finally:
	path_free(fabpath);
coda;
}

void args_teardown()
{
	free(g_args.ruid_name);
	free(g_args.euid_name);
	free(g_args.rgid_name);
	free(g_args.egid_name);
	free(g_args.cwd);

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

	path_free(g_args.init_fabfile_path);
	free(g_args.bakescript_path);
}
