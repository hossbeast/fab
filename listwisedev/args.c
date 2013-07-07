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

#include <listwise.h>
#include <listwise/operator.h>
#include <listwise/ops.h>
#include <listwise/generator.h>
#include <listwise/lstack.h>

#include "args.h"

#include "macros.h"

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

int parse_args(int argc, char** argv)
{
	int init_lista = 0;

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
		"f:i:"
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
			case 'i':
				{
					if(g_args.init_listl == init_lista)
					{
						int ns = init_lista ?: 10;
						ns = ns * 2 + ns / 2;

						g_args.init_list = realloc(g_args.init_list, sizeof(*g_args.init_list) * ns);
						init_lista = ns;
					}
					g_args.init_list[g_args.init_listl++] = strdup(optarg);
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
		free(g_args.generator_file);
		g_args.generator_file = strdup("-");
		optind++;
	}

	return optind;
}

void args_teardown()
{
	free(g_args.generator_file);
}
