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
}

%code top {
  int operations_yylex(void *, void*, void*);
}

%define api.pure
%error-verbose
%locations
%name-prefix "operations_yy"
%parse-param { void* scanner }
%parse-param { struct operations_xtra* xtra }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
  uint32_t u32;
  struct identifier * identifier;
  struct operation * operation;
}

/* tokens */
%token
 ':'
 '!'
 '+'
 '-'
 '~'

/* terminals */
%token        STR
%token <u32>
 HEX
 POSINT

/* nonterminals */
%type <identifier>
 identifier
 label
 vertex

%type <operation>
 operation
 edge

%type <u32>
 attrs
 number

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
    YFATAL(list_push, xtra->li, $2);
  }
  | operation
  {
    YFATAL(list_push, xtra->li, $1);
  }
  | operations definition
  | definition
  ;

definition
  : STR number
  {
    typeof($2) *T;
    YFATAL(dictionary_set, xtra->definitions, @1.s, @1.l, &T);
    *T = $2;
  }
  ;

operation
 /* declare a vertex */
  : vertex
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $1;
    $$->type = GRAPH_OPERATION_VERTEX;
  }

 /* connect an edge */
  | '+' edge
  {
    $$ = $2;
    $$->type = GRAPH_OPERATION_CONNECT;
  }

 /* disconnect an edge */
  | '-' edge
  {
    $$ = $2;
    $$->type = GRAPH_OPERATION_DISCONNECT;
  }

 /* disintegrate a subgraph starting at an edge and proceeding downwards */
  | '~' edge
  {
    $$ = $2;
    $$->type = GRAPH_OPERATION_DISINTEGRATE;
  }
  ;

edge
  : vertex ':' attrs ':' vertex 
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $1;
    $$->B = $5;
    $$->attrs = $3;
  }
  | vertex ':' vertex
  {
    YFATAL(xmalloc, &$$, sizeof(*$$));
    $$->A = $1;
    $$->B = $3;
  }
  ;

vertex
  : identifier
  | identifier '!' attrs
  {
    $$ = $1;
    $$->attrs = $3;
  }
  ;

identifier
  : identifier '/' label
  {
    $$ = $3;
    $$->next = $1;
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

attrs
  : number
  | STR
  {
    typeof($$) * T;
    $$ = 0;
    if((T = dictionary_get(xtra->definitions, @1.s, @1.l)))
      $$ = *T;
  }
  ;
  
number
  : HEX
  | POSINT
  ;
