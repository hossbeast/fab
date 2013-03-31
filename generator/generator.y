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

%code top {
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>

	#include "generator.def.h"

	int  generator_yylex(void* yylvalp, void* scanner);
	void generator_yyerror(void* scanner, parse_param* pp, char const* err);
}

%define api.pure
%token-table
%error-verbose
%name-prefix="generator_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }
%expect 8

%union {
	struct								// string segment
	{
		char*				s;
		int					l;
	}							str;

	struct
	{
		char *			s;
		int					l;
		operator *	v;
	}							op;

	struct
	{
		char *			s;
		int					l;
		int					v;
	}							bref;

	struct
	{
		char *			s;
		int					l;
		int64_t			v;
	}							i64;

	arg *					arg;
}

%token	<str>			'/'
%token	<str>			LF

%token	<str>			STR
%token	<bref>		BREF
%token  <i64>		  I64
%token	<op>			OP

%type		<arg>			arg
%type		<arg>			string

/* sugar */
%token END 0 "end of file"

%%

items
	: items items
	| '/'
	| LF
	| OP
	{
		if(parm->opsa == parm->g->opsl)
		{
			int ns = parm->opsa ?: 10;
			ns = ns * 2 + ns / 2;

			parm->g->ops = realloc(parm->g->ops, ns * sizeof(*parm->g->ops));
			parm->opsa = ns;
		}

		parm->g->ops[parm->g->opsl] = calloc(1, sizeof(*parm->g->ops[0]));
		parm->g->ops[parm->g->opsl]->op = $1.v;
		parm->g->opsl++;

		parm->opargsa = 0;
		free($1.s);
	}
	| arg
	{
		if(parm->g->opsl == 0)
		{
			if(parm->argsa == parm->g->argsl)
			{
				int ns = parm->argsa ?: 10;
				ns = ns * 2 + ns / 2;

				parm->g->args = realloc(parm->g->args, ns * sizeof(*parm->g->args));
				parm->argsa = ns;
			}

			parm->g->args[parm->g->argsl++] = $1;
		}
		else
		{
			if(parm->opargsa == parm->g->ops[parm->g->opsl - 1]->argsl)
			{
				int ns = parm->opargsa ?: 10;
				ns = ns * 2 + ns / 2;

				parm->g->ops[parm->g->opsl - 1]->args = realloc(parm->g->ops[parm->g->opsl - 1]->args, ns * sizeof(*parm->g->ops[0]->args));
				parm->opargsa = ns;
			}

			parm->g->ops[parm->g->opsl - 1]->args[parm->g->ops[parm->g->opsl - 1]->argsl++] = $1;
		}
	} 
	;

arg
	: string
	| I64
	{
		$$ = calloc(1, sizeof(*$$));

		$$->l = $1.l;
		$$->s = $1.s;
		$$->itype = ITYPE_I64;
		$$->i64 = $1.v;
	}
	;

string
	: string STR
	{
		$$ = $1;

		char* o = $$->s;

		// reallocate the string value of the argument
		$$->s = realloc($$->s, $$->l + $2.l + 1);
		memcpy($$->s + $$->l, $2.s, $2.l);
		$$->s[$$->l + $2.l] = 0;

		// update string pointers on previous references
		int x;
		for(x = 0; x < $$->refsl; x++)
		{
			$$->refs[x].s = $$->s + ($$->refs[x].s - o);
			$$->refs[x].e = $$->refs[x].s + $$->refs[x].l;
		}

		$$->l += $2.l;
		free($2.s);
	}
	| string BREF
	{
		$$ = $1;

		char* o = $$->s;

		// reallocate the string value of the argument
		$$->s = realloc($$->s, $$->l + $2.l + 1);
		memcpy($$->s + $$->l, $2.s, $2.l);
		$$->s[$$->l + $2.l] = 0;

		// update string pointers on previous references
		int x;
		for(x = 0; x < $$->refsl; x++)
		{
			$$->refs[x].s = $$->s + ($$->refs[x].s - o);
			$$->refs[x].e = $$->refs[x].s + $$->refs[x].l;
		}

		// use new reference, if there is one
		$$->refs = realloc($$->refs, ($$->refsl + 1) * sizeof(*$$->refs));

		$$->refs[$$->refsl].s = $$->s + $$->l;
		$$->refs[$$->refsl].l = $2.l;
		$$->refs[$$->refsl].e = $$->s + $$->l + $2.l;
		$$->refs[$$->refsl].k = REFTYPE_BREF;
		$$->refs[$$->refsl].bref = $2.v;

		$$->ref_last = &$$->refs[$$->refsl];
		$$->refsl++;

		$$->l += $2.l;
		free($2.s);
	}
	| STR
	{
		$$ = calloc(1, sizeof(*$$));

		$$->s = $1.s;
		$$->l = $1.l;
	}
	| BREF
	{
		$$ = calloc(1, sizeof(*$$));

		$$->s = $1.s;
		$$->l = $1.l;

		$$->refs = calloc(1, sizeof(*$$->refs));

		$$->refs[0].s = $$->s;
		$$->refs[0].l = $$->l;
		$$->refs[0].e = $$->s + $$->l;
		$$->refs[0].k = REFTYPE_BREF;
		$$->refs[0].bref = $1.v;

		$$->ref_last = &$$->refs[0];
		$$->refsl = 1;
	}
	;





