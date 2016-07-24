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

%code top {
  #include <stdint.h>
  #include <string.h>

  #include "config_parser.internal.h"

  struct value;

  int config_yylex(void *, void*, void*);
}

%define api.pure
%error-verbose
%locations
%name-prefix "config_yy"
%parse-param { void* scanner }
%parse-param { config_xtra * xtra }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
  uint8_t     u8;
  int64_t     i64;
  long double fp;

  struct value * val;
}

/* tokens */
%token ':'
%token ','
%token '{'
%token '}'

/* terminals */
%token        STR
%token <u8>   BOOL
%token <i64>  INT
%token <i64>  HEX
%token <fp>   FLOAT
%token <u8>   CREF HREF

/* nonterminals */
%type <val> string strpart strparts quoted_string unquoted_string
%type <val> array list
%type <val> object map
%type <val> boolean
%type <val> integer
%type <val> float
%type <val> value

%%
utterance
  : map
  {
    xtra->root = $1;
  }
  ;

value
  : array
  | object
  | string
  | boolean
  | integer
  | float
  ;

array
  : '[' list ']'
  {
    $$ = $2;
  }
  ;

list
  : list value
  | value
  ;

object
  : '{' map '}'
  {
    $$ = $2;
  }
  ;

map
  : map string value
  | string value
  ;

string
  : unquoted_string
  | quoted_string
  ;

unquoted_string
  : strpart
  ;

quoted_string
  : '"' strparts '"'
  {
    $$ = $2;
  }
  ;

strparts
  : strparts strpart
  | strpart
  ;

strpart
  : STR
  {
    $$ = 0;
  }
  | CREF
  {

  }
  | HREF
  {

  }
  ;

boolean
  : BOOL
  {

  }
  ;

integer
  : INT
  {

  }
  | HEX
  {

  }
  ;

float
  : FLOAT
  {

  }
  ;
