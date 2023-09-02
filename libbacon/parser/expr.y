/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
#include <stdio.h>

  #include <stdint.h>
  #include <string.h>

  #include "value.h"
  #include "valyria/pstring.h"

  #include "make.h"
  #include "eval.h"
  #include "glue.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, bacon_parser, expr_yyu)
  #define EXPR_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {expr_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;
  bacon_value *val;
  bacon_expr *expr;
  bacon_operation oper;
}

/* terminals */
%token            STR           "unquoted-string"
%token <yyu.u8>   CREF          "character-escape"
%token <yyu.u8>   HREF          "hex-escape"
%token <yyu.b>    BOOL          "boolean"
%token <yyu.f>    FLOAT         "float"
%token <yyu.t>    VARIABLE      "variable"
%token <yyu.i128> UINTMAX8
%token <yyu.i128> UINTMAX16
%token <yyu.i128> UINTMAX32
%token <yyu.i128> UINTMAX64
%token <yyu.i128> HEX8
%token <yyu.i128> HEX16
%token <yyu.i128> HEX32
%token <yyu.i128> HEX64
%token <yyu.i128> INTMIN8
%token <yyu.i128> INTMIN16
%token <yyu.i128> INTMIN32
%token <yyu.i128> INTMIN64
%token <yyu.i128> INTMAX8
%token <yyu.i128> INTMAX16
%token <yyu.i128> INTMAX32
%token <yyu.i128> INTMAX64
%type <yyu.i128> intval

%token '{' '}' '[' ']' ':'
%token LOGAND  "&&"
%token LOGOR   "||"
%token SHL     "<<"
%token SHR     ">>"

/* nonterminals */
%type <val> unquoted_strpart strpart strparts quoted_string unquoted_string
%type <val> value
%type <val> aggregate
%type <val> list items
%type <val> set elements
%type <val> scalar primitive
%type <val> mapping boolean float integer string variable
%type <oper> unary_operator multiplicative_operator multiplicative_associative_operator additive_operator additive_associative_operator shift_operator relational_operator equality_operator
%type <expr> expr logical_or_expr logical_and_expr inclusive_or_expr exclusive_or_expr and_expr
%type <expr> equality_expr relational_expr shift_expr additive_expr multiplicative_expr unary_expr operand

%%
utterance
  : expr
  {
    PARSER->expr = $1;
    if(PARSER->yyu->attrs & YYU_PARTIAL)
    {
      yyu_loc_final(PARSER->yyu, &@1);
      YYACCEPT;
    }
  }
  ;

expr
	: logical_or_expr
	;

logical_or_expr
	: logical_and_expr
	| logical_or_expr LOGOR logical_and_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, BACON_LOGOR, $1, $3);
  }
	;

logical_and_expr
	: inclusive_or_expr
	| logical_and_expr LOGAND inclusive_or_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, BACON_LOGAND, $1, $3);
  }
	;

inclusive_or_expr
	: exclusive_or_expr
	| inclusive_or_expr '|' exclusive_or_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, BACON_BITOR, $1, $3);
  }
	;

exclusive_or_expr
	: and_expr
	| exclusive_or_expr '^' and_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, BACON_BITXOR, $1, $3);
  }
	;

and_expr
	: equality_expr
	| and_expr '&' equality_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, BACON_BITAND, $1, $3);
  }
	;

equality_expr
	: relational_expr
	| equality_expr equality_operator relational_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	;

relational_expr
	: shift_expr
	| relational_expr relational_operator shift_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	;

shift_expr
	: additive_expr
	| shift_expr shift_operator additive_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	;

additive_expr
	: multiplicative_expr
	| additive_expr additive_associative_operator multiplicative_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	| additive_expr additive_operator multiplicative_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	;

multiplicative_expr
	: unary_expr
	| multiplicative_expr multiplicative_associative_operator unary_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	| multiplicative_expr multiplicative_operator unary_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $2, $1, $3);
  }
	;

unary_expr
	: operand
	| unary_operator unary_expr
  {
    bacon_expr_mk(PARSER, &@$, &$$, $1, $2, NULL);
  }
	;

