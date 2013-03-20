%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.parse.h"
	#include "ffn.h"

	static int __attribute__((weakref, alias("ff_yylex"))) ff_dsc_yylex(); 
	static int __attribute__((weakref, alias("ff_yyerror"))) ff_dsc_yyerror(); 
}

%define api.pure
%error-verbose
%name-prefix="ff_dsc_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }
%expect 3

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
%token <num> ':'								/* dependency */
%token <num> '*'								/* weak reference */
%token <num> '['								/* list start */
%token <num> ']'								/* list end */
%token <num> ','								/* list element separator */
%token <num> '~'  							/* list transformation via listwise */

/* nonterminals */
%type  <wordparts> gwordparts
%type  <wordparts> qwordparts

%type  <node> statement
%type  <node> statements
%type  <node> dependency
%type  <node> list
%type  <node> listpartsnone
%type  <node> listpartscomma
%type  <node> listpart
%type  <node> word
%type  <node> varref
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
	|
	{
		parm->ffn = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_STMTLIST, (void*)0, (void*)0, (void*)0);
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
	: dependency
	;

dependency
	: list ':' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $3->e, $1, $3, FFN_SINGLE);
	}
	| list ':' '*' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, $1, $4, FFN_SINGLE | FFN_WEAK);
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
	| list
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
