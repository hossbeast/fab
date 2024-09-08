/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <string.h>

  #include "types.h"

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/list.h"
  #include "valyria/llist.h"
  #include "bacon/make.h"

  #include "rule.h"
  #include "import.h"
  #include "anchor.h"
  #include "ruleset_parser.h"
  #include "module.h"
  #include "pattern.h"
  #include "block.h"

  #include "macros.h"

  struct pattern;
  struct bacon_value;
}

%code top {
  int ruleset_yylex(void*, void*, void*);
  #define PARSER ((ruleset_parser*)parser)
  #define RULESET_YYLTYPE yyu_location

  struct yyu_location;
  struct yyu_parser;
  static void ruleset_yyerror(struct yyu_location* loc, void * scanner, struct yyu_parser * pp, char const * err)
    __attribute__((weakref("yyu_grammar_error")));
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {ruleset_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval  yyu;

  struct rule * rule;
  struct import *import;
  struct anchor * anchor;
  struct pattern * pattern;
  struct bacon_value * value;
  struct bacon_expr * expr;
  bacon_operation oper;

  // enums
  edge_kind graph_type;
}

/* tokens */
%token '[' ']'
%token '<' '>'
%token '(' ')'
%token ':'
%token '$'

/* terminals */
%token            STR        104 "string"
%token <yyu.u8>   CREF       105 "character-escape"
%token <yyu.u8>   HREF       106 "hex-escape"
%token <yyu.b>    BOOL           "boolean"
%token <yyu.f>    FLOAT          "float"
%token <yyu.b>    VARIABLE       "variable"
%token <yyu.i128> UINTMAX8
%token <yyu.i128> UINTMAX16
%token <yyu.i128> UINTMAX32
%token <yyu.i128> UINTMAX64
%token <yyu.i128> HEX8
%token <yyu.i128> HEX16
%token <yyu.i128> HEX32
%token <yyu.i128> HEX64
%token <yyu.i128> INTMIN8
%token <yyu.i128> INTMIN16
%token <yyu.i128> INTMIN32
%token <yyu.i128> INTMIN64
%token <yyu.i128> INTMAX8
%token <yyu.i128> INTMAX16
%token <yyu.i128> INTMAX32
%token <yyu.i128> INTMAX64
%type <yyu.i128> intval

%token
 AS         "as"
 DEPENDS    "depends"
 FS         "fs"
 RULE       "rule"
 DASHES     "--"
 STARBOX    "[*]"
 IF         "if"
 NAME       "name"
 USE        "use"
 BIND       "bind"
 REQUIRE    "require"
 ANCHOR     "anchor"
 SEARCH     "search"
 GENERATE   "generate"
 VARSETS    "varsets"
 RULESETS   "rulesets"

%token
 <pattern> SEARCH_PATTERN
 <pattern> GENERATE_PATTERN
 <pattern> REFERENCE_PATTERN
 <pattern> LOOKUP_PATTERN
 <pattern> MATCH_PATTERN

%type
 <pattern> generate-pattern

%token
 LOGAND  "&&"
 LOGOR   "||"
 SHL     "<<"
 SHR     ">>"

%type <yyu.t> string quoted-string unquoted-string strpart strparts
%type <rule> rule rule-form
%type <rule> bind
%type <import> import
%type <anchor> anchor
%type <expr> if-statement
%type <yyu.u16> connector bind-connector

%destructor { pattern_free($$); } <pattern>

/* nonterminals */
%type <value> value
%type <value> aggregate
%type <value> list items
%type <value> set elements
%type <value> scalar primitive
%type <value> mapping boolean float integer strval variable
%type <oper> unary_operator multiplicative_operator multiplicative_associative_operator additive_operator additive_associative_operator shift_operator relational_operator equality_operator
%type <expr> expr logical_or_expr logical_and_expr inclusive_or_expr exclusive_or_expr and_expr
%type <expr> equality_expr relational_expr shift_expr additive_expr multiplicative_expr unary_expr operand

%%
utterance
  : start open-block declarations definitions close-block
  ;

start
  : %empty
  {
    llist_init(&PARSER->blocks);
    block_alloc(&PARSER->block);
    llist_push(&PARSER->blocks, &PARSER->block->lln);
  }
  ;

declarations
  : declaration-list
  | %empty
  ;

declaration-list
  : declaration-list declaration
  | declaration
  ;

declaration
  : anchor
  | varsets
  | rulesets
  ;

anchor
  : ANCHOR LOOKUP_PATTERN
  {
    anchor_release(PARSER->anchor);
    anchor_mk(&PARSER->anchor, $2);
  }
  ;

varsets
  : VARSETS LOOKUP_PATTERN
  {
    llist_append(&PARSER->varsets, &$2->lln);
  }
  ;

rulesets
  : RULESETS LOOKUP_PATTERN
  {
    llist_append(&PARSER->rulesets, &$2->lln);
  }
  ;

definitions
  : definition-list
  | %empty
  ;

definition-list
  : definition-list definition
  | definition
  ;

definition
  : compound-block
  | rule
  {
    llist_append(&PARSER->block->rules, &$1->lln);
  }
  | bind
  {
    llist_append(&PARSER->block->rules, &$1->lln);
  }
  | import
  {
    llist_append(&PARSER->block->imports, &$1->lln);
  }
  ;

compound-block
  : open-block '{' if-statements definitions '}' close-block
  ;

if-statements
  : if-statement-list
  | %empty
  ;

if-statement-list
  : if-statement-list if-statement
  | if-statement
  ;

if-statement
  : IF '(' expr ')'
  {
    llist_append(&PARSER->block->exprs, &$3->lln);
  }
  ;

import
  /* uses */
  : USE LOOKUP_PATTERN AS string
  {
    import_uses_mk(&$$, $2, $4.s, $4.l, 0);
  }
  | USE LOOKUP_PATTERN AS
  {
    import_uses_mk(&$$, $2, 0, 0, IMPORT_FLAGS_NONAME);
  }
  | USE LOOKUP_PATTERN
  {
    import_uses_mk(&$$, $2, 0, 0, 0);
  }
  /* requires */
  | REQUIRE LOOKUP_PATTERN
  {
    import_requires_mk(&$$, $2, 0, 0, 0);
  }

bind
  /* bind target => attachment */
  : BIND bind-bacon LOOKUP_PATTERN bind-connector REFERENCE_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, 0, 0, 0, $3, $5, 0, &PARSER->bacon, $4, &@$);
  }
  ;

