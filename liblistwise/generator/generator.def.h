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

#ifndef _GENERATOR_DEF_H
#define _GENERATOR_DEF_H

#include <stdio.h>
#include <stdint.h>

#include "listwise/internal.h"

typedef struct
{
	int			f_lin;
	int			f_col;
	int			l_lin;
	int			l_col;

	char *	s;
	char *	e;
} generator_loc;

typedef struct
{
	generator*			g;						// completed generator goes here
	void*						scanner;

	int							states[64];		// start states stack
	int							states_n;

	generator_loc 	loc;					// running location track for this parse
	generator_loc		last_loc;			// location of last parsed token
	int							last_tok;			// last parsed token

	char						space[256];		// temp space
	char						space2[256];

	int							argsa;
	int							opsa;
	int							opargsa;

	int							r;						// return value of the parse ; zeroed in yyerror
} parse_param;

/// generator_yyerror
//
// SUMMARY
//  1. flag the parse as failed by (setting pp->r = 0)
//  2. write the last error to listwise_error_fd
//  3. write the last token to listwise_error_fd
//
// DETAILS
//  called from tab.o and lex.o
//
void generator_yyerror(void* loc, void* scanner, parse_param* pp, char const* err);

#endif
