/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include "types.h"

  #include "moria/traverse.h"
  #include "valyria/llist.h"
  #include "valyria/pstring.h"
  #include "xlinux/xstdlib.h"
  #include "xlinux/xstring.h"

  #include "selector_parser.h"
  #include "selector.h"
  #include "fsent.h"
  #include "pattern.h"

  #include "macros.h"

  struct pattern;
  struct yyu_location;
  struct yyu_parser;

  /* called from tab.o and lex.o */
  static void selector_yyerror(struct yyu_location* loc, void * scanner, struct yyu_parser * pp, char const * err)
    __attribute__((weakref("yyu_grammar_error")));
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
  moria_traversal_mode traverse_order;
  bool traverse_cross_module;

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
%type <traverse_direction> traverse-direction
%type <traverse_order> traverse-order

 /* keywords */
%token
  ALL             "all"
  DIRECTION       "direction"
  DOWN            "down"
  EXTENSION       "extension"
  EXHAUSTIVE      "exhaustive"
  CROSS_MODULE    "cross-module"
  NOFOLLOW        "nofollow"
  HIDDEN          "hidden"
  FS              "fs"
  FS_HIDDEN       "fs-hidden"
  DIRTREE         "dirtree"
  MODTREE         "modtree"
  FILE_TYPE       "file-type"
  IMPORTS         "imports"
  MAX_DISTANCE    "max-distance"
  MIN_DISTANCE    "min-distance"
  MODULE          "module"
  ANCHOR          "anchor"
  NONE            "none"
  PATTERN         "pattern"
  LOOKUP          "lookup"
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
    yyu_loc_final(&PARSER->yyu, &@1);
    if(PARSER->yyu.attrs & YYU_PARTIAL)
    {
      YYACCEPT;
    }
  }
  ;

allocate-sequence-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_SEQUENCE, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-union-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_UNION, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-pattern-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_PATTERN, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-lookup-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_LOOKUP, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-traverse-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_TRAVERSE, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
    sel->criteria.min_depth = SELECTOR_TRAVERSE_DEFAULT_MIN_DEPTH;
    sel->criteria.max_depth = SELECTOR_TRAVERSE_DEFAULT_MAX_DEPTH;
    sel->mode = SELECTOR_TRAVERSE_DEFAULT_MODE;
    sel->direction = SELECTOR_TRAVERSE_DEFAULT_DIRECTION;
    sel->cross_module = SELECTOR_TRAVERSE_DEFAULT_CROSS_MODULE;
    selector_graph_fstree(&sel->criteria);
  }
  ;

allocate-nodeset-selector
  : %empty
  {
    selector * sel;
    selector_alloc(SELECTOR_NODESET, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

selector-utterance
  : aggregate-selector
  /* single operation wrapped in default sequence selector */
  | allocate-sequence-selector operation-selector
  {
    selector * sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector * Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, &sel->lln);
  }
  /* default sequence selector */
  | allocate-sequence-selector '[' selectors ']'
  ;

selectors
  : selectors selector
  {
    selector * sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector * Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, &sel->lln);
  }
  | selector
  {
    selector * sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector * Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, &sel->lln);
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
  | lookup-selector
  | traverse-selector
  ;

sequence-selector
  : allocate-sequence-selector SEQUENCE ':' '[' selectors ']'
  ;

union-selector
  : allocate-union-selector UNION ':' '[' selectors ']'
  ;

nodeset-selector
  : allocate-nodeset-selector selector-nodeset
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nodeset = $2;
  }
  ;

pattern-selector
  : allocate-pattern-selector PATTERN ':' lookup-pattern
  {
    llist_first(&PARSER->selector_stack, selector, lln)->pattern = $4;
  }
  ;

lookup-selector
  : allocate-lookup-selector LOOKUP ':' '{' lookup-settings '}'
  ;

lookup-settings
  : lookup-settings lookup-setting
  | lookup-setting
  ;

lookup-setting
  : PATH ':' string
  {
    llist_first(&PARSER->selector_stack, selector, lln)->path = $3;
    llist_first(&PARSER->selector_stack, selector, lln)->path_len = strlen($3);
  }
  | NOFOLLOW ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nofollow = $3;
  }
  | HIDDEN ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->hidden = $3;
  }
  ;

traverse-selector
  : allocate-traverse-selector TRAVERSE ':' '{' traverse-settings '}'
  ;

traverse-settings
  : traverse-settings traverse-setting
  | traverse-setting
  ;

traverse-setting
  /* control the traversal */
  : GRAPH ':' graph-type
  | DIRECTION ':' traverse-direction
  {
    llist_first(&PARSER->selector_stack, selector, lln)->direction = $3;
  }
  | MIN_DISTANCE ':' uint16
  {
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.min_depth = $3;
  }
  | MAX_DISTANCE ':' uint16
  {
    llist_first(&PARSER->selector_stack, selector, lln)->criteria.max_depth = $3;
  }
  | EXHAUSTIVE ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->exhaustive = $3;
  }
  | CROSS_MODULE ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->cross_module = $3;
  }
  | NOFOLLOW ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nofollow = $3;
  }
  | ORDER ':' traverse-order
  {
    llist_first(&PARSER->selector_stack, selector, lln)->mode = $3;
  }
  ;

graph-type
  : FS        { selector_graph_fstree(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | FS_HIDDEN { selector_graph_fstree_hidden(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | MODTREE   { selector_graph_modtree(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | DIRTREE   { selector_graph_dirtree(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | DEPENDS   { selector_graph_depends(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | IMPORTS   { selector_graph_imports(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | USES      { selector_graph_uses(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  | REQUIRES  { selector_graph_requires(&llist_first(&PARSER->selector_stack, selector, lln)->criteria); }
  ;

traverse-direction
  : UP   { $$ = MORIA_TRAVERSE_UP; }
  | DOWN { $$ = MORIA_TRAVERSE_DOWN; }
  ;

traverse-order
  : PREORDER  { $$ = MORIA_TRAVERSE_PRE; }
  | POSTORDER { $$ = MORIA_TRAVERSE_POST; }

selector-nodeset
  : SOURCE  { $$ = SELECTOR_NODESET_SOURCE; }
  | SOURCES { $$ = SELECTOR_NODESET_SOURCES; }
  | TARGET  { $$ = SELECTOR_NODESET_TARGET; }
  | TARGETS { $$ = SELECTOR_NODESET_TARGETS; }
  | MODULE  { $$ = SELECTOR_NODESET_MODULE; }
  | ANCHOR  { $$ = SELECTOR_NODESET_ANCHOR; }
  | ALL     { $$ = SELECTOR_NODESET_ALL; }
  | NONE    { $$ = SELECTOR_NODESET_NONE; }
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
  : strparts
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
  | uint16
  {
    $$ = 0;
    ixstrndup(&$$, @1.s, @1.l);
  }
  ;