bind-connector
  : '=' '>'                  /* => */
  {
    $$ = RULE_BIND_ONE;
  }
  | '=' '>' STARBOX          /* => [*] */
  {
    $$ = RULE_BIND_MANY;
  }
  ;

bind-bacon
  : { memset(&PARSER->bacon, 0, sizeof(PARSER->bacon)); } bind-bacon-mapping-list
  ;

bind-bacon-mapping-list
  : '{' bind-bacon-mappings '}'
  | %empty
  ;

bind-bacon-mappings
  : bind-bacon-mappings bind-bacon-mapping
  | bind-bacon-mapping
  ;

bind-bacon-mapping
  : NAME ':' strtok-list
  {
    PARSER->bacon.name_len = memncpy(PARSER->bacon.name, sizeof(PARSER->bacon.name), @3.s, @3.l);
  }
  ;

open-block
  : %empty
  {
    block_alloc(&PARSER->block);
    llist_push(&PARSER->blocks, &PARSER->block->lln);
  }
  ;

close-block
  : %empty
  {
    block *prev;
    prev = PARSER->block;
    llist_pop(&PARSER->blocks, block, lln);
    PARSER->block = llist_last(&PARSER->blocks, block, lln);
    llist_push(&PARSER->block->blocks, &prev->lln);
  }
  ;

rule-init
  : %empty { memset(&PARSER->bacon, 0, sizeof(PARSER->bacon)); }
  ;

rule-start
  : RULE rule-bacon
  ;

search-start
  : RULE rule-bacon
  | RULE rule-bacon SEARCH
  | SEARCH rule-bacon
  ;

generate-pattern
  : GENERATE GENERATE_PATTERN
  {
    $$ = $2;
  }
  | GENERATE_PATTERN
  ;

rule
  : rule-init rule-form
  {
    $$ = $2;
  }
  ;

