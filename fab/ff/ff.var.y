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
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>

	#include "xlinux.h"

	#include "ff.parse.h"
	#include "ffn.h"

	static int __attribute__((weakref, alias("ff_yylex"))) ff_var_yylex(); 
	static int __attribute__((weakref, alias("ff_yyerror"))) ff_var_yyerror(); 
}

%define api.pure
%error-verbose
%name-prefix="ff_var_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
	struct ff_node*			node;
}

/* terminals */
%right WORD

/* nonterminals */
%type  <node> statement
%type  <node> statements
%type  <node> varassign
%type  <node> varadd
%type  <node> varsub
%type  <node> varxfm
%type  <node> list
%type  <node> listparts
%type  <node> listpart
%type  <node> word
%type  <node> varref
%type  <node> varrefs
%type  <node> nofile
%type  <node> nofileparts
%type  <node> generator

/* sugar */
%token END 0 "end of file"

%destructor { ffn_free($$); } <node>
%destructor { free($$.v); } <wordparts>

%%

ff
	: statements
	{
		YYU_FATAL(ffn_mknode, &parm->ffn, &@$, FFN_STMTLIST, $1->s, $1->e, $1);
	}
	;

statements
	: statements statement
	{
		$$ = ffn_addchain($1, $2);
	}
	| statement
	;

statement
	: varassign
	| varadd
	| varsub
	| varxfm
	;

varassign
	: varrefs '=' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_VARASSIGN, $1->s, $3->e, $1, $3);
	}
	;

varadd
	: varrefs '+' '=' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_VARXFM_ADD, $1->s, $4->e, $1, $4);
	}
	;

varsub
	: varrefs '-' '=' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_VARXFM_SUB, $1->s, $4->e, $1, $4);
	}
	;

varxfm
	: varrefs '=' '~' generator
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_VARXFM_LW, $1->s, $4->e, $1, $4, (void*)0);
	}
	| varrefs '=' '~' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_VARXFM_LW, $1->s, $4->e, $1, (void*)0, $4);
	}
	;

list
	: '[' listparts ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, $2      , (void*)0, (void*)0);
	}
	| '[' ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, (void*)0, (void*)0, (void*)0);
	}
	| '[' listparts '~' generator ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, $2      , $4      , (void*)0);
	}
	| '[' '~' generator ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, (void*)0, $3, (void*)0);
	}
	| '[' listparts '~' list ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, $2      , (void*)0, $4);
	}
	| '[' '~' list ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, (void*)0, (void*)0, $3);
	}
	;

listparts
	: listparts listpart
	{
		$$ = ffn_addchain($1, $2);
	}
	| listpart
	;

listpart
	: varref
	| word %prec WORD
	| nofile
	| list
	;

varrefs
	: varrefs varref
	{
		$$ = ffn_addchain($1, $2);
	}
	| varref
	;

varref
	: '$' word %prec WORD
	{
		$$ = $2;
		$$->type = FFN_VARREF;
	}
	;

generator
	: word
	{
		$$ = $1;
		$$->type = FFN_TRANSFORM;
	}
	;

nofile
	: '@' nofileparts
	{
		/* nofile is a shorthand for non-file-backed notation */

		// parts
		char ** l = 0;
		int ll = 0;
		int la = 0;

		// text
		char * v = 0;

		// e
		char * e = $2->e;

		// add to text
		YYU_FATAL(ixstrcatf, &v, "/..");

		// add to parts
		if(ll == la)
		{
			int ns = la ?: 3;
			ns = ns * 2 + ns / 2;
			YYU_FATAL(xrealloc, &l, sizeof(*l), ns, la);
			la = ns;
		}
		YYU_FATAL(ixstrdup, &l[ll++], "..");

		ff_node * n = $2;
		while(n)
		{
			// add to text
			YYU_FATAL(ixstrcatf, &v, "/%s", n->text);

			// add to parts
			if(ll == la)
			{
				int ns = la ?: 3;
				ns = ns * 2 + ns / 2;
				YYU_FATAL(xrealloc, &l, sizeof(*l), ns, la);
				la = ns;
			}
			YYU_FATAL(ixstrdup, &l[ll++], n->text);

			ff_node * nn = n->next;
			n->next = 0;
			ffn_free(n);
			n = nn;
		}

		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_NOFILE, @1.s, e, v, l, ll);
	}
	;

nofileparts
	: nofileparts '.' word %prec WORD
	{
		$$ = ffn_addchain($1, $3);
	}
	| word %prec WORD
	;

word
	: word WORD
	{
		$$ = $1;
		YYU_FATAL(ixstrncat, &$$->text, @1.s, @1.e - @1.s);
	}
	| WORD
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_WORD);
	}
	;
