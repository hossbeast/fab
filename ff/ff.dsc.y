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

	ff_node* mknode(void* loc, size_t locz, ff_file * ff, uint32_t type, ...);
	ff_node* addchain(ff_node* a, ff_node* b);
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

	struct
	{
		const char*			s;
		const char*			e;

		const char*			vs;
		const char*			ve;
	}							var;

	ff_node*			node;
}

/* terminals with a semantic value */
%token <str> WORD							"WORD"
%token <str> WS								"WS"				/* a single tab/space */
%token <var> VARNAME					"VARNAME"
%token <num> LW								"=>>"
%token <num> ':'
%token <num> '*'
%token <num> '['
%token <num> ']'
%token <num> '"'

/* nonterminals */
%type  <wordparts> wordparts

%type  <node> statement
%type  <node> statement_list
%type  <node> dependency
%type  <node> list
%type  <node> listpiece
%type  <node> word
%type  <node> generator


/* sugar */
%token END 0 "end of file"

%destructor { ff_freenode($$); } <node>

%%

ff
	: statement_list
	{
		*parm->ffn = mknode(&@$, sizeof(&@$), parm->ff, FFN_STMTLIST, $1->s, $1->e, $1);
	}
	;

statement_list
	: statement_list statement
	{
		$$ = addchain($1, $2);
	}
	| statement
	;

statement
	: dependency
	;

dependency
	: list ':' list
	{
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_DEPENDENCY, $1->s, $3->e, FFN_SINGLE, $1, $3);
	}
	| list ':' '*' list
	{
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, FFN_SINGLE | FFN_WEAK, $1, $4);
	}
	;

list
	: '[' listpiece ']'
	{
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0);
	}
	| '[' listpiece ']' LW generator
	{
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_LIST, $1.s, $5->e, $2, $5);
	}
	;

listpiece
	: listpiece listpiece
	{
		$$ = addchain($1, $2);
	}
	| VARNAME
	{
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_VARNAME, $1.s, $1.e, $1.vs, $1.ve);
	}
	| word
	| list
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
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WS wordparts WS
	{
		@$ = @2;	/* exclude the enclosing WS for word location */
		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WORD
	{
		char* v = calloc(1, ($1.e - $1.s) + 1);
		memcpy(v, $1.s, $1.e - $1.s);

		$$ = mknode(&@$, sizeof(&@$), parm->ff, FFN_WORD, $1.s, $1.e, v);
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
