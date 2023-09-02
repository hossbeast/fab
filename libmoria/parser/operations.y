/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <stdint.h>
  #include <string.h>
  #include <stdio.h>

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/list.h"

  #include "parser.internal.h"
  #include "operations.h"

  #include "macros.h"
  #include "common/attrs.h"
}

%code top {
  int operations_yylex(void *, void*, void*);
  #define PARSER containerof(parser, moria_parser, operations_yyu)
  #define OPERATIONS_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {operations_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
  yyu_lval yyu;
  uint32_t u32;
  struct label *label;
  struct identifier *identifier;
  struct identifier_list *identifier_list;
  struct {
    uint32_t attrs;
    identifier_list *B;
    identifier_list *A;
  } edge_spec;
}

/* tokens */
%token
 '!'
 '+'
 ':'
 '='
 '_'
 '~'
 SLASH2

/* terminals */
%token            STR         104 "string"
%token <yyu.imax> INTMAX8     105
%token <yyu.imax> INTMAX16    106
%token <yyu.umax> UINTMAX8    107
%token <yyu.umax> UINTMAX16   108
%token <yyu.umax> UINTMAX32   109
%token <yyu.umax> HEX8        110
%token <yyu.umax> HEX16       111
%token <yyu.umax> HEX32       112

/* nonterminals */
%type <label>
 name
 label

%type <identifier>
 identifier

%type <identifier_list>
 identifier-list

%type <edge_spec>
 edge

%type <u32>
 vertex_attrs
 edge_attrs
 uint32

%destructor { identifier_list_free($$.A); identifier_list_free($$.B); } <edge_spec>
%destructor { identifier_list_free($$); } <identifier_list>
%destructor { identifier_free($$); } <identifier>
%destructor { label_free($$); } <label>

%%
utterance
  : operations
  |
  ;

operations
  : operations operation
  | operation
  ;

operation
 /* declare some vertices */
  : identifier-list
  {
    operations_declare(PARSER, $1);
  }

  /* refresh some vertices */
  | '@' identifier-list
  {
    operations_refresh(PARSER, $2);
  }

  /* invalidate some vertices */
  | '~' identifier-list
  {
    operations_invalidate(PARSER, $2);
  }

  /* connect an edge */
  | '+' edge
  {
    operations_connect(PARSER, $2.B, $2.A, $2.attrs);
  }

  /* disconnect an edge */
  | '=' edge
  {
    operations_disconnect(PARSER, $2.B, $2.A);
  }
  ;

edge
  : identifier-list ':' edge_attrs ':' identifier-list
  {
    $$.B = $1;
    $$.A = $5;
    $$.attrs = $3;
  }
  | identifier-list ':' identifier-list
  {
    $$.B = $1;
    $$.A = $3;
    $$.attrs = 0;
  }
  ;

identifier-list
  : identifier-list ',' identifier
  {
    $$ = $1;
    llist_push(&$$->identifiers, &$3->lln);
  }
  | identifier
  {
    $$ = identifier_list_alloc();
    llist_push(&$$->identifiers, &$1->lln);
  }
  | '_'
  {
    $$ = identifier_list_alloc();
  }
  ;

identifier
  : identifier '/' label
  {
    $$ = $1;
    llist_push(&$$->labels, &$3->lln);
  }
  | SLASH2 label
  {
    $$ = identifier_alloc($2, MORIA_OPATTRS_INIT_SLASH);
  }
  | label
  {
    $$ = identifier_alloc($1, MORIA_OPATTRS_INIT);
  }
  ;

label
  : name '!' vertex_attrs
  {
    $$ = $1;
    $$->attrs = $3;
  }
  | name
  ;

name
  : STR
  {
    $$ = label_alloc(@1.s, @1.l);
  }
  | '"' words '"'
  {
    $$ = label_alloc(@2.s, @2.l);
  }
  ;

words
  : words word
  | word
  ;

word
  : STR
  ;

vertex_attrs
  : uint32
  | STR
  {
    $$ = attrs32_value_bynamew(PARSER->vertex_defs, @1.s, @1.l);
  }
  ;

edge_attrs
  : uint32
  | STR
  {
    $$ = attrs32_value_bynamew(PARSER->edge_defs, @1.s, @1.l);
  }
  ;

uint32
  : INTMAX8
  {
    $$ = $1;
  }
  | INTMAX16
  {
    $$ = $1;
  }
  | UINTMAX8
  {
    $$ = $1;
  }
  | UINTMAX16
  {
    $$ = $1;
  }
  | UINTMAX32
  {
    $$ = $1;
  }
  | HEX8
  {
    $$ = $1;
  }
  | HEX16
  {
    $$ = $1;
  }
  | HEX32
  {
    $$ = $1;
  }
  ;
