%code requires {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
}

%code top {
	#include "bf.h"

	int  bf_yylex(void* yylvalp, void* yylloc, void* scanner);
	bf_node* mknode(void* loc, char* bfdir, uint32_t type, ...);

	bf_node* addchain(bf_node* a, bf_node* b)
	{
		bf_node* i = a;
		while(a->next)
			a = a->next;

		a->next = b;
		return i;
	}
}

%define api.pure
%error-verbose
%name-prefix="bf_yy"
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
		uint32_t				v;
	}							num;

	bf_node*			node;
}

/* terminals with a semantic value */
%token <str> FNAME
%token <str> TEXT
%token <num> BF_REF_PATH
%token <num> BF_REF_EDEPSO
%token <num> BF_REF_IDEPSC
%token <num> LF								"LF"
%token <num> ':'
%token <num> '{'
%token <num> '}'

/* nonterminals */
%type  <node> statement
%type  <node> statement_list
%type  <node> file_list
%type  <node> file
%type  <node> recipe
%type  <node> command
%type  <node> command_seg
%type  <node> dependency
%type  <num>  reference

/* sugar */
%token END 0 "end of file"

%destructor { bf_freenode($$); } <node>

%%

bf
	: statement_list tail
	{
		*parm->bfn = $1;
	}
	;

tail
	: LF_list
	|
	;

statement_list
	: statement_list LF_list statement
	{
		$$ = addchain($1, $3);
	}
	| statement
	;

statement
	: dependency
	| recipe
	;

dependency
	: file_list ':' file_list
	{
		$$ = mknode(&@$, parm->bf_dir, BFN_DEPENDENCY, $1->s, $3->e, $1, $3);
	}
	;

file_list
	: file_list file
	{
		$$ = addchain($1, $2);
	}
	| file
	;

file
	: FNAME
	{
		$$ = mknode(&@$, parm->bf_dir, BFN_FNAME, $1.s, $1.e, $1.s, $1.e);
	}
	;

recipe
	: file_list '{' command '}'
	{
		$$ = mknode(&@$, parm->bf_dir, BFN_FORMULA, $1->s, $4.e, $1, $3);
	}
	;

command
	: command command_seg
	{
		$$ = addchain($1, $2);
	}
	| command_seg
	;

command_seg
	: TEXT
	{
		$$ = mknode(&@$, parm->bf_dir, BFN_COMMAND_TEXT, $1.s, $1.e, $1.s, $1.e);
	}
	| LF
	{
		$$ = mknode(&@$, parm->bf_dir, BFN_COMMAND_TEXT, $1.s, $1.e, $1.s, $1.e);
	}
	| reference
	{
		$$ = mknode(&@$, parm->bf_dir, BFN_COMMAND_REF, $1.s, $1.e, $1.v);
	}
	;

reference
	: BF_REF_PATH
	| BF_REF_EDEPSO
	| BF_REF_IDEPSC
	;

LF_list
	: LF_list LF
	| LF
	;