rule-form
  /* (search) antecedent(s), (generate) consequent(s) : (reference) formula */
  : search-start SEARCH_PATTERN connector generate-pattern ':' REFERENCE_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, $4, $6, 0, 0, 0, &PARSER->bacon, $3, &@$);
  }
  | search-start SEARCH_PATTERN connector generate-pattern
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, $4, 0, 0, 0, 0, &PARSER->bacon, $3, &@$);
  }

  /* bind */
  | search-start SEARCH_PATTERN '=' '>' STARBOX generate-pattern
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, 0, 0, 0, $6, 0, &PARSER->bacon, RULE_BIND_MANY, &@$);
  }

  /* search / search */
  | search-start SEARCH_PATTERN STARBOX '-' '>' STARBOX SEARCH SEARCH_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, 0, 0, 0, 0, $8, &PARSER->bacon, RULE_SEARCH_SEARCH | RULE_LTR, &@$);
  }
  | search-start SEARCH_PATTERN STARBOX '<' '0' STARBOX SEARCH SEARCH_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, 0, 0, 0, 0, $8, &PARSER->bacon, RULE_SEARCH_SEARCH | RULE_RTL, &@$);
  }

  /* search -- */
  | search-start SEARCH_PATTERN DASHES ':' REFERENCE_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, 0, $5, 0, 0, 0, &PARSER->bacon, RULE_ZERO_TO_ONE | RULE_RTL, &@$);
  }
  /* search [*] -- */
  | search-start SEARCH_PATTERN STARBOX DASHES ':' REFERENCE_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, $2, 0, $6, 0, 0, 0, &PARSER->bacon, RULE_ZERO_TO_MANY | RULE_RTL, &@$);
  }
  /* -- generate */
  | rule-start DASHES generate-pattern ':' REFERENCE_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, 0, $3, $5, 0, 0, 0, &PARSER->bacon, RULE_ZERO_TO_ONE | RULE_LTR, &@$);
  }
  /* -- [*] generate */
  | rule-start DASHES STARBOX generate-pattern ':' REFERENCE_PATTERN
  {
    $$ = rule_mk(PARSER->g, PARSER->mod, PARSER->rules, 0, $4, $6, 0, 0, 0, &PARSER->bacon, RULE_ZERO_TO_MANY | RULE_LTR, &@$);
  }
  ;

rule-bacon
  : '{' rule-bacon-mappings '}'
  | %empty
  ;

rule-bacon-mappings
  : rule-bacon-mappings rule-bacon-mapping
  | rule-bacon-mapping
  ;

rule-bacon-mapping
  : NAME ':' strtok-list
  {
    PARSER->bacon.name_len = memncpy(PARSER->bacon.name, sizeof(PARSER->bacon.name), @3.s, @3.l);
  }
  ;

connector
  : '<' '-'                  /* <- */
  {
    $$ = RULE_RTL | RULE_ONE_TO_ONE;
  }
  | '-' '>'                  /* -> */
  {
    $$ = RULE_LTR | RULE_ONE_TO_ONE;
  }
  | '<' '-' STARBOX          /* <- [*] */
  {
    $$ = RULE_RTL | RULE_MANY_TO_ONE;
  }
  | '-' '>' STARBOX          /* -> [*] */
  {
    $$ = RULE_LTR | RULE_ONE_TO_MANY;
  }
  | STARBOX '<' '-'          /* [*] <- */
  {
    $$ = RULE_RTL | RULE_ONE_TO_MANY;
  }
  | STARBOX '-' '>'          /* [*] -> */
  {
    $$ = RULE_LTR | RULE_MANY_TO_ONE;
  }
  | STARBOX '<' '-' STARBOX  /* [*] <- [*] */
  {
    $$ = RULE_RTL | RULE_MANY_TO_MANY;
  }
  | STARBOX '-' '>' STARBOX  /* [*] -> [*] */
  {
    $$ = RULE_LTR | RULE_MANY_TO_MANY;
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
    memset(&$$, 0, sizeof($$));
  }
  ;

strparts
  : strparts strpart
  {
    $$ = $1;
    ixstrcat(&$$.s, $2.s);
    $$.l += $2.l;
  }
  | strpart
  ;

strpart
  : STR
  {
    $$.s = 0;
    ixstrndup(&$$.s, @1.s, @1.l);
    $$.l = @1.l;
  }
  | CREF
  {
    $$.s = 0;
    ixstrndup(&$$.s, (char*)&$1, 1);
    $$.l = 1;
  }
  | HREF
  {
    $$.s = 0;
    ixstrndup(&$$.s, (char*)&$1, 1);
    $$.l = 1;
  }
  ;

strtok-list
  : strtok-list STRTOK
  | STRTOK
  ;

STRTOK
  : STR
  | intval
  ;

