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


#ifndef _ARGS_H
#define _ARGS_H

#define restrict __restrict

struct g_args_t
{
	char		dump;							// -d
	char		number;						// -n=1 -N=2
	char		in_null;					// -0
	char		out_null;					// -z
	char		out_stack;				// -k
	char		out_list;					// -a
	char *	generator_file;		// -f/-
	char *	init_file;				// -l

	char **	init_list;				// -i 
	int *		init_list_lens;		// -i 
	int			init_listl;				// -i
	int			init_lista;				// -i
} g_args;

int parse_args(const int argc, char ** const restrict argv, int * const restrict genx)
	__attribute__((nonnull));

void args_teardown();

#endif
