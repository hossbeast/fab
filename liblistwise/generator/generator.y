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

	#include "xmem.h"

	// defined in generator.lex.o
	int generator_yylex(void* yylvalp, void* yyllocp, void* scanner);

	#define YYU_ERROR(...) dprintf(listwise_error_fd, #__VA_ARGS__)
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
	int						bref;
	int64_t				i64;
}

%token					STR
%token          LF
%token          WS
%token          '/'
%token	<bref>	BREF
%token  <i64>		I64
%token	<op>		OP

%type   <generator> generator
%type   <operation> operation
%type   <operations> operations
%type   <args>  args

%type		<arg>		arg
%type   <arg>   string
%type   <arg>   strpart

%right '/' STR

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
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));
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
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));
		$$->args = $1;
		$$->argsl = parm->argsl;
	}
	| operations
	{
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));
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

			YYU_FATAL(xrealloc, &$$, sizeof(*$$), ns + 1, parm->opsa);
			parm->opsa = ns;
		}
		$$[parm->opsl++] = $3;
	}
	| operation
	{
		YYU_FATAL(xmalloc, &$$, sizeof(*$$) * 2);
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
	: operation argsep args %prec STR
	{
		$$ = $1;
		$$->args = $3;
		$$->argsl = parm->argsl;
	}
	| OP
	{
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));
		$$->op = $1;
	}
	;

args
	: args argsep arg
	{
		$$ = $1;
		if(parm->argsa == parm->argsl)
		{
			int ns = parm->argsa ?: 3;
			ns = ns * 2 + ns / 2;
			YYU_FATAL(xrealloc, &$$, sizeof(*$$), ns + 1, parm->argsa);
			parm->argsa = ns;
		}
		$$[parm->argsl++] = $3;
	}
	| arg
	{
		YYU_FATAL(xmalloc, &$$, sizeof(*$$) * 2);
		parm->argsl = 0;
		parm->argsa = 1;
		$$[parm->argsl++] = $1;
	}
	;

argsep
	: '/'
	;

arg
	: string
	| I64
	{
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));

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
		YYU_FATAL(xrealloc, &$$->s, 1, $$->l + $2->l + 1, 0);
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
			YYU_FATAL(xrealloc, &$$->refs, sizeof(*$$), $$->refsl + 1, 0);

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
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));

		$$->s = strdup(@1.s);
		$$->l = @1.e - @1.s;
	}
	| BREF
	{
		YYU_FATAL(xmalloc, &$$, sizeof(*$$));

		$$->s = strdup(@1.s);
		$$->l = @1.e - @1.s;

		YYU_FATAL(xmalloc, &$$->refs, sizeof(*$$->refs));

		$$->refs[0].s = $$->s;
		$$->refs[0].l = $$->l;
		$$->refs[0].e = $$->s + $$->l;
		$$->refs[0].k = REFTYPE_BREF;
		$$->refs[0].bref = $1;

		$$->ref_last = &$$->refs[0];
		$$->refsl = 1;
	}
	;
