%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.h"

	int  ff_yylex(void* yylvalp, void* yylloc, void* scanner);
	ff_node* mknode(void* loc, size_t locz, ff_file * ff, uint32_t type, ...);

	ff_node* addchain(ff_node* a, ff_node* b);
}

%define api.pure
%error-verbose
%name-prefix="ff_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }
%expect 22

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
%token <str> LF								"LF"				/* a single newline */
%token <num> LW								"=>>"				/* listwise transformation */
%token <num> '$'	/* variable reference */
%token <num> ':'	/* dependency */
%token <num> '*'	/* weak reference */
%token <num> '~'	/* dependency discovery */
%token <num> '['	/* list start */
%token <num> ']'	/* list end */
%token <num> '{'	/* formula start */
%token <num> '}'	/* formula end */
%token <num> '='	/* variable assignment */
%token <num> '+'  /* invocation */
%token <num> '<'	/* variable push, invocation context start */
%token <num> '>'	/* variable pop,  invocation context end */
%token <num> '@'  /* variable contextual push */
%token <num> '"'	/* generator-string literal */

/* nonterminals */
%type  <wordparts> wordparts
%type  <wordparts> nofileparts

%type  <node> statement
%type  <node> statement_list
%type  <node> invocation
%type  <node> specifier
%type  <node> context
%type  <node> designation
%type  <node> designation_list
%type  <node> varassign
%type  <node> varpush
%type  <node> varpop
%type  <node> formula
%type  <node> formula_list
%type  <node> dependency
%type  <node> discovery
%type  <node> fabrication
%type  <node> task
%type  <node> taskdep
%type  <node> taskname
%type  <node> list
%type  <node> listpiece
%type  <node> word
%type  <node> nofile
%type  <node> generator
%type  <node> varref
%type  <node> varreflist
%type  <node> varvalue

/* sugar */
%token END 0 "end of file"

%destructor { ff_freenode($$); } <node>
%destructor { free($$.v); } <wordparts>

%%

ff
	: statement_list
	{
		parm->ff->ffn = mknode(&@$, sizeof(@$), parm->ff, FFN_STMTLIST, $1->s, $1->e, $1);
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
	: invocation
	| varassign
	| varpush
	| varpop
	| dependency
	| task
	| fabrication
	| discovery
	;

invocation
	: '+' specifier
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $2->e, $2, (void*)0);
	}
	| '+' specifier context
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3);
	}
	;

specifier
	: list
	| nofile
	;

nofile
	: nofileparts
	{
		/* nofile is a shorthand for non-file-backed notation */

		char * v = realloc($1.v, strlen($1.v) + 5);
		memcpy(v, "/../", 4);
		memcpy(v + 4, $1.v, strlen($1.v));
		v[4 + strlen($1.v)] = 0;

		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, v);
	}
	;

nofileparts
	: nofileparts ':' ':' WORD
	{
		$$ = $1;

		int l = strlen($$.v);
		int nl = $4.e - $4.s;

		$$.v = realloc($$.v, l + nl + 1 + 1);
		memcpy($$.v + l, "/", 1);
		memcpy($$.v + l + 1, $4.s, nl);
		$$.v[l + nl + 1] = 0;

		$$.e = (char*)$4.e;
	}
	| WORD
	{
		$$.s = $1.s;
		$$.e = $1.e;

		$$.v = calloc(1, ($$.e - $$.s) + 1);
		memcpy($$.v, $$.s, $$.e - $$.s);
	}
	;

context
	: '<' designation_list '>'
	{
		$$ = $2;
		$$->flags = FFN_GATED;
	}
	| designation_list
	;

designation_list
	: designation_list ',' designation_list
	{
		$$ = addchain($1, $3);
	}
	| designation
	;

designation
	: varvalue '@' varreflist
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION_CTX, $1->s, $3->e, $1, $3);
	}
	;

varreflist
	: varreflist varreflist
	{
		$$ = addchain($1, $2);
	}
	| varref
	;

varassign
	: varref '='
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $2.e, $1, (void*)0);
	}
	| varref '=' varvalue
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $3->e, $1, $3);
	}
	;

varpush
	: varref '<'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPUSH, $1->s, $2.e, $1, (void*)0);
	}
	| varref '<' varvalue
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPUSH, $1->s, $3->e, $1, $3);
	}
	;

varpop
	: varref '>'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPOP, $1->s, $2.e, $1, (void*)0);
	}
	| varref '>' varvalue
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPOP, $1->s, $3->e, $1, $3);
	}
	;

varvalue
	: list
	| varref
	;

dependency
	: list ':' list
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $3->e, FFN_SINGLE, $1, $3);
	}
	| list ':' '*' list
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, FFN_SINGLE | FFN_WEAK, $1, $4);
	}
	| list ':' ':' list
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, FFN_MULTI, $1, $4);
	}
	| list ':' ':' '*' list
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $5->e, FFN_MULTI | FFN_WEAK, $1, $5);
	}
	;

task
	: taskdep
	| taskdep formula
	{
		$2->flags = FFN_SINGLE;
		$2->targets = $1->needs;
		$2->nodes_freeguard = 1;
		$$ = addchain($1, $2);
	}
	| taskname formula
	{
		$2->flags = FFN_SINGLE;
		$2->targets = $1;
		$$ = $2;
	}
	;

fabrication
	: dependency formula
	{
		$2->flags = $1->flags;
		$2->targets = $1->needs;
		$2->nodes_freeguard = 1;
		$$ = addchain($1, $2);
	}
	| list ':' formula
	{
		$3->flags = FFN_SINGLE;
		$3->targets = $1;
		$$ = $3;
	}
	| list ':' ':' formula
	{
		$4->flags = FFN_MULTI;
		$4->targets = $1;
		$$ = $4;
	}
	;

discovery
	: list '~' formula
	{
		$3->flags = FFN_SINGLE | FFN_DISCOVERY;
		$3->targets = $1;
		$$ = $3;
	}
	;

taskdep
	: taskname ':' list
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $3->e, FFN_SINGLE, $1, $3);
	}
	;

taskname
	: nofile
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1->s, $1->e, $1, (void*)0);
	}
	;

formula
	: '{' formula_list '}'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1.s, $3.e, $2);
	}
	;

formula_list
	: formula_list formula_list
	{
		$$ = addchain($1, $2);
	}
	| LF
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LF, $1.s, $1.e, $1.s, $1.e);
	}
	| list
	| word
	;

list
	: '[' listpiece ']'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0);
	}
	| '[' listpiece ']' LW generator
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5->e, $2, $5);
	}
	;

listpiece
	: listpiece listpiece
	{
		$$ = addchain($1, $2);
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
		/* only concatenate consecutive WORD's when enclosed in quotes or WS */
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WS wordparts WS
	{
		@$ = @2;	/* exclude the enclosing WS for word location */
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WORD
	{
		char* v = calloc(1, ($1.e - $1.s) + 1);
		memcpy(v, $1.s, $1.e - $1.s);

		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, v);
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
