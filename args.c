/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "args.h"

#include <listwise.h>
#include <listwise/operator.h>
#include <listwise/ops.h>
#include <listwise/generator.h>
#include <listwise/lstack.h>

struct g_args_t g_args;

static void usage(int help)
{
	printf(
"listwise : list-based computation utility and library\n"
" v0.4.0\n"
"\n"
"usage : lw [options] generator-string\n"
"usage : lw --help|-h for this message\n"
"\n"
"------------------[options]-----------------------------------------------------\n"
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
"------------------[options]-----------------------------------------------------\n"
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

	exit(!help);
}

int parse_args(int argc, char** argv)
{
	int init_lista = 0;

	int c;
	while((c = getopt(argc, argv, "adhknz0f:i:")) != -1)
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
			default:
				usage(c == 'h');
		}
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
