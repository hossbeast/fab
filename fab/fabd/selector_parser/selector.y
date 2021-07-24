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
  #include "types.h"

  #include "moria/traverse.h"
  #include "valyria/list.h"
  #include "valyria/llist.h"
  #include "valyria/pstring.h"
  #include "xlinux/xstdlib.h"
  #include "xlinux/xstring.h"

  #include "selector_parser.internal.h"
  #include "selector.h"
  #include "fsent.h"
  #include "pattern.h"

  #include "macros.h"

  struct pattern;
}

%code top {
  int selector_yylex(void *, void*, void*);
  #define PARSER containerof(parser, selector_parser, yyu)
  #define SELECTOR_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {selector_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;

  uint16_t  u16;
  struct pattern * pattern;

  // enums
  selector_nodeset selector_nodeset;
  moria_traversal_direction traverse_direction;
  moria_traversal_mode traverse_mode;
  struct {
    edge_type edge_travel;
    edge_type edge_visit;
    vertex_filetype vertex_travel;
    vertex_filetype vertex_visit;
    bool cross_module;
  } graph_type;

  // result
  selector *  selector;
}

/* terminals */
%token             STR        "string"
%token <yyu.u8>    CREF       "character-escape"
%token <yyu.u8>    HREF       "hex-escape"
%token <yyu.b>     BOOL       "boolean"
%token <yyu.umax>  UINTMAX8
%token <yyu.umax>  UINTMAX16
%token <yyu.umax>  UINTMAX32
%token <yyu.umax>  UINTMAX64
%token <yyu.imax>  INTMIN8
%token <yyu.imax>  INTMIN16
%token <yyu.imax>  INTMIN32
%token <yyu.imax>  INTMIN64
%token <yyu.imax>  INTMAX8
%token <yyu.imax>  INTMAX16
%token <yyu.imax>  INTMAX32
%token <yyu.imax>  INTMAX64

%type <selector_nodeset> selector-nodeset
%type <graph_type> graph-type
%type <traverse_direction> traverse-direction
%type <traverse_mode> traverse-mode

 /* keywords */
%token
  ALL             "all"
  DIRECTION       "direction"
  DOWN            "down"
  EXTENSION       "extension"
  EXHAUSTIVE      "exhaustive"
  FS              "fs"
  DIRTREE         "dirtree"
  MODTREE         "modtree"
  FILE_TYPE       "file-type"
  IMPORTS         "imports"
  MAX_DISTANCE    "max-distance"
  MIN_DISTANCE    "min-distance"
  MODULE          "module"
  MODULES         "modules"
  NONE            "none"
  PATTERN         "pattern"
  PATH            "path"
  REG             "reg"
  GRAPH           "graph"
  REQUIRES        "requires"
  SEQUENCE        "sequence"
  DEPENDS         "depends"
  USES            "uses"
  SOURCE          "source"
  SOURCES         "sources"
  TARGET          "target"
  TARGETS         "targets"
  TRAVERSE        "traverse"
  UNION           "union"
  UP              "up"
  ORDER           "order"
  PREORDER        "preorder"
  POSTORDER       "postorder"

%token <pattern> LOOKUP_PATTERN "lookup-pattern"
%type <pattern> lookup-pattern

/* nonterminals */
%type <u16> uint16
%type <yyu.s> string quoted-string unquoted-string strpart strparts

%destructor { pattern_free($$); } <pattern>

%%
utterance
  : selector-utterance
  {
    if(PARSER->yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->yyu, &@1);
      YYACCEPT;
    }
  }
  ;

allocate-sequence-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_SEQUENCE, &sel);
    llist_prepend(&PARSER->selector_stack, sel, lln);
  }
  ;

allocate-union-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_UNION, &sel);
    llist_prepend(&PARSER->selector_stack, sel, lln);
  }
  ;

allocate-pattern-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_PATTERN, &sel);
    llist_prepend(&PARSER->selector_stack, sel, lln);
  }
  ;

allocate-path-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_PATH, &sel);
    llist_prepend(&PARSER->selector_stack, sel, lln);
  }
  ;

allocate-traverse-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_TRAVERSE, &sel);
    llist_prepend(&PARSER->selector_stack, sel, lln);
    sel->criteria = SELECTOR_TRAVERSE_DEFAULT_CRITERIA;
    sel->mode = SELECTOR_TRAVERSE_DEFAULT_MODE;
    sel->direction = SELECTOR_TRAVERSE_DEFAULT_DIRECTION;
    sel->cross_module = SELECTOR_TRAVERSE_DEFAULT_CROSS_MODULE;
//    sel->min_distance = SELECTOR_TRAVERSE_DEFAULT_MIN_DISTANCE;
//    sel->max_distance = SELECTOR_TRAVERSE_DEFAULT_MAX_DISTANCE;
  }
  ;

allocate-nodeset-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_NODESET, &sel);
    llist_prepend(&PARSER->selector_stack, sel, lln);
  }
  ;

selector-utterance
  : aggregate-selector
  /* single operation wrapped in default sequence selector */
  | allocate-sequence-selector operation-selector
  {
    selector * sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector * Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, sel, lln);
  }
  /* default sequence selector */
  | allocate-sequence-selector '[' selectors ']'
  ;