operand
	: '(' expr ')'
	{
		$$ = $2;
	}
  | value
  {
    bacon_expr_wrap(PARSER, $1, &$$);
  }
	;

unary_operator
	: '~'
	{
		$$ = BACON_BITINV;
	}
	| '!'
	{
		$$ = BACON_LOGINV;
	}
	;

multiplicative_operator
	: '/'
	{
		$$ = BACON_DIV;
	}
	| '%'
	{
		$$ = BACON_MOD;
	}
	;

multiplicative_associative_operator
	: '*'
	{
		$$ = BACON_MUL;
	}
	;

additive_operator
	: '-'
	{
		$$ = BACON_SUB;
	}
	;

additive_associative_operator
	: '+'
	{
		$$ = BACON_ADD;
	}
	;

shift_operator
	: SHL
	{
		$$ = BACON_SHL;
	}
	| SHR
	{
		$$ = BACON_SHR;
	}
	;

relational_operator
	: '>' '='
	{
		$$ = BACON_GE;
	}
	| '>'
	{
		$$ = BACON_GT;
	}
	| '<' '='
	{
		$$ = BACON_LE;
	}
	| '<'
	{
		$$ = BACON_LT;
	}
	;

equality_operator
	: '=' '='
	{
		$$ = BACON_EQ;
	}
	| '!' '='
	{
		$$ = BACON_NE;
	}
	;

value
  : scalar
  | aggregate
  ;

scalar
  : primitive
  | mapping
  ;

aggregate
  : list
  | set
  ;

primitive
  : boolean
  | float
  | integer
  | string
  | variable
  ;

mapping
  : primitive ':' value
  {
    bacon_mapping_mk(PARSER, &@$, &$$, $1, $3);
  }
  ;

list
  : '[' items ']'
  {
    $$ = $2;
  }
  | '[' ']'
  {
    bacon_list_mkv(PARSER, &@$, 0, &$$, 0);
  }
  ;

items
  : items value
  {
    bacon_list_mkv(PARSER, &@$, $1, &$$, $2);
  }
  | value
  {
    bacon_list_mkv(PARSER, &@$, 0, &$$, $1);
  }
  ;

set
  : '{' elements '}'
  {
    $$ = $2;
  }
  | '{' '}'
  {
    bacon_set_mkv(PARSER, &@$, 0, &$$, 0);
  }
  ;

elements
  : elements value
  {
    bacon_set_mkv(PARSER, &@$, $1, &$$, $2);
  }
  | value
  {
    bacon_set_mkv(PARSER, &@$, 0, &$$, $1);
  }
  ;

string
  : unquoted_string
  | quoted_string
  ;

unquoted_string
  : unquoted_strpart
  ;

quoted_string
  : '"' strparts '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$ = 0;
  }
  ;

strparts
  : strparts strpart
  {
    bacon_string_mkv(PARSER, &@$, $1, &$$, $2);
  }
  | strpart
  ;

strpart
  : unquoted_strpart
  | CREF
  {
    bacon_string_mkc(PARSER, &@$, 0, &$$, $1);
  }
  | HREF
  {
    bacon_string_mkc(PARSER, &@$, 0, &$$, $1);
  }
  ;

unquoted_strpart
  : STR
  {
    bacon_string_mkw(PARSER, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

boolean
  : BOOL
  {
    bacon_boolean_mk(PARSER, &@$, &$$, $1);
  }
  ;

integer
  : intval
  {
    bacon_integer_mk(PARSER, &@$, &$$, $1);
  }
  ;

intval
  : INTMIN8
  | INTMIN16
  | INTMIN32
  | INTMIN64
  | INTMAX8
  | INTMAX16
  | INTMAX32
  | INTMAX64
  | UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  | HEX8
  | HEX16
  | HEX32
  | HEX64
  ;

float
  : FLOAT
  {
    bacon_float_mk(PARSER, &@$, &$$, $1);
  }
  ;

variable
  : VARIABLE
  {
    bacon_variable_mkw(PARSER, &@$, &$$, $1.s, $1.l);
  }
  ;
