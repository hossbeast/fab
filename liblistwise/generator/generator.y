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

%code requires {
	#define YYLTYPE generator_loc

	#define YYLLOC_DEFAULT(Cur, Rhs, N)					\
	do																					\
	{																						\
		if(N)																			\
		{																					\
			(Cur).f_lin = YYRHSLOC(Rhs, 1).f_lin;		\
			(Cur).f_col = YYRHSLOC(Rhs, 1).f_col;		\
			(Cur).l_lin = YYRHSLOC(Rhs, N).l_lin;		\
			(Cur).l_col = YYRHSLOC(Rhs, N).l_col;		\
																							\
			(Cur).s = YYRHSLOC(Rhs, 1).s;						\
			(Cur).e = YYRHSLOC(Rhs, N).e;						\
		}																					\
		else																			\
		{																					\
			(Cur).f_lin = YYRHSLOC(Rhs, 0).l_lin;		\
			(Cur).l_lin = YYRHSLOC(Rhs, 0).l_lin;		\
			(Cur).f_col = YYRHSLOC(Rhs, 0).l_col;		\
			(Cur).l_col = YYRHSLOC(Rhs, 0).l_col;		\
																							\
			(Cur).s = YYRHSLOC(Rhs, 0).e;						\
		}																					\
	} while(0)
}

%code top {
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>

	#include "generator.def.h"

	int  generator_yylex(void* yylocp, void* yylvalp, void* scanner);
}

%define api.pure
%error-verbose
%locations
%name-prefix="generator_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }

/* zero-based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
	operator *		op;
	int						bref;
	int64_t				i64;
	arg *					arg;
}

%token					STR
%token          LF
%token          WS
%token          '/'
%token	<bref>	BREF
%token  <i64>		I64
%token	<op>		OP

%type		<arg>		argument
%type   <arg>   string
%type   <arg>   strpart

%%

generator
	: opsep operations
	| operations
	;

operations
	: operation opsep operation
	| operation opsep
	| operation
	;

operation
	: OP argsep args
	| OP
	;

opsep
	: LF
	| WS
	;

args
	: argument argsep argument
	| argument
	;

argsep
	: '/'
	;

argument
	: string
	| I64
	{
		$$ = calloc(1, sizeof(*$$));

		$$->s = strdup(@1.s);
		$$->l = @1.e - @1.s;

		$$->itype = ITYPE_I64;
		$$->i64 = $1;
	}
	;

string
	: string strpart
	{
		$$ = $1;

		char* o = $$->s;

		// reallocate the string value of the argument
		$$->s = realloc($$->s, $$->l + $2->l + 1);
		memcpy($$->s + $$->l, $2->s, $2->l);
		$$->s[$$->l + $2->l] = 0;

		// update string pointers on previous references
		int x;
		for(x = 0; x < $$->refsl; x++)
		{
			$$->refs[x].s = $$->s + ($$->refs[x].s - o);
			$$->refs[x].e = $$->refs[x].s + $$->refs[x].l;
		}

		// use new reference, if there is one
		if($2->refs)
		{
			$$->refs = realloc($$->refs, ($$->refsl + 1) * sizeof(*$$->refs));

			$$->refs[$$->refsl].s = $$->s + $$->l;
			$$->refs[$$->refsl].l = $2->l;
			$$->refs[$$->refsl].e = $$->s + $$->l + $2->l;
			$$->refs[$$->refsl].k = REFTYPE_BREF;
			$$->refs[$$->refsl].bref = $2->refs[0].bref;

			$$->ref_last = &$$->refs[$$->refsl];
			$$->refsl++;
		}

		$$->l += $2->l;
		free($2->s);
		free($2);
	}
	| strpart
	;

strpart
	: STR
	{
		$$ = calloc(1, sizeof(*$$));

		$$->s = strdup(@1.s);
		$$->l = @1.e - @1.s;
	}
	| BREF
	{
		$$ = calloc(1, sizeof(*$$));

		$$->s = strdup(@1.s);
		$$->l = @1.e - @1.s;

		$$->refs = calloc(1, sizeof(*$$->refs));

		$$->refs[0].s = $$->s;
		$$->refs[0].l = $$->l;
		$$->refs[0].e = $$->s + $$->l;
		$$->refs[0].k = REFTYPE_BREF;
		$$->refs[0].bref = $1;

		$$->ref_last = &$$->refs[0];
		$$->refsl = 1;
	}
	;
