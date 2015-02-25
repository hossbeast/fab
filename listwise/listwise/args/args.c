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

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/operators.h"
#include "listwise/transform.h"

#include "xapi.h"
#include "LISTWISE.errtab.h"
#undef perrtab
#define perrtab perrtab_LISTWISE
#include "xlinux.h"

#include "args.h"
#include "logs.h"

#include "macros.h"
#include "parseint.h"

struct g_args_t g_args;

//
// static
//

static void usage(int valid, int version, int help, int logs, int ops, uint64_t opmask)
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
"usage : lw [ [ <option> ] [ <logexpr> ] [ <transform-expr> ] ] ...\n"
"\n"
" --help      this message\n"
" --version   version information\n"
" --logs      logexpr listing\n"
" --ops       listwise operator listing\n"
"\n"
"----------------- [ options ] ---------------------------------------------------------------\n"
"\n"
" -s                 (default) output only selected entries\n"
" -a                           output entire list, not just selected entries\n"
" -k                           output entire stack, not just top list\n"
" -n                 (default) number output rows 0 .. n\n"
" -N                           number output rows by list index\n"
"                    (default) separate output rows by newline\n"
" -Z                           separate output rows by null byte instead of by newline\n"
" -A                 (default) process subsequent <path> arguments line-wise (ascii)\n"
" -0                           process subsequent <path> arguments nullbyte-wise (null)\n"
" +<int>                       append subsequent non-options using priority <int> (default : 0)\n"
"\n"
" -t <arg>                     add <arg> to transform-expr (transform)\n"
" +t                 (default) treat subsequent non-options as -t <arg>\n"
" -T <path>                    read transform-expr from <path>\n"
" +T                           treat subsequent non-options as -T <path>\n"
"\n"
" -l <arg>                     add <arg> to initial list (list)\n"
" +l                           treat subsequent non-options as -l <arg>\n"
" -L <path>                    read initial list from <path>\n"
" +L                           treat subsequent non-options as -L <path>\n"
"\n"
" -H <path>                    read transform-expr and initial list from <path> (hybrid)\n"
" +H                           treat subsequent non-options as -H <path>\n"
#if DEBUG || DEVEL
"\n"
" --logtrace-no      (default) do not include file/function/line in log messages\n"
" --logtrace                   include file/function/line in log messages\n"
" --backtrace-pithy  (default) produce a summary of the callstack on error\n"
" --backtrace-full             produce a complete description of the callstack on error\n"
#endif
);
}

if(logs)
{
printf(
"\n"
"----------------- [ logexpr  ] -----------------------------------------------------------\n"
"\n"
" +<logcat> to enable logging category (use TAG for all)\n"
" -<logcat> to disable logging category (use TAG for all)\n"
"\n"
);

int x;
for(x = 0; x < g_logs_l; x++)
	printf("  %-10s %s\n", g_logs[x].s, g_logs[x].d);
}

