/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

%code requires {
#include <stdio.h>

  #include <stdint.h>
  #include <string.h>

  #include "value.h"
  #include "valyria/list.h"
  #include "valyria/pstring.h"

  #include "make.internal.h"
  #include "parser.internal.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, value_parser, value_set_yyu)
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
  value * val;
}

/* terminals */
%token            STR           "unquoted-string"
%token <yyu.u8>   CREF          "character-reference"
%token <yyu.b>    BOOL          "boolean"
%token <yyu.f>    FLOAT         "float"
%token <yyu.t>    VARIABLE      "variable"
%token <yyu.umax> UINTMAX8
%token <yyu.umax> UINTMAX16
%token <yyu.umax> UINTMAX32
%token <yyu.umax> UINTMAX64
%token <yyu.umax> HEX8
%token <yyu.umax> HEX16
%token <yyu.umax> HEX32
%token <yyu.umax> HEX64
%token <yyu.imax> INTMIN8
%token <yyu.imax> INTMIN16
%token <yyu.imax> INTMIN32
%token <yyu.imax> INTMIN64
%token <yyu.imax> INTMAX8
%token <yyu.imax> INTMAX16
%token <yyu.imax> INTMAX32
%token <yyu.imax> INTMAX64

%token '{' '}' '[' ']'

%type <yyu.umax> posint_ugroup
%type <yyu.imax> posint_igroup
%type <yyu.imax> negint_igroup

/* nonterminals */
%type <val> unquoted_strpart strpart strparts quoted_string unquoted_string
%type <val> value
%type <val> aggregate
%type <val> list items
%type <val> set elements
%type <val> scalar primitive
%type <val> mapping boolean float posint negint string variable

%%
utterance
  : elements
  {
    PARSER->root = $1;

    if(PARSER->value_set_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->value_set_yyu, &@1);
      YYACCEPT;
    }
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
  | negint
  | posint
  | string
  | variable
  ;

mapping
  : primitive ':' value
  {
    YFATAL(value_mapping_mk, PARSER, &@$, &$$, $1, $3, VALUE_MERGE_ADD);
  }
  | primitive '=' value
  {
    YFATAL(value_mapping_mk, PARSER, &@$, &$$, $1, $3, VALUE_MERGE_SET);
  }
  ;

list
  : '[' items ']'
  {
    $$ = $2;
  }
  | '[' ']'
  {
    YFATAL(value_list_mkv, PARSER, &@$, 0, &$$, 0);
  }
  ;

items
  : items value
  {
    YFATAL(value_list_mkv, PARSER, &@$, $1, &$$, $2);
  }
  | value
  {
    YFATAL(value_list_mkv, PARSER, &@$, 0, &$$, $1);
  }
  ;

set
  : '{' elements '}'
  {
    $$ = $2;
  }
  | '{' '}'
  {
    YFATAL(value_set_mkv, PARSER, &@$, 0, &$$, 0);
  }
  ;

elements
  : elements value
  {
    YFATAL(value_set_mkv, PARSER, &@$, $1, &$$, $2);
  }
  | value
  {
    YFATAL(value_set_mkv, PARSER, &@$, 0, &$$, $1);
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
    YFATAL(value_string_mkv, PARSER, &@$, $1, &$$, $2);
  }
  | strpart
  ;

strpart
  : unquoted_strpart
  | CREF
  {
    YFATAL(value_string_mkc, PARSER, &@$, 0, &$$, $1);
  }
  ;

unquoted_strpart
  : STR
  {
    YFATAL(value_string_mkw, PARSER, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

boolean
  : BOOL
  {
    YFATAL(value_boolean_mk, PARSER, &@$, &$$, $1);
  }
  ;

posint
  : posint_ugroup
  {
    YFATAL(value_posint_mk, PARSER, &@$, &$$, $1);
  }
  | posint_igroup
  {
    YFATAL(value_posint_mk, PARSER, &@$, &$$, $1);
  }
  ;

posint_ugroup
  : UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  | HEX8
  | HEX16
  | HEX32
  | HEX64
  ;

posint_igroup
  : INTMAX8
  | INTMAX16
  | INTMAX32
  | INTMAX64
  ;

negint
  : negint_igroup
  {
    YFATAL(value_negint_mk, PARSER, &@$, &$$, $1);
  }
  ;

negint_igroup
  : INTMIN8
  | INTMIN16
  | INTMIN32
  | INTMIN64
  ;

float
  : FLOAT
  {
    YFATAL(value_float_mk, PARSER, &@$, &$$, $1);
  }
  ;

variable
  : VARIABLE
  {
    YFATAL(value_variable_mkw, PARSER, &@$, &$$, $1.s, $1.l);
  }
  ;
