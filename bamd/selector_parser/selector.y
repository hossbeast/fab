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
  static void selector_yyerror(struct yyu_location* loc, void *scanner, struct yyu_parser *pp, char const *err)
    __attribute__((weakref("yyu_grammar_error")));
}

%code top {
  int selector_yylex(void *, void*, void*);
  #define PARSER containerof(parser, selector_parser, yyu)
  #define SELECTOR_YYLTYPE yyu_location
  #define TOP llist_first(&PARSER->selector_stack, selector, lln)
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {selector_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;

  struct lookup_pattern *lookup_pattern;
  struct path_pattern *path_pattern;
  struct search_pattern *search_pattern;

  // enums
  selector_nodeset selector_nodeset;
  moria_traversal_direction traverse_direction;
  moria_traversal_mode traverse_order;
  bool traverse_cross_module;
  selector_expect expect;
  selector_sortkey sortkey;

  // result
  selector *selector;
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
%token <yyu.umax>  HEX8
%token <yyu.umax>  HEX16
%token <yyu.umax>  HEX32
%token <yyu.umax>  HEX64

%type <selector_nodeset> selector-nodeset
%type <traverse_direction> traverse-direction
%type <traverse_order> traverse-order
%type <sortkey> sort-key;

 /* keywords */
%token
  DIRECTION       "direction"
  DOWN            "down"
  EXHAUSTIVE      "exhaustive"
  CROSS_MODULE    "cross-module"
  CROSS_FSTREE    "cross-fstree"
  NOFOLLOW        "nofollow"
  HIDDEN          "hidden"
  EXPECT          "expect"
  ANY             "any"
  ZERO            "zero"
  NONZERO         "nonzero"
  ONE             "one"
  FS              "fs"
  FSTREE          "fstree"
  RECORD          "record"
  PRIMARY         "primary"
  INVALID         "invalid"
  MAX_DISTANCE    "max-distance"
  MIN_DISTANCE    "min-distance"
  MODULE          "module"
  MODULES         "modules"
  ANCHOR          "anchor"
  PATTERN         "pattern"
  LOOKUP          "lookup"
  PATH            "path"
  SEARCH          "search"
  GRAPH           "graph"
  VISIT           "visit"
  RULE            "rule"
  RMA             "rma"
  BPE             "bpe"
  CLIENT          "client"
  RULESET         "ruleset"
  FORMULA         "formula"
  VARSET          "varset"
  CONFIGSET       "configset"
  PLUGIN          "plugin"
  DIRECTORY       "directory"
  SEQUENCE        "sequence"
  DEPENDS         "depends"
  SPECIAL         "special"
  RECORDS         "records"
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
  MODULE_OF       "module-of"
  FSTREE_OF       "fstree-of"
  NAMESPACE       "namespace"
  SUBSET          "subset"
  SORT            "sort"
  LIMIT           "limit"
  RUNTIME         "runtime"
  FORKTIME        "forktime"
  ASCENDING       "ascending"
  DESCENDING      "descending"
  KEYS            "keys"

%token <lookup_pattern> LOOKUP_PATTERN "lookup-pattern"
%token <path_pattern> PATH_PATTERN "path-pattern"
%token <search_pattern> SEARCH_PATTERN "search-pattern"

%type <lookup_pattern> lookup-pattern
%type <path_pattern> path-pattern
%type <search_pattern> search-pattern

/* nonterminals */
%type <yyu.u16> uint16
%type <yyu.s> unquoted-string
%type <expect> expect-type

%destructor { pattern_free(&$$->pattern); } <path_pattern>
%destructor { pattern_free(&$$->pattern); } <lookup_pattern>
%destructor { pattern_free(&$$->pattern); } <search_pattern>

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
    selector *sel;
    selector_alloc(SELECTOR_SEQUENCE, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-union-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_UNION, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-subset-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_SUBSET, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-lookup-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_LOOKUP, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);

    sel->expect = SELECTOR_EXPECT_NONZERO;
  }
  ;

allocate-path-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_PATH, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);

    sel->expect = SELECTOR_EXPECT_NONZERO;
  }
  ;

allocate-search-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_SEARCH, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-fstree-of-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_FSTREE_OF, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-module-of-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_MODULE_OF, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

