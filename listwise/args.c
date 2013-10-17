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

#include "listwise_control.h"

#include "args.h"

#include "macros.h"
#include "xmem.h"
#include "xstring.h"

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

static void usage(int valid, int version, int help, int operators, struct optypes * optypes)
{
	printf(
"listwise : list transformation utility\n"
);
if(version)
{
	printf(" fab-"
#if DBUG
	XQUOTE(FABVERSIONS) "+DBUG"
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
"usage : lw [options] generator-string\n"
" --help|-h   : this message\n"
" --version   : version information\n"
" --operators : operator listing\n"
"\n"
"----------------- [ operator listing options] ----------------------------------\n"
"\n"
" --o1       type 1        restrict listing to those operators of matching type\n"
" --o2       type 2\n"
"  ...\n"
" --od       type d\n"
"\n"
"----------------- [ listwise execution options ] -------------------------------\n"
"\n"
" -a         output entire list, not just selected entries\n"
" -k         output entire stack, not just top list\n"
" -d         dump list-stack at each step during execution (debug)\n"
"            do not suppress liblistwise info/warn messages or error traces\n"
" -n         number output rows 0 .. n\n"
" -N         number output rows with their list index\n"
" -z         separate output rows with null byte instead of newline\n"
" -0         separate input rows read from file by null byte instead of newline\n"
" -i <item>  initial list item\n"
" -l <path>  read initial list items from <path>\n"
"            <path> of - means read from stdin\n"
" -f <path>  read generator-string from <path> instead of argv\n"
"            <path> of - means read from stdin\n"
" -          equal to -f -\n"
"\n"
	);
}
if(operators)
{
	if(!help)
		printf("\n");

	printf(
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
	printf("\n");
}

if(help || operators)
{
	printf(
"For more information visit http://fabutil.org\n"
	);
}

	printf("\n");
	exit(!valid);
}

int parse_args(const int argc, char ** const argv, int * const genx)
{
	int help = 0;
	int version = 0;
	int operators = 0;

	struct optypes optypes = {};

	struct option longopts[] = {
		  { "help"				, no_argument	, &help, 1 } 
		, { "version"			, no_argument	, &version, 1 } 
		, { "operators"		, no_argument	, &operators, 1 } 
		, { "o1"					, no_argument	, &optypes._1, 1 }
		, { "o2"					, no_argument	, &optypes._2, 1 }
		, { "o3"					, no_argument	, &optypes._3, 1 }
		, { "o4"					, no_argument	, &optypes._4, 1 }
		, { "o5"					, no_argument	, &optypes._5, 1 }
		, { "o6"					, no_argument	, &optypes._6, 1 }
		, { "o7"					, no_argument	, &optypes._7, 1 }
		, { "o8"					, no_argument	, &optypes._8, 1 }
		, { "o9"					, no_argument	, &optypes._9, 1 }
		, { "oa"					, no_argument	, &optypes._a, 1 }
		, { "ob"					, no_argument	, &optypes._b, 1 }
		, { "oc"					, no_argument	, &optypes._c, 1 }
		, { "od"					, no_argument	, &optypes._d, 1 }
		, { }
	};

	char * switches =
		// no-argument switches
		"adhknz0N"

		// with-argument switches
		"f:l:i:"
	;

	int c;
	int indexptr;
	opterr = 1;
	while((c = getopt_long(argc, argv, switches, longopts, &indexptr)) != -1)
	{
		switch(c)
		{
			case 'a':
				g_args.out_list = 1;
				break;
			case 'd':
				g_args.dump = 1;
				break;
			case 'k':
				g_args.out_stack = 1;
				break;
			case 'n':
				g_args.number = 1;
				break;
			case 'N':
				g_args.number = 2;
				break;
			case 'z':
				g_args.out_null = 1;
				break;
			case '0':
				g_args.in_null = 1;
				break;
			case 'f':
				free(g_args.generator_file);
				g_args.generator_file = strdup(optarg);
				break;
			case 'l':
				free(g_args.init_file);
				g_args.init_file = strdup(optarg);
				break;
			case 'i':
				{
					if(g_args.init_listl == g_args.init_lista)
					{
						int ns = g_args.init_lista ?: 10;
						ns = ns * 2 + ns / 2;

						fatal(xrealloc, &g_args.init_list, sizeof(*g_args.init_list), ns, g_args.init_lista);
						fatal(xrealloc, &g_args.init_list_lens, sizeof(*g_args.init_list_lens), ns, g_args.init_lista);
						g_args.init_lista = ns;
					}
					g_args.init_list[g_args.init_listl] = strdup(optarg);
					g_args.init_list_lens[g_args.init_listl] = strlen(optarg);
					g_args.init_listl++;
				}
				break;
			case 'h':
				usage(1, 1, 1, 0, 0);
				break;
			case '?':
				usage(0, 1, 1, 0, 0);
				break;
		}
	}

	if(help || version || operators)
	{
		usage(1, 1, help, operators, &optypes);
	}

	if(optind < argc && strcmp(argv[optind], "-") == 0)
	{
		fatal(xstrdup, &g_args.generator_file, "-");
		optind++;
	}

	(*genx) = optind;

	finally : coda;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.init_listl; x++)
		free(g_args.init_list[x]);

	free(g_args.init_list);
	free(g_args.generator_file);
	free(g_args.init_file);
	free(g_args.init_list_lens);
}
