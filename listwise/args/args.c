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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/ops.h"
#include "listwise/generator.h"

#include "xapi.h"
#include "LISTWISE.errtab.h"
#undef perrtab
#define perrtab perrtab_LISTWISE
#include "xlinux.h"

#include "args.h"
#include "logs.h"

#include "macros.h"

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

static void usage(int valid, int version, int help, int logopts, int operators, struct optypes * optypes)
{
	printf(
"listwise : list transformation utility\n"
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
if(help)
{
	printf(
"\n"
"usage : lw [[options] [logopts] [transform-string]]*\n"
" --help|-h   : this message\n"
" --version   : version information\n"
" --logopts   : logging category listing\n"
" --operators : listwise operator listing\n"
"\n"
"----------------- [ options ] ------------------------------------------------------------\n"
"\n"
" -a                             output entire list, not just selected entries\n"
" -k                             output entire stack, not just top list\n"
" -n                             number output rows 0 .. n\n"
" -N                             number output rows with their list index\n"
" -z                             separate output rows by null byte instead of by newline\n"
" -i <item>                      initial list item\n"
" -l <path>                      read initial list items from <path>\n"
"                                <path> of - means read from stdin\n"
" -0                             separate initial list items read from file by null byte instead of by newline\n"
" -f <path>                      read transform-string from <path> instead of argv\n"
"                                <path> of - means read from stdin\n"
" -                              equal to -f -\n"
#if DEVEL
"\n"
"  --logtrace-no       (default) do not include file/function/line in log messages\n"
"  --logtrace                    include file/function/line in log messages\n"
"\n"
"  --backtrace-pithy   (default) produce a summary of the callstack upon failure\n"
"  --backtrace-full              produce a complete description of the callstack upon failure\n"
"\n"
" liblistwise sanity checks\n"
"  --sanity                      enable liblistwise sanity checks (slow)\n"
#endif
	);
}

if(logopts)
{
	printf(
"\n"
"----------------- [ logopts ] ------------------------------------------------------------\n"
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
"----------------- [ listing options] ----------------------------------------------------\n"
"\n"
" --o1       type 1        restrict listing to those operators of matching type\n"
" --o2       type 2\n"
"  ...\n"
" --od       type d\n"
"\n"
"----------------- [ operators ] ---------------------------------------------------------\n"
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

printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
);

	exit(!valid);
}

int parse_args(pstring ** remnant)
{
	int help = 0;
	int version = 0;
	int	logopts = 0;
	int operators = 0;

	struct optypes optypes = {};

	struct option longopts[] = {
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

#if DEVEL
		, { "backtrace-pithy"							, no_argument	, &g_args.mode_backtrace, MODE_BACKTRACE_PITHY }
		, { "backtrace-full"							, no_argument	, &g_args.mode_backtrace, MODE_BACKTRACE_FULL }
		, { "logtrace-no"									, no_argument	, &g_args.mode_logtrace	, MODE_LOGTRACE_NONE }
		, { "logtrace"										, no_argument	, &g_args.mode_logtrace	, MODE_LOGTRACE_FULL }
#endif
		, { }
	};

	char * switches =
		// getopt options
		"-"

		// no-argument switches
		"ahknz0N"

		// with-argument switches
		"f:l:i:"
	;

	//
	// args:defaults
	//
#if DEVEL
	g_args.mode_backtrace		= DEFAULT_MODE_BACKTRACE;
	g_args.mode_logtrace		= DEFAULT_MODE_LOGTRACE;
#endif

	// initialize logger - prepare g_argc/g_argv
#if DEVEL
	fatal(log_init_and_describe, L_LOGGER);
#else
	fatal(log_init, 0);
#endif

	int x;
	int indexptr;
	opterr = 0;
	while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], switches, longopts, &indexptr)) != -1)
	{
		if(x == 0)
		{
			// longopts
		}
		else if(x == 'a')
		{
			g_args.out_list = 1;
		}
		else if(x == 'k')
		{
			g_args.out_stack = 1;
		}
		else if(x == 'n')
		{
			g_args.numbering = 1;
		}
		else if(x == 'z')
		{
			g_args.out_null = 1;
		}
		else if(x == '0')
		{
			g_args.in_null = 1;
		}
		else if(x == 'f')
		{
			fatal(ixstrdup, &g_args.generator_file, optarg);
		}
		else if(x == 'l')
		{
			fatal(ixstrdup, &g_args.init_file, optarg);
		}
		else if(x == 'i')
		{
			if(g_args.init_listl == g_args.init_lista)
			{
				int ns = g_args.init_lista ?: 10;
				ns = ns * 2 + ns / 2;

				fatal(xrealloc, &g_args.init_list, sizeof(*g_args.init_list), ns, g_args.init_lista);
				fatal(xrealloc, &g_args.init_list_lens, sizeof(*g_args.init_list_lens), ns, g_args.init_lista);
				g_args.init_lista = ns;
			}
			fatal(ixstrdup, &g_args.init_list[g_args.init_listl], optarg);
			g_args.init_list_lens[g_args.init_listl] = strlen(optarg);
			g_args.init_listl++;
		}
		else if(x == 'N')
		{
			g_args.numbering = 2;
		}
		else if(x == 'h')
		{
			help = 1;
		}
		else if(x == '?')
		{
			// unrecognized argv element
			if(*remnant)
				fatal(pscats, remnant, " ");
			fatal(pscatf, remnant, "-%c", optopt);
		}
		else if(strcmp(optarg, "-") == 0)
		{
			// the argument "-"
			fatal(ixstrdup, &g_args.generator_file, optarg);
		}
		else
		{
			// non-option argv elements
			if(*remnant)
				fatal(pscats, remnant, " ");
			fatal(pscats, remnant, optarg);
		}
	}

	for(; optind < g_argc; optind++)
	{
		// options following --
		if(*remnant)
			fatal(pscats, remnant, " ");
		fatal(pscats, remnant, g_argv[optind]);
	}

	// configure logger prefix and trace
	uint64_t prefix = L_LWOPINFO;
	
#if DEVEL
	uint64_t trace = 0;
	prefix |= L_LOGGER | L_LWTOKEN | L_LWSTATE;
	if(g_args.mode_logtrace == MODE_LOGTRACE_FULL)
		trace = L_LWOPINFO | L_LWTOKEN | L_LWSTATE;

	log_config(prefix, trace);
#else
	log_config(prefix);
#endif


	if(help || version || logopts || operators)
	{
		usage(1, 1, help, logopts, operators, &optypes);
	}

	finally : coda;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.init_listl; x++)
		free(g_args.init_list[x]);

	free(g_args.init_list);
	free(g_args.init_list_lens);
	free(g_args.generator_file);
	free(g_args.init_file);
}