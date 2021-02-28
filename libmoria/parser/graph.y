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
  #include "moria.h"
  #include "vertex.h"
  #include "operations.h"
  #include "graph.internal.h"

  #include "zbuffer.h"
  #include "common/attrs.h"
}

%code top {
  int graph_yylex(void *, void*, void*);
  #define PARSER containerof(parser, graph_parser, graph_yyu)
  #define GRAPH_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {graph_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
  yyu_lval yyu;

  uint32_t u32;

  struct {
    struct moria_vertex *v0;
    struct moria_vertex **v;
    uint16_t len;
  } vertex_list;

  struct moria_vertex * vertex;
}

/* tokens */
%token
 '!'
 '+' // --
 ':'
 '=' // --
 '_'
 '~' // --
 ','
 '-'
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
%type <vertex>
 vertex
 vertex_label
 edge_vertex

%type <vertex_list>
 edge_vertex_list

%type <u32>
 vertex_attrs
 edge_attrs
 uint32
 hex32

%%
utterance
  : graph
  ;

graph
  : graph edge
  | graph vertex
  | edge
  | vertex
  ;

edge
  : edge_vertex_list ':' edge_attrs ':' edge_vertex_list
  {
    struct moria_vertex ** A = $1.v;
    if($1.len == 1)
      A = &$1.v0;

    struct moria_vertex ** B = $5.v;
    if($5.len == 1)
      B = &$5.v0;

    YFATAL(graph_parser_hyperconnect, PARSER, A, $1.len, B, $5.len, $3, 0);
    if($1.len > 1)
      wfree($1.v);
    if($5.len > 1)
      wfree($5.v);
  }
  | edge_vertex_list ':' edge_vertex_list
  {
    struct moria_vertex ** A = $1.v;
    if($1.len == 1)
      A = &$1.v0;

    struct moria_vertex ** B = $3.v;
    if($3.len == 1)
      B = &$3.v0;

    YFATAL(graph_parser_hyperconnect, PARSER, A, $1.len, B, $3.len, 0, 0);
    if($1.len > 1)
      wfree(A);
    if($3.len > 1)
      wfree(B);
  }
  ;

edge_vertex_list
  : edge_vertex_list ',' edge_vertex
  {
    $$ = $1;
    YFATAL(xrealloc, &$$.v, sizeof(*$$.v), $$.len + 1, $$.len);
    if($$.len == 1)
      $$.v[0] = $$.v0;
    $$.v[$$.len++] = $3;
  }
  | edge_vertex
  {
    $$.v = 0;
    $$.v0 = $1;
    $$.len = 1;
  }
  | '_'
  {
    $$.v = 0;
    $$.v0 = 0;
    $$.len = 0;
  }
  ;

edge_vertex
  : uint32
  {
    $$ = map_get(PARSER->vertex_map, (void*)&$1, sizeof($1));
  }
  | vertex
  ;

vertex
  : uint32 '-' vertex_label '!' vertex_attrs
  {
    $$ = $3;
    $$->attrs = $5;
    YFATAL(map_put, PARSER->vertex_map, (void*)&$1, sizeof($1), $$, 0);
  }
  | uint32 '-' vertex_label
  {
    $$ = $3;
    YFATAL(map_put, PARSER->vertex_map, (void*)&$1, sizeof($1), $$, 0);
  }
  | vertex_label '!' vertex_attrs
  {
    $$ = $1;
    $$->attrs = $3;
  }
  | vertex_label
  ;

vertex_label
  : STR
  {
    YFATAL(graph_parser_create_vertex, PARSER, &$$, 0, 0, @1.s, @1.l);
  }
  ;

vertex_attrs
  : hex32
  | STR
  {
    $$ = attrs32_value_bynamew(PARSER->vertex_defs, @1.s, @1.l);
  }
  ;

edge_attrs
  : hex32
  | STR
  {
    $$ = attrs32_value_bynamew(PARSER->edge_defs, @1.s, @1.l);
  }
  ;

hex32
  : HEX8
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
  ;
