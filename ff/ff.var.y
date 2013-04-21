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
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.parse.h"
	#include "ffn.h"

	#include "xstring.h"
	#include "xmem.h"

	static int __attribute__((weakref, alias("ff_yylex"))) ff_var_yylex(); 
	static int __attribute__((weakref, alias("ff_yyerror"))) ff_var_yyerror(); 
}

%define api.pure
%error-verbose
%name-prefix="ff_var_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }
%expect 4

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
	struct
	{
		const char*			s;
		const char*			e;
	}							str;

	struct
	{
		const char*			s;
		const char*			e;

		char*						v;
	}							wordparts;

	struct
	{
		const char*			s;
		const char*			e;
		uint32_t				v;
	}							num;

	ff_node*			node;
}

/* terminals with a semantic value */
%token <str> WORD		"WORD"			/* string token */
%token <str> GWORD	"GWORD"			/* whitespace-delimited generator-string token */
%token <str> QWORD	"QWORD"			/* double-quote delimited string token */
%token <str> WS			"WS"				/* single tab/space */
%token <num> '$'								/* variable reference */
%token <num> '@'								/* nofile reference */
%token <num> '.'								/* nofile scoping */
%token <num> '['								/* list start */
%token <num> ']'								/* list end */
%token <num> ','								/* list element separator */
%token <num> '~'  							/* list transformation via listwise */
%token <num> '='								/* variable assignment */
%token <num> '-'								/* variable link */

/* nonterminals */
%type  <wordparts> gwordparts
%type  <wordparts> qwordparts

%type  <node> statement
%type  <node> statements
%type  <node> varassign
%type  <node> varadd
%type  <node> varsub
%type  <node> varxfm
%type  <node> list
%type  <node> listpartsnone
%type  <node> listpartscomma
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
		parm->ffn = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_STMTLIST, $1->s, $1->e, $1);
	}
	;

statements
	: statements statements
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
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $3->e, $1, $3);
	}
	;

varadd
	: varrefs '+' '=' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARADD, $1->s, $4->e, $1, $4);
	}
	;

varsub
	: varrefs '-' '=' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARSUB, $1->s, $4->e, $1, $4);
	}
	;

varxfm
	: varrefs '=' '~' generator
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARXFM, $1->s, $4->e, $1, $4, (void*)0);
	}
	| varrefs '=' '~' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARXFM, $1->s, $4->e, $1, (void*)0, $4);
	}
	;

list
	: '[' listpartsnone ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2      , (void*)0, (void*)0, FFN_WSSEP);
	}
	| '[' listpartscomma ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2      , (void*)0, (void*)0, FFN_COMMASEP);
	}
	| '[' listpart ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2      , (void*)0, (void*)0, 0);
	}
	| '[' ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $2.e, (void*)0, (void*)0, (void*)0, 0);
	}
	| '[' listpartsnone '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2      , $4      , (void*)0, FFN_WSSEP);
	}
	| '[' listpartscomma '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2      , $4      , (void*)0, FFN_COMMASEP);
	}
	| '[' listpart '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2      , $4      , (void*)0, 0);
	}
	| '[' '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $4.e, (void*)0, (void*)0, $3      , 0);
	}
	| '[' listpartsnone '~' list ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2      , (void*)0, $4      , FFN_WSSEP);
	}
	| '[' listpartscomma '~' list ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2      , (void*)0, $4      , FFN_COMMASEP);
	}
	| '[' listpart '~' list ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2      , (void*)0, $4      , 0);
	}
	| '[' '~' list ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $4.e, (void*)0, (void*)0, $3      , 0);
	}
	;

listpartsnone
	: listpartsnone listpart
	{
		$$ = ffn_addchain($1, $2);
	}
	| listpart listpart
	{
		$$ = ffn_addchain($1, $2);
	}
	;

listpartscomma
	: listpartscomma ',' listpart
	{
		$$ = ffn_addchain($1, $3);
	}
	| listpart ',' listpart
	{
		$$ = ffn_addchain($1, $3);
	}
	;

listpart
	: varref
	| word
	| nofile
	| list
	;

varrefs
	: varrefs varrefs
	{
		$$ = ffn_addchain($1, $2);
	}
	| varref
	;

varref
	: '$' word
	{
		$$ = $2;
		$$->type = FFN_VARREF;
	}
	;

generator
	: word
	{
		$$ = $1;
		$$->type = FFN_GENERATOR;
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
		xstrcatf(&v, "/..");

		// add to parts
		if(ll == la)
		{
			int ns = la ?: 3;
			ns = ns * 2 + ns / 2;
			xrealloc(&l, sizeof(*l), ns, la);
			la = ns;
		}
		xstrdup(&l[ll++], "..");

		ff_node * n = $2;
		while(n)
		{
			// add to text
			xstrcatf(&v, "/%s", n->text);

			// add to parts
			if(ll == la)
			{
				int ns = la ?: 3;
				ns = ns * 2 + ns / 2;
				l = realloc(l, sizeof(*l) * ns);
				la = ns;
			}
			xstrdup(&l[ll++], n->text);

			ff_node * nn = n->next;
			n->next = 0;
			ffn_free(n);
			n = nn;
		}

		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_NOFILE, $1.s, e, v, l, ll);
	}
	;

nofileparts
	: nofileparts '.' word
	{
		$$ = ffn_addchain($1, $3);
	}
	| word
	;

word
	: qwordparts
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, $1.v);
	}
	| gwordparts
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, $1.v);
	}
	| WORD
	{
		char* v = calloc(1, ($1.e - $1.s) + 1);
		memcpy(v, $1.s, $1.e - $1.s);

		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, v);
	}
	;

gwordparts
	: gwordparts GWORD
	{
		$$ = $1;

		int l = strlen($$.v);
		int nl = $2.e - $2.s;

		$$.v = realloc($$.v, l + nl + 1);
		memcpy($$.v + l, $2.s, nl);
		$$.v[l + nl] = 0;

		$$.e = (char*)$2.e;
	}
	| GWORD
	{
		$$.s = $1.s;
		$$.e = $1.e;

		$$.v = calloc(1, ($$.e - $$.s) + 1);
		memcpy($$.v, $$.s, $$.e - $$.s);
	}
	;

qwordparts
	: qwordparts QWORD
	{
		$$ = $1;

		int l = strlen($$.v);
		int nl = $2.e - $2.s;

		$$.v = realloc($$.v, l + nl + 1);
		memcpy($$.v + l, $2.s, nl);
		$$.v[l + nl] = 0;

		$$.e = (char*)$2.e;
	}
	| QWORD
	{
		$$.s = $1.s;
		$$.e = $1.e;

		$$.v = calloc(1, ($$.e - $$.s) + 1);
		memcpy($$.v, $$.s, $$.e - $$.s);
	}
	;
