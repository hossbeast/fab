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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/operators.h"
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

static void usage(int valid, int version, int help, int logcats, int operators, uint64_t opmask)
{
	printf(
"listwise : list transformation utility\n"
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
"usage : lw [ [ option ] [ logexpr ] [ transform-string ] ] ...\n"
"\n"
"        lw --help|-h   : this message\n"
"        lw --version   : version information\n"
"        lw --logcats   : logging category listing\n"
"        lw --operators : listwise operator listing\n"
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
" -t <path>                      read transform-string from <path>\n"
" -f <path>                      read transform-string and initial list items from <path>\n"
"                                 <path> of - means stdin\n"
" -                              equal to -f -\n"
" -A                   (default) process subsequent -l, -t, and -f options line-wise\n"
" -0                             process subsequent -l, -t, and -f options nullbyte-wise\n"
#if DEVEL
" --logtrace-no        (default) do not include file/function/line in log messages\n"
" --logtrace                     include file/function/line in log messages\n"
" --backtrace-pithy    (default) produce a summary of the callstack upon failure\n"
" --backtrace-full               produce a complete description of the callstack upon failure\n"
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
				, g_ops[x]->optype & LWOPT_SELECTION_STAGE 				? 'x' : ' '
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

int args_parse(pstring ** remnant)
{
	int help = 0;
	int version = 0;
	int	logcats = 0;
	int operators = 0;
	uint64_t opmask = 0;

	struct option longopts[] = {
		  { "help"												, no_argument				, &help, 1 } 
		, { "version"											, no_argument				, &version, 1 } 
		, { "logcats"											, no_argument				, &logcats, 1 } 
		, { "logs"												, no_argument				, &logcats, 1 } 
		, { "operators"										, no_argument				, &operators, 1 } 
		, { "oplist"											, no_argument				, &operators, 1 } 
		, { "ops"													, no_argument				, &operators, 1 } 
		, { "o"														, required_argument	, 0, 0 }
		, { "o1"													, no_argument				, 0, 0 }
		, { "o2"													, no_argument				, 0, 0 }
		, { "o3"													, no_argument				, 0, 0 }
		, { "o4"													, no_argument				, 0, 0 }
		, { "o5"													, no_argument				, 0, 0 }
		, { "o6"													, no_argument				, 0, 0 }
		, { "o7"													, no_argument				, 0, 0 }
		, { "o8"													, no_argument				, 0, 0 }
		, { "o9"													, no_argument				, 0, 0 }
		, { "oa"													, no_argument				, 0, 0 }
		, { "ob"													, no_argument				, 0, 0 }
		, { "oc"													, no_argument				, 0, 0 }
		, { "od"													, no_argument				, 0, 0 }

#if DEVEL
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
		"aknzA0N"

		// with-argument switches
		"t:f:l:i:"
	;

	//
	// args:defaults
	//
#if DEVEL
	g_args.mode_backtrace		= DEFAULT_MODE_BACKTRACE;
	g_args.mode_logtrace		= DEFAULT_MODE_LOGTRACE;
#endif

	// process input files linewise
	int linewise = 1;

	// allocations
/*
	size_t linewise_init_list_filesa = 0;
	size_t nullwise_init_list_filesa = 0;
	size_t linewise_transform_filesa = 0;
	size_t nullwise_transform_filesa = 0;
	size_t linewise_input_filesa = 0;
	size_t nullwise_input_filesa = 0;
	size_t init_lista = 0;
*/
	size_t inputsa = 0;

	int x;
	int indexptr;
	opterr = 0;
	while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], switches, longopts, &indexptr)) != -1)
	{
		if(x == 0)
		{
			if(strcmp(longopts[indexptr].name, "o") == 0)
			{
				struct operator * op = 0;
				if((op = op_lookup(optarg, strlen(optarg))))
					opmask |= op->optype;

				operators = 1;
			}
			else if(longopts[indexptr].name[0] == 'o' && strlen(longopts[indexptr].name) == 2)
			{
				int off = atoi(longopts[indexptr].name + 1);
				opmask |= (0x01ULL << (off - 1));
				operators = 1;
			}
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
			linewise = 0;
		}
		else if(x == 'A')
		{
			linewise = 1;
		}
/*
		else if(x == 't')
		{
			if(linewise)
				fatal(listadd, &g_args.linewise_input_files, &g_args.linewise_input_filesl, &linewise_input_filesa, optarg);
			else
				fatal(listadd, &g_args.nullwise_input_files, &g_args.nullwise_input_filesl, &nullwise_input_filesa, optarg);
		}
		else if(x == 'f')
		{
			if(linewise)
				fatal(listadd, &g_args.linewise_transform_files, &g_args.linewise_transform_filesl, &linewise_transform_filesa, optarg);
			else
				fatal(listadd, &g_args.nullwise_transform_files, &g_args.nullwise_transform_filesl, &nullwise_transform_filesa, optarg);
		}
		else if(x == 'l')
		{
			if(linewise)
				fatal(listadd, &g_args.linewise_init_list_files, &g_args.linewise_init_list_filesl, &linewise_init_list_filesa, optarg);
			else
				fatal(listadd, &g_args.nullwise_init_list_files, &g_args.nullwise_init_list_filesl, &nullwise_init_list_filesa, optarg);
		}
		else if(x == 'i')
		{
			if(g_args.init_listl == init_lista)
			{
				int ns = init_lista ?: 10;
				ns = ns * 2 + ns / 2;

				fatal(xrealloc, &g_args.init_list, sizeof(*g_args.init_list), ns, init_lista);
				fatal(xrealloc, &g_args.init_list_lens, sizeof(*g_args.init_list_lens), ns, init_lista);
				init_lista = ns;
			}
			fatal(ixstrdup, &g_args.init_list[g_args.init_listl], optarg);
			g_args.init_list_lens[g_args.init_listl] = strlen(optarg);
			g_args.init_listl++;
		}
*/
		else if(x == 't' || x == 'f' || x == 'l' || x == 'i')
		{
			if(g_args.inputsl == inputsa)
			{
				int ns = inputsa ?: 10;
				ns = ns * 2 + ns / 2;

				fatal(xrealloc, &g_args.inputs, sizeof(*g_args.inputs), ns, inputsa);
				inputsa = ns;
			}
			g_args.inputs[g_args.inputsl].linewise = linewise;
			if(x == 'i')
				g_args.inputs[g_args.inputsl].kind = KIND_INIT_LIST_ITEM;
			else if(x == 'l')
				g_args.inputs[g_args.inputsl].kind = KIND_INIT_LIST_FILE;
			else if(x == 't')
				g_args.inputs[g_args.inputsl].kind = KIND_TRANSFORM_FILE;
			else if(x == 'f')
				g_args.inputs[g_args.inputsl].kind = KIND_INPUT_FILE;

			fatal(ixstrdup, &g_args.inputs[g_args.inputsl].s, optarg);
			g_args.inputsl++;
		}
		else if(x == 'N')
		{
			g_args.numbering = 2;
		}
		else if(x == '?')
		{
			// unrecognized argv element
			if(optopt)
			{
				failf(LISTWISE_BADARGS, "unknown switch : -%c", optopt);
			}
			else
			{
				failf(LISTWISE_BADARGS, "unknown argument : %s", g_argv[optind-1]);
			}
		}
		else if(strcmp(optarg, "-") == 0)
		{
			// the argument "-"
//			fatal(ixstrdup, &g_args.generator_file, optarg);
		}
		else
		{
			// non-option argv elements
			if(*remnant)
				fatal(pscats, remnant, " ");
			fatal(pscats, remnant, optarg);
		}
	}

	// read transform-string from stdin unless it has been mentioned for some other purpose
	g_args.stdin_init_list_items = 1;
	for(x = 0; x < g_args.inputsl; x++)
	{
		if(g_args.inputs[x].kind != KIND_INIT_LIST_ITEM && strcmp(g_args.inputs[x].s, "-") == 0)
		{
			g_args.stdin_init_list_items = 0;
			break;
		}
	}

	// process stdin using the prevailing method at the end of argument processing
	g_args.stdin_linewise = linewise;

	for(; optind < g_argc; optind++)
	{
		// options following --
		if(*remnant)
			fatal(pscats, remnant, " ");
		fatal(pscats, remnant, g_argv[optind]);
	}

	if(help || version || logcats || operators)
	{
		usage(1, 1, help, logcats, operators, opmask);
	}

	finally : coda;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.inputsl; x++)
		free(g_args.inputs[x].s);

	free(g_args.inputs);
/*
	for(x = 0; x < g_args.init_listl; x++)
		free(g_args.init_list[x]);

	free(g_args.init_list);
	free(g_args.init_list_lens);
	free(g_args.generator_file);
	free(g_args.init_file);
*/
}
