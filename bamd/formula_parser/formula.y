/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <stdint.h>
  #include <string.h>

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/rbtree.h"

  #include "formula_parser.internal.h"
  #include "formula.h"
  #include "bacon/make.h"
  #include "exec_value.h"
  #include "selector.h"
  #include "pattern.h"

  #include "macros.h"

  struct lookup_pattern;
}

%code top {
  int formula_yylex(void *, void*, void*);
  #define PARSER containerof(parser, formula_parser, yyu)
  #define FORMULA_YYLTYPE yyu_location

  #include "formula_parser.internal.h"
  #include "bacon/value.h"

  static void replace(formula_parser *parser, struct bacon_value ** dst, struct bacon_value *val)
  {
    bacon_value_free(parser->bacon_parser, *dst);
    *dst = val;
  }
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {formula_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;

  bacon_value *value;
  struct lookup_pattern *lookup_pattern;

  // enums
  fsent_property node_property;

  // delegated
  struct selector *selector;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'

/* terminals */
%token            STR           300 "unquoted-string"
%token <yyu.u8>   CREF          301 "character-escape"
%token <yyu.u8>   HREF          302 "hex-escape"
%token <yyu.b>    BOOL          303 "boolean"
%token <yyu.f>    FLOAT         304 "float"
%token <yyu.t>    VARIABLE      305 "variable"
%token <yyu.umax> UINTMAX8      306
%token <yyu.umax> UINTMAX16     307
%token <yyu.umax> UINTMAX32     308
%token <yyu.umax> UINTMAX64     309
%token <yyu.umax> HEX8          310
%token <yyu.umax> HEX16         311
%token <yyu.umax> HEX32         312
%token <yyu.umax> HEX64         313
%token <yyu.imax> INTMIN8       314
%token <yyu.imax> INTMIN16      315
%token <yyu.imax> INTMIN32      316
%token <yyu.imax> INTMIN64      317
%token <yyu.imax> INTMAX8       318
%token <yyu.imax> INTMAX16      319
%token <yyu.imax> INTMAX32      320
%token <yyu.imax> INTMAX64      321

/* terminals */
%token <selector> SELECTOR
%token <lookup_pattern> LOOKUP_PATTERN "lookup-pattern"

/* nonterminals */
%type <yyu.umax> posint_ugroup
%type <yyu.imax> posint_igroup
%type <yyu.imax> negint_igroup

/* nonterminals */
%type <lookup_pattern> lookup-pattern

%destructor { pattern_free(&$$->pattern); } <lookup_pattern>

%token
  ARGS          "args"
  ENVS          "envs"
  FILETOKEN     "file"
  PROPERTY      "property"
  PREPEND       "prepend"
  SELECT        "select"
  LOOKUP        "lookup"
  PATTERN       "pattern"
  NAMESPACE     "namespace"

%token
  ABSDIR        "absdir"
  ABSPATH       "abspath"
  BASE          "base"
  EXT           "ext"
  NAME          "name"
  RELDIR        "reldir"
  ID            "id"
  RELPATH       "relpath"
  SEQUENCE      "sequence"
  SUFFIX        "suffix"

/* nonterminals */
%type <value> string quote_words quote_word quoted_string unquoted_string
%type <value> primitive boolean float negint posint variable
%type <value> operation-list operation-list-item operation sequence-operation lookup-operation
%type <value> arg-values arg-value
%type <value> env-var env-var-value
%type <value> file-value
%type <value> env-vars
%type <value> value scalar aggregate mapping
%type <value> set elements
%type <value> list items
%type <node_property> node-property

%destructor { bacon_value_free(PARSER->bacon_parser, $$); } <value>
%destructor { selector_free($$); } <selector>

%%
utterance
  : sections
  | %empty
  ;

sections
  : sections section
  | section
  ;

section
  : envs-section
  | args-section
  | file-section
  ;

file-section
  : FILETOKEN ':' file-value
  {
    replace(PARSER, &PARSER->file, $3);
  }
  ;

file-value
  : '{' lookup-operation '}'
  {
    $$ = $2;
  }
  ;

args-section
  : ARGS ':' '[' arg-values ']'
  {
    replace(PARSER, &PARSER->args, $4);
  }
  ;

arg-values
  : arg-values arg-value
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | arg-value
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

arg-value
  : primitive
  | sequence-operation
  | lookup-operation
  ;

envs-section
  : ENVS ':' '{' env-vars '}'
  {
    replace(PARSER, &PARSER->envs, $4);
  }
  ;

env-vars
  : env-vars env-var
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | env-var
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

env-var
  : unquoted_string ':' env-var-value
  {
    bacon_mapping_mk(PARSER->bacon_parser, &@$, &$$, $1, $3);
  }
  ;

env-var-value
  : '{' sequence-operation '}'
  {
    $$ = $2;
  }
  | '{' lookup-operation '}'
  {
    $$ = $2;
  }
  | value
  {
    $$ = $1;
  }
  ;

primitive
  : boolean
  | float
  | negint
  | posint
  | variable
  | string
  ;

list
  : '[' items ']'
  {
    $$ = $2;
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

mapping
  : primitive ':' value
  {
    bacon_mapping_mk(PARSER->bacon_parser, &@$, &$$, $1, $3);
  }
  ;

operation-list
  : operation-list operation-list-item
  {
    exec_value_sequence_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | operation-list-item
  {
    exec_value_sequence_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

operation-list-item
  : operation
  | primitive
  ;

sequence-operation
  : SEQUENCE ':' '[' operation-list ']'
  {
    $$ = $4;
  }
  ;

operation
  : SELECT ':' SELECTOR
  {
    exec_value_select_mk(PARSER->bacon_parser, &@$, &$$, $3);
  }
  | PROPERTY ':' node-property
  {
    exec_value_property_mk(PARSER->bacon_parser, &@$, &$$, $3);
  }
  | PREPEND ':' primitive
  {
    exec_value_prepend_mk(PARSER->bacon_parser, &@$, &$$, $3);
  }
  ;

lookup-operation
  : LOOKUP ':' '{' PATTERN ':' lookup-pattern NAMESPACE ':' STR '}'
  {
    exec_value_lookup_mk(PARSER->bacon_parser, &@$, &$$, $6, @9.s, @9.l);
  }
  | LOOKUP ':' '{' NAMESPACE ':' STR PATTERN ':' lookup-pattern '}'
  {
    exec_value_lookup_mk(PARSER->bacon_parser, &@$, &$$, $9, @6.s, @6.l);
  }
  | LOOKUP ':' '{' PATTERN ':' lookup-pattern '}'
  {
    exec_value_lookup_mk(PARSER->bacon_parser, &@$, &$$, $6, 0, 0);
  }
  ;

node-property
  : NAME         { $$ = FSENT_PROPERTY_NAME; }
  | EXT          { $$ = FSENT_PROPERTY_EXT; }
  | SUFFIX       { $$ = FSENT_PROPERTY_SUFFIX; }
  | BASE         { $$ = FSENT_PROPERTY_BASE; }
  | ABSPATH      { $$ = FSENT_PROPERTY_ABSPATH; }
  | ABSDIR       { $$ = FSENT_PROPERTY_ABSDIR; }
  | RELPATH      { $$ = FSENT_PROPERTY_RELPATH; }
  | RELDIR       { $$ = FSENT_PROPERTY_RELDIR; }
  | ID           { $$ = FSENT_PROPERTY_ID; }
  ;

string
  : unquoted_string
  | quoted_string
  ;

unquoted_string
  : word
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

word
  : STR
  | '/'
  | '-'
  | '+'
  | '='
  | ','
  ;

quoted_string
  : '"' quote_words '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$ = 0;
  }
  ;

quote_words
  : quote_words quote_word
  {
    bacon_string_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
    bacon_value_free(PARSER->bacon_parser, $2);
  }
  | quote_word
  ;

quote_word
  : STR
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  | CREF
  {
    bacon_string_mkc(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  | HREF
  {
    bacon_string_mkc(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

boolean
  : BOOL
  {
    bacon_boolean_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

posint
  : posint_ugroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  | posint_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

posint_ugroup
  : UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  | HEX8
  | HEX16
  | HEX32
  | HEX64
  ;

posint_igroup
  : INTMAX8
  | INTMAX16
  | INTMAX32
  | INTMAX64
  ;

negint
  : negint_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

negint_igroup
  : INTMIN8
  | INTMIN16
  | INTMIN32
  | INTMIN64
  ;

float
  : FLOAT
  {
    bacon_float_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

variable
  : VARIABLE
  {
    bacon_variable_mkw(PARSER->bacon_parser, &@$, &$$, $1.s, $1.l);
  }
  ;

lookup-pattern
  : LOOKUP_PATTERN
  ;
