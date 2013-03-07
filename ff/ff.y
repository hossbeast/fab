%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.h"
	#include "xstring.h"

	int ff_yylex(void* yylvalp, void* yylloc, void* scanner);
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

	ff_node*			node;
}

/* terminals with a semantic value */
%token <str> WORD		"WORD"			/* string token */
%token <str> GWORD	"GWORD"			/* whitespace-delimited generator-string token */
%token <str> QWORD	"QWORD"			/* double-quote delimited string token */
%token <str> WS			"WS"				/* single tab/space */
%token <str> LF			"LF"				/* single newline */
%token <num> '$'								/* variable reference */
%token <num> '@'								/* nofile reference */
%token <num> '.'								/* nofile scoping */
%token <num> ':'								/* dependency */
%token <num> '*'								/* weak reference */
%token <num> '%'								/* dependency discovery */
%token <num> '['								/* list start */
%token <num> ']'								/* list end */
%token <num> ','								/* list element separator */
%token <num> '~'  							/* listwise transformation */
%token <num> '{'								/* formula start */
%token <num> '}'								/* formula end */
%token <num> '='								/* variable assignment */
%token <num> '<'								/* variable push */
%token <num> '>'								/* variable pop */
%token <num> '^'								/* variable designation */
%token <num> '"'								/* generator-string literal */
%token <num> '+'								/* invocation */
%token <num> '-'								/* invocation gate */
%token <num> '('								/* invocation context */
%token <num> ')'								/* invocation context */

/* nonterminals */
%type  <wordparts> gwordparts
%type  <wordparts> qwordparts
 
%type  <node> statements
%type  <node> statement
%type  <node> invocation
%type  <node> varassign
%type  <node> varpush
%type  <node> varpop
%type  <node> vardesignate
%type  <node> vardesignates
%type  <node> command
%type  <node> commands
%type  <node> dependency
%type  <node> discovery
%type  <node> fabrication
%type  <node> list
%type  <node> listpartsnone
%type  <node> listpartscomma
%type  <node> listpart
%type  <node> word
%type  <node> nofile
%type  <node> nofileparts
%type  <node> generator
%type  <node> varref
%type  <node> varrefs
%type  <node> varvalue

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
	: varassign
	| varpush
	| varpop
	| invocation
	| dependency
	| fabrication
	| discovery
	;

varassign
	: varrefs '='
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $2.e, $1, (void*)0);
	}
	| varrefs '=' varvalue
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $3->e, $1, $3);
	}
	;

varpush
	: varrefs '<'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARPUSH, $1->s, $2.e, $1, (void*)0);
	}
	| varrefs '<' varvalue
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARPUSH, $1->s, $3->e, $1, $3);
	}
	;

varpop
	: varrefs '>'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARPOP, $1->s, $2.e, $1, (void*)0);
	}
	| varrefs '>' varvalue
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARPOP, $1->s, $3->e, $1, $3);
	}
	;

vardesignate
	: '^' varrefs
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARDESIGNATE, $1.s, $2->e, $2, (void*)0);
	}
	| varvalue '^' varrefs
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_VARDESIGNATE, $1->s, $3->e, $3, $1);
	}
	;

varvalue
	: list
	| varref
	;

invocation
	: '+' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $2->e, $2, (void*)0, (void*)0, 0);
	}
	| '+' list '-'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3.e, $2, (void*)0, (void*)0, FFN_GATED);
	}
	| '+' list '(' ')'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $4.e, $2, (void*)0, (void*)0, 0);
	}
	| '+' list '(' ')' '-'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $5.e, $2, (void*)0, (void*)0, FFN_GATED);
	}
	| '+' list '(' nofile ')'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $5.e, $2, $4, (void*)0, 0);
	}
	| '+' list '(' nofile ')' '-'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $6.e, $2, $4, (void*)0, FFN_GATED);
	}
	| '+' list '(' vardesignates ')'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $5.e, $2, (void*)0, $4, 0);
	}
	| '+' list '(' vardesignates ')' '-'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $6.e, $2, (void*)0, $4, FFN_GATED);
	}
	| '+' list '(' nofile '|' vardesignates ')'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $7.e, $2, $4, $6, 0);
	}
	| '+' list '(' nofile '|' vardesignates ')' '-'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $8.e, $2, $4, $6, FFN_GATED);
	}
	;

vardesignates
	: vardesignates ',' vardesignates
	{
		$$ = ffn_addchain($1, $3);
	}
	| vardesignate
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
	| list ':' ':' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, $1, $4, FFN_MULTI);
	}
	| list ':' ':' '*' list
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $5->e, $1, $5, FFN_MULTI | FFN_WEAK);
	}
	;

fabrication
	: dependency '{' commands '}'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $4.e, (void*)0, $1->needs, $3, $1->flags | FFN_FABRICATION);
		$$ = ffn_addchain($1, $$);
	}
	| list '{' commands '}'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $4.e, $1, (void*)0, $3, FFN_SINGLE | FFN_FABRICATION);
	}
	| list ':' '{' commands '}'
	{
		/* this form is redundant but is included for completeness */
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $5.e, $1, (void*)0, $4, FFN_SINGLE | FFN_FABRICATION);
	}
	| list ':' ':' '{' commands '}'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $6.e, $1, (void*)0, $5, FFN_MULTI | FFN_FABRICATION);
	}
	;

discovery
	: list '%' '{' commands '}'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $5.e, $1, (void*)0, $4, FFN_SINGLE | FFN_DISCOVERY);
	}
	;

commands
	: commands commands
	{
		$$ = ffn_addchain($1, $2);
	}
	| command
	;

command
	: LF
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LF, $1.s, $1.e, $1.s, $1.e);
	}
	| list
	| word
	;

list
	: '[' listpartsnone ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0, FFN_WSSEP);
	}
	| '[' listpartscomma ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0, FFN_COMMASEP);
	}
	| '[' listpart ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0, FFN_WSSEP);
	}
	| '[' ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $2.e, (void*)0, (void*)0, FFN_WSSEP);
	}
	| '[' listpartsnone '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2, $4, FFN_WSSEP);
	}
	| '[' listpartscomma '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2, $4, FFN_COMMASEP);
	}
	| '[' listpart '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5.e, $2, $4, FFN_WSSEP);
	}
	| '[' '~' generator ']'
	{
		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $4.e, (void*)0, $3, FFN_WSSEP);
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

		char * v = 0;
		xstrcatf(&v, "/..");

		char * e = $2->e;

		ff_node * n = $2;
		while(n)
		{
			xstrcatf(&v, "/%s", n->text);

			ff_node * nn = n->next;
			n->next = 0;
			ffn_free(n);
			n = nn;
		}

		$$ = ffn_mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, e, v);
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