expr
  : logical_or_expr
  ;

logical_or_expr
  : logical_and_expr
  | logical_or_expr LOGOR logical_and_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, BACON_LOGOR, $1, $3);
  }
  ;

logical_and_expr
  : inclusive_or_expr
  | logical_and_expr LOGAND inclusive_or_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, BACON_LOGAND, $1, $3);
  }
  ;

inclusive_or_expr
  : exclusive_or_expr
  | inclusive_or_expr '|' exclusive_or_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, BACON_BITOR, $1, $3);
  }
  ;

exclusive_or_expr
  : and_expr
  | exclusive_or_expr '^' and_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, BACON_BITXOR, $1, $3);
  }
  ;

and_expr
  : equality_expr
  | and_expr '&' equality_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, BACON_BITAND, $1, $3);
  }
  ;

equality_expr
  : relational_expr
  | equality_expr equality_operator relational_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  ;

relational_expr
  : shift_expr
  | relational_expr relational_operator shift_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  ;

shift_expr
  : additive_expr
  | shift_expr shift_operator additive_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  ;

additive_expr
  : multiplicative_expr
  | additive_expr additive_associative_operator multiplicative_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  | additive_expr additive_operator multiplicative_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  ;

multiplicative_expr
  : unary_expr
  | multiplicative_expr multiplicative_associative_operator unary_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  | multiplicative_expr multiplicative_operator unary_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $2, $1, $3);
  }
  ;

unary_expr
  : operand
  | unary_operator unary_expr
  {
    bacon_expr_mk(PARSER->bacon_parser, &@$, &$$, $1, $2, NULL);
  }
  ;

operand
  : '(' expr ')'
  {
    $$ = $2;
  }
  | value
  {
    bacon_expr_wrap(PARSER->bacon_parser, $1, &$$);
  }
  ;

unary_operator
  : '~'
  {
    $$ = BACON_BITINV;
  }
  | '!'
  {
    $$ = BACON_LOGINV;
  }
  ;

multiplicative_operator
  : '/'
  {
    $$ = BACON_DIV;
  }
  | '%'
  {
    $$ = BACON_MOD;
  }
  ;

multiplicative_associative_operator
  : '*'
  {
    $$ = BACON_MUL;
  }
  ;

additive_operator
  : '-'
  {
    $$ = BACON_SUB;
  }
  ;

additive_associative_operator
  : '+'
  {
    $$ = BACON_ADD;
  }
  ;

shift_operator
  : SHL
  {
    $$ = BACON_SHL;
  }
  | SHR
  {
    $$ = BACON_SHR;
  }
  ;

relational_operator
  : '>' '='
  {
    $$ = BACON_GE;
  }
  | '>'
  {
    $$ = BACON_GT;
  }
  | '<' '='
  {
    $$ = BACON_LE;
  }
  | '<'
  {
    $$ = BACON_LT;
  }
  ;

equality_operator
  : '=' '='
  {
    $$ = BACON_EQ;
  }
  | '!' '='
  {
    $$ = BACON_NE;
  }
  ;

value
  : scalar
  | aggregate
  ;

scalar
  : primitive
  | mapping
  ;

aggregate
  : list
  | set
  ;

primitive
  : boolean
  | float
  | integer
  | strval
  | variable
  ;

strval
  : STR
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

mapping
  : primitive ':' value
  {
    bacon_mapping_mk(PARSER->bacon_parser, &@$, &$$, $1, $3);
  }
  ;

list
  : '[' items ']'
  {
    $$ = $2;
  }
  | '[' ']'
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, 0, &$$, 0);
  }
  ;

items
  : items value
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | value
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

set
  : '{' elements '}'
  {
    $$ = $2;
  }
  | '{' '}'
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, 0, &$$, 0);
  }
  ;

elements
  : elements value
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | value
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

boolean
  : BOOL
  {
    bacon_boolean_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

integer
  : intval
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

intval
  : INTMIN8
  | INTMIN16
  | INTMIN32
  | INTMIN64
  | INTMAX8
  | INTMAX16
  | INTMAX32
  | INTMAX64
  | UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  | HEX8
  | HEX16
  | HEX32
  | HEX64
  ;

float
  : FLOAT
  {
    bacon_float_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

variable
  : '$' STR
  {
    bacon_variable_mkw(PARSER->bacon_parser, &@$, &$$, @2.s, @2.l);
  }
  ;
