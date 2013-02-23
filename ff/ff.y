%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
}

%code top {
	#include "ff.h"
	#include "xstring.h"

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
%expect 29

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
%token <num> '.'	/* scope resolution */
%token <num> '*'	/* weak reference */
%token <num> '~'	/* dependency discovery */
%token <num> '['	/* list start */
%token <num> ']'	/* list end */
%token <num> '{'	/* formula start */
%token <num> '}'	/* formula end */
%token <num> '='	/* variable assignment */
%token <num> '+'  /* invocation */
%token <num> '<'	/* variable push, gated invocation context start */
%token <num> '>'	/* variable pop,  gated invocation context end */
%token <num> '@'  /* variable contextual push */
%token <num> '"'	/* generator-string literal */

/* nonterminals */
%type  <wordparts> wordparts
 
%type  <node> statements
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
%type  <node> listparts
%type  <node> listornofile
%type  <node> word
%type  <node> scope
%type  <node> nofile
%type  <node> nofiles
%type  <node> nofilelist
%type  <node> nofileparts
%type  <node> generator
%type  <node> varref
%type  <node> varrefs
%type  <node> varvalue

/* sugar */
%token END 0 "end of file"

%destructor { ff_freenode($$); } <node>
%destructor { free($$.v); } <wordparts>

%%

ff
	: statements
	{
		parm->ff->ffn = mknode(&@$, sizeof(@$), parm->ff, FFN_STMTLIST, $1->s, $1->e, $1);
	}
	;

statements
	: statements statements
	{
		$$ = addchain($1, $2);
	}
	| varassign
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
printf("varassign 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $2.e, $1, (void*)0);
	}
	| varrefs '=' varvalue
	{
printf("varassign 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARASSIGN, $1->s, $3->e, $1, $3);
	}
	;

varpush
	: varrefs '<'
	{
printf("varpush 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPUSH, $1->s, $2.e, $1, (void*)0);
	}
	| varrefs '<' varvalue
	{
printf("varpush 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPUSH, $1->s, $3->e, $1, $3);
	}
	;

varpop
	: varrefs '>'
	{
printf("varpop 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPOP, $1->s, $2.e, $1, (void*)0);
	}
	| varrefs '>' varvalue
	{
printf("varpop 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARPOP, $1->s, $3->e, $1, $3);
	}
	;

vardesignate
	: '@' varrefs
	{
printf("vardesignate 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARDESIGNATE, $1.s, $2->e, (void*)0, $2);
	}
	| varvalue '@' varrefs
	{
printf("vardesignate 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_VARDESIGNATE, $1->s, $3->e, $3, $1);
	}
	;

varvalue
	: list
	{
$$ = $1;
printf("varvalue 1\n");
	}
	| varref
	{
$$ = $1;
printf("varvalue 2\n");
	}
	;

invocation
	: '+' listornofile
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $2->e, $2, (void*)0);
	}
	| '+' listornofile '<' '>'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	| '+' listornofile '<' scope '>'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	| '+' listornofile '<' vardesignates '>'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	| '+' listornofile '<' scope '|' vardesignates '>'
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	| '+' listornofile scope
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	| '+' listornofile vardesignates
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	| '+' listornofile scope '|' vardesignates
	{
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_INVOCATION, $1.s, $3->e, $2, $3, FFN_GATED);
	}
	;

scope
	: nofile
	;

vardesignates
	: vardesignates ',' vardesignates
	{
		$$ = addchain($1, $3);
	}
	| vardesignate
	;

dependency
	: listornofile ':' listornofile
	{
printf("dependency 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $3->e, $1, $3, FFN_SINGLE);
	}
	| list ':' '*' listornofile
	{
printf("dependency 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, $1, $4, FFN_SINGLE);
	}
	| listornofile ':' ':' listornofile
	{
printf("dependency 3\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $4->e, $1, $4, FFN_MULTI);
	}
	| list ':' ':' '*' listornofile
	{
printf("dependency 4\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_DEPENDENCY, $1->s, $5->e, $1, $5, FFN_MULTI | FFN_WEAK);
	}
	;

fabrication
	: dependency '{' commands '}'
	{
printf("fabrication 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $4.e, (void*)0, $1->needs, $3, $1->flags | FFN_FABRICATION);
		$$ = addchain($1, $$);
	}
	| listornofile '{' commands '}'
	{
printf("fabrication 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $4.e, $1, (void*)0, $3, FFN_SINGLE | FFN_FABRICATION);
	}
	| listornofile ':' '{' commands '}'
	{
		/* this form is redundant but is included for completeness */
printf("fabrication 3\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $5.e, $1, (void*)0, $4, FFN_SINGLE | FFN_FABRICATION);
	}
	| list ':' ':' '{' commands '}'
	{
printf("fabrication 4\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $6.e, $1, (void*)0, $5, FFN_MULTI | FFN_FABRICATION);
	}
	;

discovery
	: list '~' '{' commands '}'
	{
printf("discovery 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_FORMULA, $1->s, $5.e, $1, $4, FFN_SINGLE | FFN_DISCOVERY);
	}
	;

commands
	: commands commands
	{
printf("commands 1\n");
		$$ = addchain($1, $2);
	}
	| command
	{
printf("commands 2\n");
$$ = $1;
	}
	;

command
	: LF
	{
printf("command 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LF, $1.s, $1.e, $1.s, $1.e);
	}
	| list
	{
printf("command 2\n");
$$ = $1;
	}
	| word
	{
printf("command 3\n");
$$ = $1;
	}
	;

listornofile
	: list
	{
$$ = $1;
printf("listornofile 1\n");
	}
	| nofile
	{
printf("listornofile 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1->s, $1->e, $1, (void*)0);
	}
	;

list
	: '[' listparts ']'
	{
printf("list 1\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $3.e, $2, (void*)0);
	}
	| '[' listparts ']' LW generator
	{
printf("list 2\n");
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_LIST, $1.s, $5->e, $2, $5);
	}
	;

listparts
	: listparts listparts
	{
printf("listparts 1\n");
		$$ = addchain($1, $2);
	}
	| varref
	{
$$ = $1;
printf("listparts 2\n");
	}
	| word
	{
$$ = $1;
printf("listparts 3\n");
	}
	| list
	{
$$ = $1;
printf("listparts 4\n");
	}
	;

varrefs
	: varrefs varrefs
	{
printf("varrefs 1\n");
		$$ = addchain($1, $2);
	}
	| varref
	{
$$ = $1;
printf("varrefs 2\n");
	}
	;

varref
	: '$' word
	{
printf("varref 1\n");
		$$ = $2;
		$$->type = FFN_VARREF;
	}
	;

generator
	: word
	{
printf("generator 1\n");
		$$ = $1;
		$$->type = FFN_GENERATOR;
	}
	;

nofilelist
	: '(' nofiles ')'
	{
		$$ = $2;
	}
	| nofile
	;

nofiles
	: nofiles nofiles
	{
		$$ = addchain($1, $2);
	}
	| nofile
	;

nofile
	: nofileparts
	{
printf("nofile 1\n");
		/* nofile is a shorthand for non-file-backed notation */

		char * v = 0;
		xstrcatf(&v, "/..");

		ff_node * n = $$;
		while(n)
		{
			xstrcatf(&v, "/%s", n->text);

			ff_node * nn = n->next;
			n->next = 0;
			ff_freenode(n);
			n = nn;
		}

		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1->s, $1->e, v);
	}
	;

nofileparts
	: nofileparts '.' nofileparts
	{
printf("nofileparts 1\n");
		$$ = addchain($1, $3);
	}
	| word
	{
$$ = $1;
printf("nofileparts 2\n");
	}
	;

word
	: '"' wordparts '"'
	{
printf("word 1\n");
		/* only concatenate consecutive WORD's when enclosed in quotes or WS */
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WS wordparts WS
	{
printf("word 2\n");
		@$ = @2;	/* exclude the enclosing WS for word location */
		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $3.e, $2.v);
	}
	| WORD
	{
printf("word 3\n");
		char* v = calloc(1, ($1.e - $1.s) + 1);
		memcpy(v, $1.s, $1.e - $1.s);

		$$ = mknode(&@$, sizeof(@$), parm->ff, FFN_WORD, $1.s, $1.e, v);
	}
	;

wordparts
	: wordparts WORD
	{
printf("wordparts 1\n");
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
printf("wordparts 2\n");
		$$.s = $1.s;
		$$.e = $1.e;

		$$.v = calloc(1, ($$.e - $$.s) + 1);
		memcpy($$.v, $$.s, $$.e - $$.s);
	}
	;