allocate-traverse-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_TRAVERSE, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
    sel->traverse.criteria.min_depth = SELECTOR_TRAVERSE_DEFAULT_MIN_DEPTH;
    sel->traverse.criteria.max_depth = SELECTOR_TRAVERSE_DEFAULT_MAX_DEPTH;
    sel->traverse.mode = SELECTOR_TRAVERSE_DEFAULT_MODE;
    sel->traverse.direction = SELECTOR_TRAVERSE_DEFAULT_DIRECTION;
    sel->traverse.cross_module = SELECTOR_TRAVERSE_DEFAULT_CROSS_MODULE;
    sel->traverse.cross_fstree = SELECTOR_TRAVERSE_DEFAULT_CROSS_FSTREE;
    sel->hidden = SELECTOR_TRAVERSE_DEFAULT_HIDDEN;
    sel->nofollow = SELECTOR_TRAVERSE_DEFAULT_NOFOLLOW;
    selector_graph_fs(&sel->traverse);
  }
  ;

allocate-nodeset-selector
  : %empty
  {
    selector *sel;
    selector_alloc(SELECTOR_NODESET, &sel);
    llist_prepend(&PARSER->selector_stack, &sel->lln);
  }
  ;

selector-utterance
  : aggregate-selector
  /* single operation wrapped in default sequence selector */
  | allocate-sequence-selector operation-selector
  {
    selector *sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector *Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, &sel->lln);
  }
  /* default sequence selector -- used by request_parser, config_parser */
  | allocate-sequence-selector '[' selectors ']'
  ;

selectors
  : selectors selector
  {
    selector *sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector *Z = llist_first(&PARSER->selector_stack, selector, lln);
    llist_append(&Z->head, &sel->lln);
  }
  | selector
  {
    selector *sel = llist_shift(&PARSER->selector_stack, selector, lln);
    selector *Z = llist_first(&PARSER->selector_stack, selector, lln);
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
  | subset-selector
  ;

operation-selector
  : nodeset-selector
  | lookup-selector
  | path-selector
  | search-selector
  | traverse-selector
  | module-of-selector
  | fstree-of-selector
  ;

sequence-selector
  : allocate-sequence-selector SEQUENCE ':' '[' selectors ']'
  ;

union-selector
  : allocate-union-selector UNION ':' '[' selectors ']'
  ;

subset-selector
  : allocate-subset-selector SUBSET ':' '{' subset-mappings '}'
  ;

subset-mappings
  : subset-mappings-list
  | %empty
  ;

subset-mappings-list
  : subset-mappings-list subset-mapping
  | subset-mapping
  ;

subset-mapping
  : SORT ':' '[' sort-items ']'
  | LIMIT ':' uint16
  {
    TOP->subset.limit = $3;
  }
  ;

sort-items
  : sort-items sort-item
  | sort-item
  ;

sort-item
  : sort-spec
  {
    TOP->subset.sort.ranks_len++;
  }
  ;

sort-spec
  : ORDER ':' ASCENDING
  {
   TOP->subset.sort.ranks[TOP->subset.sort.ranks_len].order = SELECTOR_ORDER_ASCENDING;
  }
  | ORDER ':' DESCENDING
  {
   TOP->subset.sort.ranks[TOP->subset.sort.ranks_len].order = SELECTOR_ORDER_DESCENDING;
  }
  | sort-key
  {
    TOP->subset.sort.ranks[TOP->subset.sort.ranks_len].key = $1;
  }
  ;

sort-key
  : RUNTIME
  {
    $$ = SELECTOR_SORTKEY_RUNTIME;
  }
  | FORKTIME
  {
    $$ = SELECTOR_SORTKEY_FORKTIME;
  }
  ;

nodeset-selector
  : allocate-nodeset-selector selector-nodeset
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nodeset = $2;
  }
  ;

lookup-selector
  : allocate-lookup-selector LOOKUP ':' '{' lookup-mappings '}'
  ;

lookup-mappings
  : lookup-pattern-mapping lookup-namespace-mapping
  | lookup-expect-mapping lookup-pattern-mapping lookup-namespace-mapping
  | lookup-pattern-mapping lookup-expect-mapping lookup-namespace-mapping
  | lookup-pattern-mapping lookup-namespace-mapping lookup-expect-mapping
  | lookup-namespace-mapping lookup-pattern-mapping
  | lookup-expect-mapping lookup-namespace-mapping lookup-pattern-mapping
  | lookup-namespace-mapping lookup-expect-mapping lookup-pattern-mapping
  | lookup-namespace-mapping lookup-pattern-mapping lookup-expect-mapping
  ;

lookup-pattern-mapping
  : PATTERN ':' lookup-pattern
  {
    llist_first(&PARSER->selector_stack, selector, lln)->lookup.pattern = $3;
  }
  ;

lookup-namespace-mapping
  : NAMESPACE ':' unquoted-string
  {
    llist_first(&PARSER->selector_stack, selector, lln)->lookup.namespace = $3;
    llist_first(&PARSER->selector_stack, selector, lln)->lookup.namespace_len = strlen($3);
  }

lookup-expect-mapping
  : EXPECT ':' expect-type
  {
    llist_first(&PARSER->selector_stack, selector, lln)->expect = $3;
  }
  ;

