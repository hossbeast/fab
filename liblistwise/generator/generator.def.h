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

#include "yyutil.h"

typedef struct
{
	/* yyu-defined xtra fields */
	yyu_extra;

	generator * 		g;						// parsing results

	char						temp[256];		// temp space
	char *					name;					// input name
	int							namel;
	uint32_t				scanmode;			// one of GENSCAN_*

	/* intermediary allocation tracking */
	int							opsl;
	int							opsa;
	int							argsl;
	int							argsa;
} parse_param;

// defined in generator.tab.o
int generator_yyparse(void*, parse_param*);

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
static void generator_yyerror(void* loc, void* scanner, parse_param* pp, char const* err)
	__attribute__((weakref("yyu_error")));

#endif
