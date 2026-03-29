/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include "types.h"

  #include "bacon/parser.h"
  #include "bacon/make.h"

  #include "mak_parser/glue.h"
  #include "mak_parser.h"

  struct bacon_value;
}

%code top {
  int mak_yylex(void*, void*, void*);
  #define PARSER ((mak_parser*)parser)
  #define BACON PARSER->bacon_parser
  #define MAK_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {mak_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;
  bacon_value *val;
}

/* terminals */
%token '='

%token NAME  300
%token VALUE 301

/* nonterminals */
%type <val> items
%type <val> mapping name value

%destructor { bacon_value_free(BACON, $$); } <val>

%%
utterance
  : items
  {
    PARSER->root = $1;
  }
  | %empty
  ;

items
  : items mapping
  {
    if($2) {
      bacon_set_mkv(BACON, &@$, $1, &$$, $2);
    } else {
      $$ = $1;
    }
  }
  | mapping
  {
    if($1) {
      bacon_set_mkv(BACON, &@$, 0, &$$, $1);
    } else {
      $$ = $1;
    }
  }
  ;

mapping
  : name '=' value
  {
    bacon_mapping_mk(BACON, &@$, &$$, $1, $3);
  }
  | name '='
  {
    bacon_value_free(BACON, $1);
    $$ = 0;
  }
  ;

name
  : NAME
  {
    bacon_string_mkw(BACON, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

value
  : VALUE
  {
    bacon_string_mkw(BACON, &@$, 0, &$$, @1.s, @1.l);
  }
  ;
