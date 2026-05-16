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
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;
  bacon_value * val;
}

/* terminals */
%token '{' '}'
%token '[' ']'
%token ':'

%token            STR           300 "unquoted-string"
%token <yyu.u8>   CREF          301 "character-escape"
%token <yyu.u8>   HREF          302 "hex-escape"
%token <yyu.b>    BOOL          303 "boolean"
%token <yyu.f>    FLOAT         304 "float"
%token <yyu.t>    VARIABLE      305 "variable"
%token <yyu.i128> UINTMAX8      306
%token <yyu.i128> UINTMAX16     307
%token <yyu.i128> UINTMAX32     308
%token <yyu.i128> UINTMAX64     309
%token <yyu.i128> HEX8          310
%token <yyu.i128> HEX16         311
%token <yyu.i128> HEX32         312
%token <yyu.i128> HEX64         313
%token <yyu.i128> INTMIN8       314
%token <yyu.i128> INTMIN16      315
%token <yyu.i128> INTMIN32      316
%token <yyu.i128> INTMIN64      317
%token <yyu.i128> INTMAX8       318
%token <yyu.i128> INTMAX16      319
%token <yyu.i128> INTMAX32      320
%token <yyu.i128> INTMAX64      321

%token LOGAND                   322 "&&"
%token LOGOR                    323 "||"
%token SHL                      324 "<<"
%token SHR                      325 ">>"
%token EQ                       326 "=="

/* nonterminals */
%type <val> words quoted_string unquoted_string
%type <val> value
%type <val> aggregate
%type <val> list items
%type <val> set elements
%type <val> scalar primitive
%type <val> mapping boolean float integer string
%type <yyu.i128> intlit

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
