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
  #include <stdio.h>

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/dictionary.h"
  #include "valyria/list.h"

  #include "operations.internal.h"

  #include "macros.h"
  #include "attrs.h"
}

%code top {
  int operations_yylex(void *, void*, void*);
  #define PARSER containerof(parser, operations_parser, yyu)
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
  struct identifier * identifier;
  struct identifier_list * identifier_list;
  struct operation * operation;
}

/* tokens */
%token
 ':'
 '!'
 '+'
 '='
 '~'
 '_'
 SLASH2

/* terminals */
%token            STR
%token <yyu.imax> INTMAX8
%token <yyu.imax> INTMAX16
%token <yyu.umax> UINTMAX8
%token <yyu.umax> UINTMAX16
%token <yyu.umax> UINTMAX32
%token <yyu.umax> HEX8
%token <yyu.umax> HEX16
%token <yyu.umax> HEX32

/* nonterminals */
%type <identifier>
 identifier
 label
 identifier-sequence

%type <identifier_list>
 identifier-sequence-list

%type <operation>
 operation
 edge

%type <u32>
 vertex_attrs
 edge_attrs
 uint32

%destructor { identifier_list_free($$); } <identifier_list>
%destructor { identifier_free($$); } <identifier>
%destructor { operation_free($$); } <operation>

%%
utterance
  : operations
  |
  ;

operations
  : operations operation
  {
    YFATAL(list_push, PARSER->li, $2, 0);
  }
  | operation
  {
    YFATAL(list_push, PARSER->li, $1, 0);
  }
  ;

operation
 /* declare some vertices */
  : identifier-sequence-list
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $1;
    $$->type = GRAPH_OPERATION_VERTEX;
  }

  /* refresh a vertex */
  | '@' identifier-sequence-list
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $2;
    $$->type = GRAPH_OPERATION_REFRESH;
  }

  /* invalidate a vertex */
  | '~' identifier-sequence-list
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $2;
    $$->type = GRAPH_OPERATION_REFRESH;
  }

  /* connect an edge */
  | '+' edge
  {
    $$ = $2;
    $$->type = GRAPH_OPERATION_CONNECT;
  }

  /* disconnect an edge */
  | '=' edge
  {
    $$ = $2;
    $$->type = GRAPH_OPERATION_DISCONNECT;
  }
  ;

edge
  : identifier-sequence-list ':' edge_attrs ':' identifier-sequence-list
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $1;
    $$->B = $5;
    $$->attrs = $3;
  }
  | identifier-sequence-list ':' identifier-sequence-list
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $1;
    $$->B = $3;
  }
  ;

identifier-sequence-list
  : identifier-sequence-list ',' identifier-sequence
  {
    $$ = $1;
    YFATAL(xrealloc, &$$->v, sizeof(*$$->v), $$->len + 1, $$->len);
    if($$->len == 1)
      $$->v[0] = $$->v0;
    $$->v[$$->len++] = $3;
  }
  | identifier-sequence
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->v0 = $1;
    $$->len = 1;
  }
  | '_'
  {
    $$ = 0;
  }
  ;

identifier-sequence
  : identifier-sequence '/' identifier
  {
    $$ = $3;
    $$->next = $1;
  }
  | SLASH2 identifier
  {
    $$ = $2;
    $$->op_attrs = MORIA_OPATTRS_INIT_SLASH;
  }
  | identifier
  {
    $$ = $1;
    $$->op_attrs = MORIA_OPATTRS_INIT;
  }
  ;

identifier
  : label '!' vertex_attrs
  {
    $$ = $1;
    $$->attrs = $3;
  }
  | label
  ;

label
  : STR
  {
    YFATAL(xmalloc, &$$, sizeof(*$$) + @1.l + 1);
    memcpy($$->label, @1.s, @1.l);
    $$->label_len = @1.l;
  }
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
