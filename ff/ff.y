%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.h"

	int  ff_yylex(void* yylvalp, void* yylloc, void* scanner);
	ff_node* mknode(void* loc, char* ffdir, uint32_t type, ...);

	ff_node* addchain(ff_node* a, ff_node* b)
	{
		ff_node* i = a;
		while(a->next)
			a = a->next;

		a->next = b;
		return i;
	}
}

%define api.pure
%error-verbose
%name-prefix="ff_yy"
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
%token <str> LF								"LF"				/* a single newline */
%token <var> VARNAME					"VARNAME"
%token <num> INC							">>"
%token <num> LW								"=>>"
%token <num> ':'
%token <num> '['
%token <num> ']'
%token <num> '{'
%token <num> '}'
%token <num> '='
%token <num> '"'

/* nonterminals */
%type  <wordparts> wordparts

%type  <node> statement
%type  <node> statement_list
%type  <node> include
%type  <node> vardecl
%type  <node> formula
%type  <node> dependency
%type  <node> list
%type  <node> listpiece
%type  <node> formula_list
%type  <node> word
%type  <node> generator

/* sugar */
%token END 0 "end of file"

%destructor { ff_freenode($$); } <node>

%%

ff
	: statement_list
	{
		*parm->ffn = mknode(&@$, parm->ff_dir, FFN_STMTLIST, $1->s, $1->e, $1);
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
	| formula
	| include
	| vardecl
	;

dependency
	: list ':' list
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_DEPENDENCY, $1->s, $3->e, $1, $3);
	}
	| list ':' ':' list
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_DEPENDENCY, $1->s, $4->e, $1, $4);
	}
	;

formula
	: list '{' formula_list '}'
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_FORMULA, $1->s, $4.e, $1, $3);
	}
	;

include
	: INC list
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_INCLUDE, $1.s, $2->e, $1, $2);
	}
	;

vardecl
	: WORD '=' list
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_VARDECL, $1.s, $3->e, $1.s, $1.e, $3);
	}
	;

formula_list
	: formula_list formula_list
	{
		$$ = addchain($1, $2);
	}
	| LF
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_LF, $1.s, $1.e, $1.s, $1.e);
	}
	| list
	| word
	;

list
	: '[' listpiece ']'
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_LIST, $1.s, $3.e, $2, (void*)0);
	}
	| '[' listpiece ']' LW generator
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_LIST, $1.s, $5->e, $2, $5);
	}
	;

listpiece
	: listpiece listpiece
	{
		$$ = addchain($1, $2);
	}
	| VARNAME
	{
		$$ = mknode(&@$, parm->ff_dir, FFN_VARNAME, $1.s, $1.e, $1.vs, $1.ve);
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
		$$ = mknode(&@$, parm->ff_dir, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WS wordparts WS
	{
		@$ = @2;	/* exclude the enclosing WS for word location */
		$$ = mknode(&@$, parm->ff_dir, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WORD
	{
		char* v = calloc(1, ($1.e - $1.s) + 1);
		memcpy(v, $1.s, $1.e - $1.s);

		$$ = mknode(&@$, parm->ff_dir, FFN_WORD, $1.s, $1.e, v);
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
