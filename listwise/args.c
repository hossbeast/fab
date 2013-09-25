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

static void usage(int valid, int version, int help, int operators)
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
"  --help|-h   : this message\n"
"  --version   : version information\n"
"  --operators : operator listing\n"
"\n"
"----------------- [ options ] --------------------------------------------------\n"
"\n"
" -a         output entire list, not just selected entries\n"
" -k         output entire stack, not just top list\n"
"            implies -a\n"
" -d         dump list-stack at each step during execution\n"
" -n         number output items\n"
" -z         separate output items with null byte instead of newline\n"
" -0         separate input items read from file by null byte instead of newline\n"
" -i <item>  initial list item\n"
" -l <path>  read initial list items from <path>\n"
" -f <path>  read generator-string from <path> instead of argv\n"
"            <path> of - indicates read from stdin\n"
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

	int x;
	for(x = 0; x < g_ops_l; x++)
	{
		printf("[%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c] %6s - %s"
/* have some effect */
			, g_ops[x]->optype & LWOP_SELECTION_STAGE ? 'x' : ' '
			, g_ops[x]->optype & LWOP_SELECTION_ACTIVATE ? 'x' : ' '
			, g_ops[x]->optype & LWOP_SELECTION_RESET				? 'x' : ' '
			, g_ops[x]->optype & LWOP_WINDOWS_STAGE ? 'x' : ' '
			, g_ops[x]->optype & LWOP_WINDOWS_ACTIVATE ? 'x' : ' '
			, g_ops[x]->optype & LWOP_WINDOWS_RESET				? 'x' : ' '
			, g_ops[x]->optype & LWOP_ARGS_CANHAVE					? 'x' : ' '
			, g_ops[x]->optype & LWOP_EMPTYSTACK_YES ? 'x' : ' '

/* informational */
			, g_ops[x]->optype & LWOP_STACKOP ? 'x' : ' '
			, g_ops[x]->optype & LWOP_MODIFIERS_CANHAVE					? 'x' : ' '
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
	}

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

	struct option longopts[] = {
		  { "help"				, no_argument	, &help, 1 } 
		, { "version"			, no_argument	, &version, 1 } 
		, { "operators"		, no_argument	, &operators, 1 } 
		, { }
	};

	char * switches =
		// no-argument switches
		"adhknz0"

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
				g_args.out_list = 1;
				break;
			case 'n':
				g_args.number = 1;
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
				usage(1, 1, 1, 0);
				break;
			case '?':
				usage(0, 1, 1, 0);
				break;
		}
	}

	if(help || version || operators)
	{
		usage(1, 1, help, operators);
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
