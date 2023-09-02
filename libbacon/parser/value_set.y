/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
#include <stdio.h>

  #include <stdint.h>
  #include <string.h>

  #include "value.h"
  #include "valyria/pstring.h"

  #include "make.h"
  #include "glue.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, bacon_parser, value_set_yyu)
  #define VALUE_SET_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {value_set_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;
  bacon_value * val;
}



/* terminals */
%token            STR           "unquoted-string"
%token <yyu.u8>   CREF          "character-reference"
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
%type <yyu.i128> intlit

%token '{' '}' '[' ']'
%token LOGAND  "&&"
%token LOGOR   "||"
%token SHL     "<<"
%token SHR     ">>"

/* nonterminals */
%type <val> words quoted_string unquoted_string
%type <val> value
%type <val> aggregate
%type <val> list items
%type <val> set elements
%type <val> scalar primitive
%type <val> mapping boolean float integer string

%destructor { bacon_value_free(PARSER, $$); } <val>

%%
utterance
  : elements
  {
    PARSER->value = $1;

    if(PARSER->value_set_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->value_set_yyu, &@1);
      YYACCEPT;
    }
  }
  | %empty
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
  : word-token
  {
    bacon_string_mkw(PARSER, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

quoted_string
  : '"' words '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$ = 0;
  }
  ;

words
  : words word-token
  {
    bacon_string_mkw(PARSER, &@$, $1, &$$, @2.s, @2.l);
  }
  | word-token
  {
    bacon_string_mkw(PARSER, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

word-token
  : STR
  | CREF
  | HREF
  | '/'
  | '-'
  | '+'
  | '='
  | ','
  ;

boolean
  : BOOL
  {
    bacon_boolean_mk(PARSER, &@$, &$$, $1);
  }
  ;

integer
  : intlit
  {
    bacon_integer_mk(PARSER, &@$, &$$, $1);
  }
  ;

intlit
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