selectors
  : selectors selector
  {
    selector * sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector * Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, sel, lln);
  }
  | selector
  {
    selector * sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector * Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, sel, lln);
  }
  ;

selector
  : aggregate-selector
  | operation-selector
  ;

aggregate-selector
  : sequence-selector
  | union-selector
  ;

operation-selector
  : nodeset-selector
  | pattern-selector
  | path-selector
  | traverse-selector
  ;

sequence-selector
  : allocate-sequence-selector SEQUENCE join '[' selectors ']'
  ;

union-selector
  : allocate-union-selector UNION join '[' selectors ']'
  ;

nodeset-selector
  : allocate-nodeset-selector selector-nodeset
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nodeset = $2;
  }
  ;

pattern-selector
  : allocate-pattern-selector PATTERN join lookup-pattern
  {
    llist_first(&PARSER->selector_stack, selector, lln)->pattern = $4;
  }
  ;

path-selector
  : allocate-path-selector PATH join string
  {
    llist_first(&PARSER->selector_stack, selector, lln)->path = $4;
    llist_first(&PARSER->selector_stack, selector, lln)->path_len = strlen($4);
  }
  ;

traverse-selector
  : allocate-traverse-selector TRAVERSE join '{' traverse-settings '}'
  ;

traverse-settings
  : traverse-settings traverse-setting
  | traverse-setting
  ;

traverse-setting
  /* control the traversal */
  : GRAPH join graph-type
  {
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.edge_travel = $3.edge_travel;
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.edge_visit = $3.edge_visit;
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.vertex_travel = $3.vertex_travel;
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.vertex_visit = $3.vertex_visit;
    llist_first(&PARSER->selector_stack, selector, lln)->cross_module = $3.cross_module;
  }
  | DIRECTION join traverse-direction
  {
    llist_first(&PARSER->selector_stack, selector, lln)->direction = $3;
  }
  | MIN_DISTANCE join uint16
  {
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.min_depth = $3;
  }
  | MAX_DISTANCE join uint16
  {
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.max_depth = $3;
  }
  | EXHAUSTIVE join BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->exhaustive = $3;
  }
  | ORDER join traverse-mode
  {
    llist_first(&PARSER->selector_stack, selector, lln)->mode = $3;
  }
  | EXTENSION join string
  {
    free(llist_first(&PARSER->selector_stack, selector, lln)->extension);
    llist_first(&PARSER->selector_stack, selector, lln)->extension = $3;
    llist_first(&PARSER->selector_stack, selector, lln)->extension_len = strlen($3);
  }
  ;

graph-type
  : FS        { $$ = (typeof($$)){ edge_travel : EDGE_FSTREE, cross_module : true }; }
  | MODTREE   { $$ = (typeof($$)){ edge_travel : EDGE_FSTREE, vertex_visit : VERTEX_MODULE_DIR, cross_module : true }; }
  | DIRTREE   { $$ = (typeof($$)){ edge_travel : EDGE_FSTREE, vertex_visit : VERTEX_FILETYPE_DIR }; }
  | DEPENDS   { $$ = (typeof($$)){ edge_travel : EDGE_DEPENDS }; }
  | IMPORTS   { $$ = (typeof($$)){ edge_travel : EDGE_IMPORTS }; }
  | USES      { $$ = (typeof($$)){ edge_travel : EDGE_USES }; }
  | REQUIRES  { $$ = (typeof($$)){ edge_travel : EDGE_REQUIRES }; }
  ;

traverse-direction
  : UP   { $$ = MORIA_TRAVERSE_UP; }
  | DOWN { $$ = MORIA_TRAVERSE_DOWN; }
  ;

traverse-mode
  : PREORDER  { $$ = MORIA_TRAVERSE_PRE; }
  | POSTORDER { $$ = MORIA_TRAVERSE_POST; }

selector-nodeset
  : SOURCE  { $$ = SELECTOR_NODESET_SOURCE; }
  | SOURCES { $$ = SELECTOR_NODESET_SOURCES; }
  | TARGET  { $$ = SELECTOR_NODESET_TARGET; }
  | TARGETS { $$ = SELECTOR_NODESET_TARGETS; }
  | MODULE  { $$ = SELECTOR_NODESET_MODULE; }
  | MODULES { $$ = SELECTOR_NODESET_MODULES; }
  | ALL     { $$ = SELECTOR_NODESET_ALL; }
  | NONE    { $$ = SELECTOR_NODESET_NONE; }
  ;

join
  : ':'
  | '='
  ;

lookup-pattern
  : LOOKUP_PATTERN
  ;

uint16
  : INTMAX8
  {
    $$ = $1;
  }
  | UINTMAX8
  {
    $$ = $1;
  }
  | INTMAX16
  {
    $$ = $1;
  }
  | UINTMAX16
  {
    $$ = $1;
  }
  ;

string
  : unquoted-string
  | quoted-string
  ;

unquoted-string
  : strpart
  ;

quoted-string
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
    $$ = $1;
    ixstrcat(&$$, $2);
  }
  | strpart
  ;

strpart
  : STR
  {
    $$ = 0;
    ixstrndup(&$$, @1.s, @1.l);
  }
  | CREF
  {
    $$ = 0;
    ixstrndup(&$$, (char*)&$1, 1);
  }
  | HREF
  {
    $$ = 0;
    ixstrndup(&$$, (char*)&$1, 1);
  }
  ;
