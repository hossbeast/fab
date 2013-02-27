%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.h"

	static int __attribute__((weakref, alias("ff_yylex"))) ff_dsc_yylex(); 
	static int __attribute__((weakref, alias("ff_yyerror"))) ff_dsc_yyerror(); 
}

%define api.pure
%error-verbose
%name-prefix="ff_dsc_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }

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
%token <str> WORD							"WORD"
%token <str> WS								"WS"				/* a single tab/space */
%token <num> LW								"=>>"
%token <num> ':'
%token <num> '*'
%token <num> '['
%token <num> ']'
%token <num> '"'

/* nonterminals */
%type  <wordparts> wordparts

%type  <node> statement
%type  <node> statements
%type  <node> dependency
%type  <node> list
%type  <node> listparts
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
		parm->ff->ffn = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_STMTLIST, $1->s, $1->e, $1);
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
	: '[' listparts ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0);
	}
	| '[' ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $2.e, (void*)0, (void*)0);
	}
	| '[' listparts ']' LW generator
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5->e, $2, $5);
	}
	| '[' ']' LW generator
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $4->e, (void*)0, $4);
	}
	;

listparts
	: listparts listparts
	{
		$$ = ffn_addchain($1, $2);
	}
	| varref
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
	: '"' wordparts '"'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WS wordparts WS
	{
		@$ = @2;	/* exclude the enclosing WS for word location */
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WORD
	{
		char* v = calloc(1, ($1.e - $1.s) + 1);
		memcpy(v, $1.s, $1.e - $1.s);

		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, v);
	}
	;

wordparts
	: wordparts WORD
	{
		$$ = $1;

		int l = strlen($$.v);
		int nl = $2.e - $2.s;

		$$.v = realloc($$.v, l + nl + 1);
		memcpy($$.v + l, $2.s, nl);
		$$.v[l + nl] = 0;

		$$.e = (char*)$2.e;
	}
	| WORD
	{
		$$.s = $1.s;
		$$.e = $1.e;

		$$.v = calloc(1, ($$.e - $$.s) + 1);
		memcpy($$.v, $$.s, $$.e - $$.s);
	}
	;