module-of-selector
  : allocate-module-of-selector MODULE_OF
  ;

fstree-of-selector
  : allocate-fstree-of-selector FSTREE_OF
  ;

path-selector
  : allocate-path-selector PATH ':' '{' path-mappings '}'
  ;

path-mappings
  : path-mappings path-mapping
  | path-mapping
  ;

path-mapping
  : PATTERN ':' path-pattern
  {
    llist_first(&PARSER->selector_stack, selector, lln)->path.pattern = $3;
  }
  | NOFOLLOW ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nofollow = $3;
  }
  | HIDDEN ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->hidden = $3;
  }
  | EXPECT ':' expect-type
  {
    llist_first(&PARSER->selector_stack, selector, lln)->expect = $3;
  }
  ;

search-selector
  : allocate-search-selector SEARCH ':' '{' search-mappings '}'
  ;

search-mappings
  : search-mappings search-mapping
  | search-mapping
  ;

search-mapping
  : PATTERN ':' search-pattern
  {
    llist_first(&PARSER->selector_stack, selector, lln)->search.pattern = $3;
  }
  ;

expect-type
  : ZERO
  {
    $$ = SELECTOR_EXPECT_ZERO;
  }
  | ONE
  {
    $$ = SELECTOR_EXPECT_ONE;
  }
  | ANY
  {
    $$ = SELECTOR_EXPECT_ANY;
  }
  | NONZERO
  {
    $$ = SELECTOR_EXPECT_NONZERO;
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
  | VISIT ':' visit-type
  | ORDER ':' traverse-order
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.mode = $3;
  }
  | DIRECTION ':' traverse-direction
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.direction = $3;
  }
  | MIN_DISTANCE ':' uint16
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.criteria.min_depth = $3;
  }
  | MAX_DISTANCE ':' uint16
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.criteria.max_depth = $3;
  }
  | EXHAUSTIVE ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.exhaustive = $3;
  }
  | HIDDEN ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->hidden = $3;
  }
  | CROSS_MODULE ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.cross_module = $3;
  }
  | CROSS_FSTREE ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->traverse.cross_fstree = $3;
  }
  | NOFOLLOW ':' BOOL
  {
    llist_first(&PARSER->selector_stack, selector, lln)->nofollow = $3;
  }
  ;

graph-type
  : FS        { selector_graph_fs(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | DEPENDS   { selector_graph_depends(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | SPECIAL   { selector_graph_special(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | RECORDS   { selector_graph_records(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | USES      { selector_graph_uses(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  ;

visit-type
  : BPE        { selector_visit_bpe(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | CLIENT     { selector_visit_client(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | CONFIGSET  { selector_visit_special_configset(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | DIRECTORY  { selector_visit_directory(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | FORMULA    { selector_visit_special_formula(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | FSTREE     { selector_visit_fstree(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | MODULE     { selector_visit_special_module(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | PLUGIN     { selector_visit_special_plugin(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | RECORD     { selector_visit_record(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | RMA        { selector_visit_rma(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | RULE       { selector_visit_rule(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | RULESET    { selector_visit_special_ruleset(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | VARSET     { selector_visit_special_varset(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }

  | PRIMARY    { selector_visit_primary(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  | INVALID    { selector_visit_invalid(&llist_first(&PARSER->selector_stack, selector, lln)->traverse); }
  ;

traverse-direction
  : UP   { $$ = MORIA_TRAVERSE_UP; }
  | DOWN { $$ = MORIA_TRAVERSE_DOWN; }
  ;

traverse-order
  : PREORDER  { $$ = MORIA_TRAVERSE_PRE; }
  | POSTORDER { $$ = MORIA_TRAVERSE_POST; }
  ;

selector-nodeset
  : SOURCE  { $$ = SELECTOR_NODESET_SOURCE; }
  | SOURCES { $$ = SELECTOR_NODESET_SOURCES; }
  | TARGET  { $$ = SELECTOR_NODESET_TARGET; }
  | TARGETS { $$ = SELECTOR_NODESET_TARGETS; }
  | MODULE  { $$ = SELECTOR_NODESET_MODULE; }
  | MODULES { $$ = SELECTOR_NODESET_MODULES; }
  | ANCHOR  { $$ = SELECTOR_NODESET_ANCHOR; }
  ;

lookup-pattern
  : LOOKUP_PATTERN
  ;

path-pattern
  : PATH_PATTERN
  ;

search-pattern
  : SEARCH_PATTERN
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

unquoted-string
  : strtoken
  {
    $$ = 0;
    ixstrndup(&$$, @1.s, @1.l);
  }
  ;

strtoken
  : STR
  | MODULE
  | MODULES
  ;
