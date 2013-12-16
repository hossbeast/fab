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

%code top {
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>

	#include "generator.def.h"

	#include "wstdlib.h"

	// defined in generator.lex.o
	int generator_yylex(void* yylvalp, void* yyllocp, void* scanner);
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
	generator *		generator;
	operation **  operations;
	operation *		operation;
	operator *		op;
	arg **				args;

	arg *					arg;
	int						ref;
	int64_t				i64;
}

%token					STR
%token          LF
%token          WS
%token          '/'
%token          ','
%token          '{'
%token          '}'
%token	<ref>		BREF
%token	<ref>		CREF
%token	<ref>		HREF
%token  <i64>		I64
%token	<op>		OP

%type   <generator> generator
%type   <operation> operation
%type   <operations> operations
%type   <args>  args

%type		<arg>		arg
%type   <arg>   string

%right '/' ',' '.' ';' ':' '{' '}' '[' ']' '(' ')' '<' '>' STR

%%

utterance
	: opsep generator
	{
		parm->g = $2;
	}
	| generator
	{
		parm->g = $1;
	}
	;

generator
	: args opsep operations
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));
		$$->args = $1;
		$$->argsl = 0;

		typeof(*$1) * T = $1;
		while(*T)
		{
			$$->argsl++;
			T++;
		}

		$$->ops = $3;
		$$->opsl = parm->opsl;
	}
	| args
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));
		$$->args = $1;
		$$->argsl = parm->argsl;
	}
	| operations
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));
		$$->ops = $1;
		$$->opsl = parm->opsl;
	}
	;

operations
	: operations opsep operation
	{
		$$ = $1;
		if(parm->opsa == parm->opsl)
		{
			int ns = parm->opsa ?: 6;
			ns = ns * 2 + ns / 2;

			YYU_FATAL(wrealloc, &$$, sizeof(*$$), ns + 1, parm->opsa);
			parm->opsa = ns;
		}
		$$[parm->opsl++] = $3;
	}
	| operations operation
	{
		$$ = $1;
		if(parm->opsa == parm->opsl)
		{
			int ns = parm->opsa ?: 6;
			ns = ns * 2 + ns / 2;

			YYU_FATAL(wrealloc, &$$, sizeof(*$$), ns + 1, parm->opsa);
			parm->opsa = ns;
		}
		$$[parm->opsl++] = $2;
	}
	| operation
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$) * 2);
		parm->opsa = 1;
		parm->opsl = 0;
		$$[parm->opsl++] = $1;
	}
	;

opsep
	: opsep LF
	| opsep WS
	| LF
	| WS
	;

operation
	: operation argdiv args %prec STR
	{
		$$ = $1;
		$$->args = $3;
		$$->argsl = parm->argsl;
	}
	| OP
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));
		$$->op = $1;
	}
	;

args
	: args argdiv arg
	{
		$$ = $1;
		if(parm->argsa == parm->argsl)
		{
			int ns = parm->argsa ?: 3;
			ns = ns * 2 + ns / 2;
			YYU_FATAL(wrealloc, &$$, sizeof(*$$), ns + 1, parm->argsa);
			parm->argsa = ns;
		}
		$$[parm->argsl++] = $3;
	}
	| args divclose %prec STR
	{
		$$ = $1;
	}
	| arg
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$) * 2);
		parm->argsl = 0;
		parm->argsa = 1;
		$$[parm->argsl++] = $1;
	}
	;

argdiv
	: divsep divopen
	| divsep
	| divopen
	;

divclose
	: divclose argclose
	| argclose
	;

divsep
	: divsep argsep
	| argsep
	;

divopen
	: divopen argopen
	| argopen
	;

argsep
	: '/'
	| ','
	| '.'
	| ';'
	| ':'
	;

argopen
	: '{'
	| '['
	| '('
	| '<'
	;

argclose
	: '}'
	| ']'
	| ')'
	| '>'
	;

arg
	: arg string
	{
		$$ = $1;

		char* o = $$->s;

		// reallocate the string value of the argument
		YYU_FATAL(wrealloc, &$$->s, 1, $$->l + $2->l + 1, $$->l);
		memcpy($$->s + $$->l, $2->s, $2->l);
		$$->s[$$->l + $2->l] = 0;

		// update string pointers on previous references
		int x;
		for(x = 0; x < $$->refs.l; x++)
		{
			$$->refs.v[x].s = $$->s + ($$->refs.v[x].s - o);
			$$->refs.v[x].e = $$->refs.v[x].s + $$->refs.v[x].l;
		}

		// use new reference, if there is one
		if($2->refs.v)
		{
			YYU_FATAL(wrealloc, &$$->refs, sizeof(*$$), $$->refs.l + 1, $$->refs.l);

			$$->refs.v[$$->refs.l].s = $$->s + $$->l;
			$$->refs.v[$$->refs.l].l = $2->l;
			$$->refs.v[$$->refs.l].e = $$->s + $$->l + $2->l;
			$$->refs.v[$$->refs.l].k = $2->refs.v[0].k;
			$$->refs.v[$$->refs.l].ref = $2->refs.v[0].ref;

			$$->refs.last = &$$->refs.v[$$->refs.l];
			$$->refs.l++;
		}

		$$->l += $2->l;
		free($2->s);
		free($2);
	}
	| arg I64
	{
		$$ = $1;

		char* o = $$->s;

		// reallocate the string value of the argument
		YYU_FATAL(wrealloc, &$$->s, 1, $$->l + (@2.e - @2.s) + 1, $$->l);
		memcpy($$->s + $$->l, @2.s, (@2.e - @2.s));
		$$->s[$$->l + (@2.e - @2.s)] = 0;

		// update string pointers on previous references
		int x;
		for(x = 0; x < $$->refs.l; x++)
		{
			$$->refs.v[x].s = $$->s + ($$->refs.v[x].s - o);
			$$->refs.v[x].e = $$->refs.v[x].s + $$->refs.v[x].l;
		}

		$$->l += (@2.e - @2.s);
		$$->itype = 0;
	}
	| I64
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));

		$$->l = @1.e - @1.s;
		YYU_FATAL(wmalloc, &$$->s, $$->l + 1);
		memcpy($$->s, @1.s, $$->l);

		$$->itype = ITYPE_I64;
		$$->i64 = $1;
	}
	| string
	;

string
	: STR
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));

		$$->l = @1.e - @1.s;
		YYU_FATAL(wmalloc, &$$->s, $$->l + 1);
		memcpy($$->s, @1.s, $$->l);
	}
	| HREF
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));

		$$->l = 1;
		YYU_FATAL(wmalloc, &$$->s, $$->l + 1);
		$$->s[0] = $1;
	}
	| CREF
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));

		$$->l = 1;
		YYU_FATAL(wmalloc, &$$->s, $$->l + 1);
		$$->s[0] = $1;
	}
	| BREF
	{
		YYU_FATAL(wmalloc, &$$, sizeof(*$$));

		$$->l = 1;
		YYU_FATAL(wmalloc, &$$->s, $$->l + 1);
		$$->s[0] = $1;

		YYU_FATAL(wmalloc, &$$->refs.v, sizeof(*$$->refs.v));

		$$->refs.v[0].s = $$->s;
		$$->refs.v[0].l = $$->l;
		$$->refs.v[0].e = $$->s + $$->l;
		$$->refs.v[0].k = REFTYPE_BREF;
		$$->refs.v[0].ref = $1;

		$$->refs.last = &$$->refs.v[0];
		$$->refs.l = 1;
	}
	;
