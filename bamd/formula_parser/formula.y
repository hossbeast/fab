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
  #include "formula_value.internal.h"
  #include "selector.h"

  #include "macros.h"
}

%code top {
  int formula_yylex(void *, void*, void*);
  #define PARSER containerof(parser, formula_parser, yyu)
  #define FORMULA_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {formula_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;

  formula_value * value;

  // enums
  fsent_property node_property;

  // delegated
  struct selector * selector;
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

%type <yyu.umax> posint_ugroup
%type <yyu.imax> posint_igroup
%type <yyu.imax> negint_igroup

/* terminals */
%token <selector> SELECTOR

%token
  ARGS          "args"
  ENVS          "envs"
  FILETOKEN     "file"
  PROPERTY      "property"
  PREPEND       "prepend"
  SELECT        "select"
  PATH_SEARCH   "path-search"

%token
  ABSDIR        "absdir"
  ABSPATH       "abspath"
  BASE          "base"
  EXT           "ext"
  NAME          "name"
  RELDIR        "reldir"
  RELPATH       "relpath"
  SEQUENCE      "sequence"
  SUFFIX        "suffix"

/* nonterminals */
%type <value> string quote_words quote_word quoted_string unquoted_string
%type <value> primitive boolean float negint posint variable
%type <value> operation-list operation-list-item operation sequence-operation path-search-operation
%type <value> arg-values arg-value
%type <value> env-var env-var-value
%type <value> file-value
%type <value> env-vars
%type <node_property> node-property

%destructor { formula_value_free($$); } <value>
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
    PARSER->file = $3;
  }
  ;

file-value
  : primitive
  | '{' path-search-operation '}'
  {
    $$ = $2;
  }
  ;

args-section
  : ARGS ':' '[' arg-values ']'
  {
    PARSER->args = $4;
  }
  ;

arg-values
  : arg-values arg-value
  {
    formula_value_list_mkv(&@$, $1, &$$, $2);
  }
  | arg-value
  {
    formula_value_list_mkv(&@$, 0, &$$, $1);
  }
  ;

arg-value
  : primitive
  | sequence-operation
  ;

envs-section
  : ENVS ':' '{' env-vars '}'
  {
    PARSER->envs = $4;
  }
  ;

env-vars
  : env-vars env-var
  {
    formula_value_set_mkv(&@$, $1, &$$, $2);
  }
  | env-var
  {
    formula_value_set_mkv(&@$, 0, &$$, $1);
  }
  ;

env-var
  : STR ':' env-var-value
  {
    formula_value_mapping_mk(&@$, &$$, @1.s, @1.l, $3);
  }
  ;

env-var-value
  : primitive
  | '[' operation-list ']'
  {
    $$ = $2;
  }
  | '{' path-search-operation '}'
  {
    $$ = $2;
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

operation-list
  : operation-list operation-list-item
  {
    formula_value_sequence_mkv(&@$, $1, &$$, $2);
  }
  | operation-list-item
  {
    formula_value_sequence_mkv(&@$, 0, &$$, $1);
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
    formula_value_select_mk(&@$, &$$, $3);
  }
  | PROPERTY ':' node-property
  {
    formula_value_property_mk(&@$, &$$, $3);
  }
  | PREPEND ':' primitive
  {
    formula_value_prepend_mk(&@$, &$$, $3);
  }
  ;

path-search-operation
  : PATH_SEARCH ':' primitive
  {
    formula_value_path_search_mk(&@$, &$$, $3);
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
  ;

string
  : unquoted_string
  | quoted_string
  ;

unquoted_string
  : word
  {
    formula_value_string_mkw(&@$, 0, &$$, @1.s, @1.l);
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
    formula_value_string_mkv(&@$, $1, &$$, $2);
    formula_value_free($2);
  }
  | quote_word
  ;

quote_word
  : STR
  {
    formula_value_string_mkw(&@$, 0, &$$, @1.s, @1.l);
  }
  | CREF
  {
    formula_value_string_mkc(&@$, 0, &$$, $1);
  }
  | HREF
  {
    formula_value_string_mkc(&@$, 0, &$$, $1);
  }
  ;

boolean
  : BOOL
  {
    formula_value_boolean_mk(&@$, &$$, $1);
  }
  ;

posint
  : posint_ugroup
  {
    formula_value_posint_mk(&@$, &$$, $1);
  }
  | posint_igroup
  {
    formula_value_posint_mk(&@$, &$$, $1);
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
    formula_value_negint_mk(&@$, &$$, $1);
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
    formula_value_float_mk(&@$, &$$, $1);
  }
  ;

variable
  : VARIABLE
  {
    formula_value_variable_mk(&@$, &$$, $1.s, $1.l);
  }
  ;
