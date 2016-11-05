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
  #include <stdint.h>
  #include <string.h>

  #include "value.h"
  #include "value/make.h"
  #include "valyria/list.h"
  #include "valyria/pstring.h"

  #include "config_parser.internal.h"
}

%code top {
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
  double      fp;

  value * key;

  struct {
    value * key;
    value * val;
    uint16_t attr;
  } mapping;

  struct value * val;
}

/* tokens */
%token ':'
%token ','
%token '{'
%token '}'
%token '+'
%token '='

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
%type <val> value scalar aggregate
%type <mapping> mapping
%type <key> key

%%
utterance
  : map
  {
    xtra->root = $1;
  }
  | value
  {
    xtra->root = $1;
  }
  ;

value
  : scalar
  | aggregate
  ;

scalar
  : string
  | boolean
  | integer
  | float
  ;

aggregate
  : array
  | object
  ;

array
  : '[' list ']'
  {
    $$ = $2;
  }
  ;

list
  : list value
  {
    YFATAL(value_list_mkv, xtra->stor, &@$, $1, &$$, $2);
  }
  | value
  {
    YFATAL(value_list_mkv, xtra->stor, &@$, 0, &$$, $1);
  }
  ;

object
  : '{' map '}'
  {
    $$ = $2;
  }
  ;

map
  : map mapping
  {
    if($2.key->els->l == 1)
    {
      YFATAL(value_map_mkv, xtra->stor, &@$, $1, &$$, list_get($2.key->els, 0), $2.val, $2.attr);
    }
    else
    {
      YFATAL(value_map_mkv, xtra->stor, &@$, 0, &$$, list_get($2.key->els, $2.key->els->l - 1), $2.val, $2.attr);

      int x;
      for(x = $2.key->els->l - 2; x > 0; x--)
        YFATAL(value_map_mkv, xtra->stor, &@$, 0, &$$, list_get($2.key->els, x), $$, 0);

      YFATAL(value_map_mkv, xtra->stor, &@$, $1, &$$, list_get($2.key->els, 0), $$, 0);
    }
  }
  | mapping
  {
    if($1.key->els->l == 1)
    {
      YFATAL(value_map_mkv, xtra->stor, &@$, 0, &$$, list_get($1.key->els, 0), $1.val, $1.attr);
    }
    else
    {
      YFATAL(value_map_mkv, xtra->stor, &@$, 0, &$$, list_get($1.key->els, $1.key->els->l - 1), $1.val, $1.attr);

      int x;
      for(x = $1.key->els->l - 2; x > 0; x--)
        YFATAL(value_map_mkv, xtra->stor, &@$, 0, &$$, list_get($1.key->els, x), $$, 0);

      YFATAL(value_map_mkv, xtra->stor, &@$, 0, &$$, list_get($1.key->els, 0), $$, 0);
    }
  }
  ;

mapping
  : key '+' '=' aggregate
  {
    $$.key = $1;
    $$.val = $4;
    $$.attr = VALUE_MERGE_ADD;
  }
  | key '=' aggregate
  {
    $$.key = $1;
    $$.val = $3;
    $$.attr = VALUE_MERGE_SET;
  }
  | key value
  {
    $$.key = $1;
    $$.val = $2;
    $$.attr = 0;
  }
  ;

key
  : key '.' string
  {
    YFATAL(value_list_mkv, xtra->stor, &@$, $1, &$$, $3);
  }
  | string
  {
    YFATAL(value_list_mkv, xtra->stor, &@$, 0, &$$, $1);
  }
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
  | '"' '"'
  {
    $$ = 0;
  }
  ;

strparts
  : strparts strpart
  {
    YFATAL(value_string_mkv, xtra->stor, &@$, $1, &$$, $2);
  }
  | strpart
  ;

strpart
  : STR
  {
    YFATAL(value_string_mkw, xtra->stor, &@$, 0, &$$, @1.s, @1.l);
  }
  | CREF
  {
    YFATAL(value_string_mkc, xtra->stor, &@$, 0, &$$, $1);
  }
  | HREF
  {
    YFATAL(value_string_mkc, xtra->stor, &@$, 0, &$$, $1);
  }
  ;

boolean
  : BOOL
  {
    YFATAL(value_boolean_mk, xtra->stor, &@$, &$$, $1);
  }
  ;

integer
  : INT
  {
    YFATAL(value_integer_mk, xtra->stor, &@$, &$$, $1);
  }
  | HEX
  {
    YFATAL(value_integer_mk, xtra->stor, &@$, &$$, $1);
  }
  ;

float
  : FLOAT
  {
    YFATAL(value_float_mk, xtra->stor, &@$, &$$, $1);
  }
  ;