if(ops)
{
int i = 0;
int x;
for(x = 0; x < g_ops_l; x++)
{
	if((g_ops[x]->optype & opmask) == opmask)
		i++;
}

// from LWOPT_TABLE in liblistwise/operator/operator.h
printf(
"\n"
"----------------- [ ops ] ---------------------------------------------------------\n"
"\n"
"options\n"
" --o<N>          0 < N < f      only list ops having property <N>\n"
" --o <opname>                   only list ops having the properties of operator <opname>\n"
"\n"
"%d ops matching 0x%"PRIx64"\n"
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

static int option(int type, int linewise, int rank, int bang, char * s)
{
	if(g_args.inputsl == g_args.inputsa)
	{
		size_t ns = g_args.inputsa ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, &g_args.inputs, sizeof(*g_args.inputs), ns, g_args.inputsa);
		g_args.inputsa = ns;
	}
	g_args.inputs[g_args.inputsl].linewise = linewise;
	g_args.inputs[g_args.inputsl].type = type;
	g_args.inputs[g_args.inputsl].rank = rank;
	g_args.inputs[g_args.inputsl].order = g_args.inputsl;
	g_args.inputs[g_args.inputsl].bang = bang;
	fatal(ixstrdup, &g_args.inputs[g_args.inputsl].s, s);
	g_args.inputsl++;

	finally : coda;
}

//
// public
//

int args_parse()
{
	int help = 0;
	int version = 0;
	int	logs = 0;
	int ops = 0;
	uint64_t opmask = 0;

	struct option longopts[] = {
		  { "help"												, no_argument				, &help, 1 }
		, { "args"												, no_argument				, &help, 1 }
		, { "params"											, no_argument				, &help, 1 }
		, { "options"											, no_argument				, &help, 1 }
		, { "opts"												, no_argument				, &help, 1 }
		, { "version"											, no_argument				, &version, 1 }
		, { "vers"												, no_argument				, &version, 1 }
		, { "vrs"													, no_argument				, &version, 1 }
		, { "log"													, no_argument				, &logs, 1 }
		, { "logs"												, no_argument				, &logs, 1 }
		, { "logcat"											, no_argument				, &logs, 1 }
		, { "logcats"											, no_argument				, &logs, 1 }
		, { "logexpr"											, no_argument				, &logs, 1 }
		, { "logexprs"										, no_argument				, &logs, 1 }
		, { "ops"													, no_argument				, &ops, 1 }
		, { "oplist"											, no_argument				, &ops, 1 }
		, { "oplists"											, no_argument				, &ops, 1 }
		, { "operator"										, no_argument				, &ops, 1 }
		, { "operators"										, no_argument				, &ops, 1 }
		, { "o"														, required_argument	, 0, 0 }
		, { "op"													, required_argument	, 0, 0 }
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
		, { "oA"													, no_argument				, 0, 0 }
		, { "ob"													, no_argument				, 0, 0 }
		, { "oB"													, no_argument				, 0, 0 }
		, { "oc"													, no_argument				, 0, 0 }
		, { "oC"													, no_argument				, 0, 0 }
		, { "od"													, no_argument				, 0, 0 }
		, { "oD"													, no_argument				, 0, 0 }
		, { "oe"													, no_argument				, 0, 0 }
		, { "oE"													, no_argument				, 0, 0 }
		, { "of"													, no_argument				, 0, 0 }
		, { "oF"													, no_argument				, 0, 0 }

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
		"saknNZA0"

		// with-argument switches
		"t:T:l:L:H:"
	;

	//
	// args:defaults
	//
#if DEBUG || DEVEL
	g_args.mode_backtrace		= DEFAULT_MODE_BACKTRACE;
	g_args.mode_logtrace		= DEFAULT_MODE_LOGTRACE;
#endif

	g_args.mode_output			= DEFAULT_MODE_OUTPUT;
	g_args.mode_numbering		= DEFAULT_MODE_OUTPUT_NUMBERING;
	g_args.mode_separator		= DEFAULT_MODE_OUTPUT_SEPARATOR;

	// process <path> linewise
	int linewise = 1;

	// non-options mode
	int non_options = INPUT_TYPE_TRANSFORM_ITEM;
	int8_t prevailing_rank = 0;

	int x;
	int indexptr;
	opterr = 0;
	while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], switches, longopts, &indexptr)) != -1)
	{
		if(x == 0)
		{
			if(strcmp(longopts[indexptr].name, "o") == 0 || strcmp(longopts[indexptr].name, "op") == 0)
			{
				struct operator * op = 0;
				if((op = op_lookup(optarg, strlen(optarg))))
					opmask |= op->optype;

				ops = 1;
			}
			else if(longopts[indexptr].name[0] == 'o' && strlen(longopts[indexptr].name) == 2)
			{
				int off = 0;
				int c = longopts[indexptr].name[1];
				if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				{
					if(c >= '0' && c <= '9')
						off = c - '0';
					else if(c >= 'a' && c <= 'f')
						off = 10 + (c - 'a');
					else if(c >= 'A' && c <= 'F')
						off = 10 + (c - 'A');

					opmask |= (0x01ULL << (off - 1));
					ops = 1;
				}
			}
		}
		else if(x == 's')
		{
			g_args.mode_output = MODE_OUTPUT_SELECTED;
		}
		else if(x == 'a')
		{
			g_args.mode_output = MODE_OUTPUT_LIST;
		}
		else if(x == 'k')
		{
			g_args.mode_output = MODE_OUTPUT_STACK;
		}
		else if(x == 'n')
		{
			g_args.mode_numbering = MODE_OUTPUT_NUMBERING_ASC;
		}
		else if(x == 'N')
		{
			g_args.mode_numbering = MODE_OUTPUT_NUMBERING_INDEX;
		}
		else if(x == 'Z')
		{
			g_args.mode_separator = MODE_OUTPUT_SEPARATOR_NULL;
		}
		else if(x == '0')
		{
			linewise = 0;
		}
		else if(x == 'A')
		{
			linewise = 1;
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
		else
		{
			if(optarg[0] == '+')
			{
				if(strcmp(optarg + 1, "t") == 0)
					non_options = INPUT_TYPE_TRANSFORM_ITEM;
				else if(strcmp(optarg + 1, "T") == 0)
					non_options = INPUT_TYPE_TRANSFORM_FILE;
				else if(strcmp(optarg + 1, "l") == 0)
					non_options = INPUT_TYPE_INIT_LIST_ITEM;
				else if(strcmp(optarg + 1, "L") == 0)
					non_options = INPUT_TYPE_INIT_LIST_FILE;
				else if(strcmp(optarg + 1, "H") == 0)
					non_options = INPUT_TYPE_HYBRID_FILE;
				else if(parseint(optarg + 1, SCNd8, INT8_MIN, INT8_MAX, 1, UINT8_MAX, &prevailing_rank, 0) == 0)
				{
					// rank
				}
				else
					failf(LISTWISE_BADARGS, "unknown argument : %s", optarg);
			}
			else
			{
				int type = non_options;
				int rank = prevailing_rank;
				if(x == 't')
					type = INPUT_TYPE_TRANSFORM_ITEM;
				else if(x == 'T')
					type = INPUT_TYPE_TRANSFORM_FILE;
				else if(x == 'l')
					type = INPUT_TYPE_INIT_LIST_ITEM;
				else if(x == 'L')
					type = INPUT_TYPE_INIT_LIST_FILE;
				else if(x == 'H')
					type = INPUT_TYPE_HYBRID_FILE;
				else
				{
					// -<arg> : always rank 0
					rank = 0;
				}

				fatal(option, type, linewise, rank, optarg == g_interpreting, optarg);
			}
		}
	}

	// read transform-expr from stdin unless it has been mentioned for some other purpose
	g_args.stdin_init_list_items = 1;
	for(x = 0; x < g_args.inputsl; x++)
	{
		if(g_args.inputs[x].type != INPUT_TYPE_INIT_LIST_ITEM && strcmp(g_args.inputs[x].s, "-") == 0)
		{
			g_args.stdin_init_list_items = 0;
			break;
		}
	}

	// process stdin using the prevailing method at the end of argument processing
	g_args.stdin_linewise = linewise;

	// options following --
	for(; optind < g_argc; optind++)
		fatal(option, non_options, linewise, prevailing_rank, g_argv[optind] == g_interpreting, g_argv[optind]);

	// sort according to rank
	int compar(typeof(*g_args.inputs) * A, typeof(*g_args.inputs) * B)
	{
		return A->rank - B->rank ?: A->order - B->order;
	};
	qsort(g_args.inputs, g_args.inputsl, sizeof(*g_args.inputs), (void*)compar);

#if 0
for(x = 0; x < g_args.inputsl; x++)
	printf("[%2d] rank=%d bang=%d linewise=%d type=%s %s\n", x, g_args.inputs[x].rank, g_args.inputs[x].bang, g_args.inputs[x].linewise, INPUT_TYPE_STR(g_args.inputs[x].type), g_args.inputs[x].s);
#endif

	if(help || version || logs || ops)
	{
		usage(1, 1, help, logs, ops, opmask);
	}

	finally : coda;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.inputsl; x++)
		free(g_args.inputs[x].s);

	free(g_args.inputs);
}
