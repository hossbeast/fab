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

#ifndef _FF_PARSE_H
#define _FF_PARSE_H

#include "yyutil.h"

typedef struct
{
	/* yyu-defined xtra fields */
	yyu_extra;

	struct ff_file *		ff;					// fabfile being parsed
	struct ff_node *		ffn;				// ffn being parsed

	const char*					orig_base;	// ptr to original input string
	int									orig_len;		// length of original input string
	char*								act_base;		// ptr to lexer copy
} parse_param;

// defined in ff.tab.o (this declaration should be in ff.tab.h but isnt)
int ff_yyparse(void *, parse_param*);

// defined in ff.dsc.tab.o (this declaration should be in ff.dsc.tab.h but isnt)
int ff_dsc_yyparse(void *, parse_param*);

// defined in ff.var.tab.o (this declaration should be in ff.var.tab.h but isnt)
int ff_var_yyparse(void *, parse_param*);

// defined in ff.list.tab.o (this declaration should be in ff.var.tab.h but isnt)
int ff_list_yyparse(void*, parse_param*);

/// ff_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR | L_FF
//  log the last token under L_ERROR | L_FF
//
// DETAILS
//  called from tab.o and lex.o
//
void ff_yyerror(void * loc, void* scanner, parse_param* pp, char const* err)
	__attribute__((weakref("yyu_error")));

#endif
