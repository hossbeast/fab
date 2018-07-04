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

  #include "xlinux/xstdlib.h"
  #include "xlinux/xstring.h"
  #include "valyria/dictionary.h"
  #include "valyria/map.h"

  #include "parser.internal.h"
  #include "vertex.internal.h"
  #include "operations.internal.h"
  #include "graph.internal.h"

  #include "zbuffer.h"
}

%code top {
  int graph_yylex(void *, void*, void*);
}

%define api.pure
%error-verbose
%locations
%name-prefix "graph_yy"
%parse-param { void* scanner }
%parse-param { struct graph_xtra* xtra }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
  uint32_t u32;
  uint64_t u64;
  struct vertex * vertex;
  char * label;
}

/* tokens */
%token ':'
%token '!'

/* terminals */
%token        STR
%token <u32>
 HEX
 POSINT

/* nonterminals */
%type <vertex>
 vertex
 vertex_label
 edge_vertex

%type <label>
 label

%type <u32>
 attrs

%destructor { wfree($$); } <label>

%%
utterance
  : graph
  ;

graph
  : graph definition
  | graph edge
  | graph vertex
  | definition
  | edge
  | vertex
  ;

definition
  : STR HEX
  {
    typeof($2) * T;
    YFATAL(dictionary_set, xtra->definitions, @1.s, @1.l, &T);
    *T = $2;
  }
  ;

edge
  : edge_vertex ':' attrs ':' edge_vertex 
  {
    YFATAL(graph_connect, xtra->g, $1, $5, $3);
  }
  | edge_vertex ':' edge_vertex
  {
    YFATAL(graph_connect, xtra->g, $1, $3, 0);
  }
  ;

edge_vertex
  : POSINT
  {
    $$ = map_get(xtra->vertex_map, &$1, sizeof($1));
  }
  | vertex
  ;

vertex
  : POSINT '-' vertex_label '!' attrs
  {
    $$ = $3;
    $$->attrs = $5;
    YFATAL(map_set, xtra->vertex_map, &$1, sizeof($1), $$);
  }
  | POSINT '-' vertex_label
  {
    $$ = $3;
    YFATAL(map_set, xtra->vertex_map, &$1, sizeof($1), $$);
  }
  | vertex_label '!' attrs
  {
    $$ = $1;
    $$->attrs = $3;
  }
  | vertex_label
  ;

vertex_label
  : label
  {
    YFATAL(vertex_creates, &$$, xtra->g, 0, $1);
    *(char **)vertex_value($$) = $1;
  }
  ;

label
  : STR
  {
    $$ = 0;
    YFATAL(ixstrncat, &$$, @1.s, @1.l);
  }
  ;

attrs
  : HEX
  | STR
  {
    typeof($$) * T;
    $$ = 0;
    if((T = dictionary_get(xtra->definitions, @1.s, @1.l)))
      $$ = *T;
  }
  ;
