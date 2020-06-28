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

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/rbtree.h"
  #include "valyria/chain.h"

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

  #include "xapi.h"
  #include "valyria/rbtree.h"
  #include "xlinux/xstdlib.h"

  static xapi rbtree_create(rbtree ** restrict rbt)
  {
    enter;

    fatal(xmalloc, rbt, sizeof(**rbt));
    rbtree_init(*rbt);

    finally : coda;
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
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;

  formula_value * value;
  rbtree * set;

  // enums
  node_property node_property;

  // delegated
  struct selector * selector;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':' '='

/* terminals */
%token            STR           "string"
%token <yyu.u8>   CREF          "character-escape"
%token <yyu.u8>   HREF          "hex-escape"
%token <yyu.b>    BOOL          "boolean"
%token <yyu.f>    FLOAT         "float"
%token <yyu.t>    VARIABLE      "variable"
%token <yyu.umax> UINTMAX8
%token <yyu.umax> UINTMAX16
%token <yyu.umax> UINTMAX32
%token <yyu.umax> UINTMAX64
%token <yyu.umax> HEX8
%token <yyu.umax> HEX16
%token <yyu.umax> HEX32
%token <yyu.umax> HEX64
%token <yyu.imax> INTMIN8
%token <yyu.imax> INTMIN16
%token <yyu.imax> INTMIN32
%token <yyu.imax> INTMIN64
%token <yyu.imax> INTMAX8
%token <yyu.imax> INTMAX16
%token <yyu.imax> INTMAX32
%token <yyu.imax> INTMAX64

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
  VARIANT       "variant"
  BASE          "base"
  EXT           "ext"
  FSROOT        "fsroot"
  NAME          "name"
  RELDIR        "reldir"
  RELPATH       "relpath"
  SEQUENCE      "sequence"
  SUFFIX        "suffix"

%token <yyu.u16>
  BM_SOURCE     "bm_source"
  BM_SOURCES    "bm_sources"
  BM_TARGET     "bm_target"
  BM_TARGETS    "bm_targets"
  BM_VARIANT    "bm_variant"

/* nonterminals */
%type <yyu.u16> sysvar-token
%type <yyu.s> string quoted-string unquoted-string strpart strparts
%type <value> primitive boolean float negint posint variable sysvar
%type <value> operation-list operation-list-item operation sequence-operation path-search-operation
%type <value> arg-values arg-value
%type <value> env-var env-var-value
%type <value> file-value
%type <set> env-vars
%type <node_property> node-property

%destructor { formula_value_free($$); } <value>
%destructor { selector_free($$); } <selector>

%%
utterance
  : sections
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
    YFATAL(formula_value_list_mk, &@$, &PARSER->args, $4);
  }
  ;

arg-values
  : arg-values arg-value
  {
    $$ = chain_add($1, $2, chn);
  }
  | arg-value
  {
    $$ = $1;
    chain_init_node(&$$->chn);
  }
  ;

arg-value
  : primitive
  | sequence-operation
  ;

envs-section
  : ENVS ':' '{' env-vars '}'
  {
    YFATAL(formula_value_set_mk, &@$, &PARSER->envs, $4);
  }
  ;

env-vars
  : env-vars env-var
  {
    $$ = $1;
    rbtree_put($$, $2, rbn, fmlval_rbn_cmp);
  }
  | env-var
  {
    YFATAL(rbtree_create, &$$);
    rbtree_put($$, $1, rbn, fmlval_rbn_cmp);
  }
  ;

env-var
  : STR ':' env-var-value
  {
    YFATAL(formula_value_mapping_mk, &@$, &$$, @1.s, @1.l, $3);
  }
  ;

env-var-value
  : primitive
  | '[' operation-list ']'
  {
    YFATAL(formula_value_sequence_mk, &@$, &$$, $2);
  }
  ;

primitive
  : boolean
  | float
  | negint
  | posint
  | variable
  | sysvar
  | string
  {
    YFATAL(formula_value_string_mk, &@$, &$$, $1);
  }
  ;

operation-list
  : operation-list operation-list-item
  {
    $$ = chain_add($1, $2, chn);
  }
  | operation-list-item
  {
    $$ = $1;
    chain_init_node(&$$->chn);
  }
  ;

operation-list-item
  : operation
  | primitive
  ;

sequence-operation
  : SEQUENCE ':' '[' operation-list ']'
  {
    YFATAL(formula_value_sequence_mk, &@$, &$$, $4);
  }
  ;

operation
  : SELECT ':' SELECTOR
  {
    YFATAL(formula_value_select_mk, &@$, &$$, $3);
  }
  | PROPERTY ':' node-property
  {
    YFATAL(formula_value_property_mk, &@$, &$$, $3);
  }
  | PREPEND ':' primitive
  {
    YFATAL(formula_value_prepend_mk, &@$, &$$, $3);
  }
  | path-search-operation
  ;

path-search-operation
  : PATH_SEARCH ':' primitive
  {
    YFATAL(formula_value_path_search_mk, &@$, &$$, $3);
  }
  ;

node-property
  : NAME         { $$ = NODE_PROPERTY_NAME; }
  | EXT          { $$ = NODE_PROPERTY_EXT; }
  | SUFFIX       { $$ = NODE_PROPERTY_SUFFIX; }
  | BASE         { $$ = NODE_PROPERTY_BASE; }
  | ABSPATH      { $$ = NODE_PROPERTY_ABSPATH; }
  | ABSDIR       { $$ = NODE_PROPERTY_ABSDIR; }
  | RELPATH      { $$ = NODE_PROPERTY_RELPATH; }
  | RELDIR       { $$ = NODE_PROPERTY_RELDIR; }
  | FSROOT       { $$ = NODE_PROPERTY_FSROOT; }
  | VARIANT      { $$ = NODE_PROPERTY_VARIANT; }
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
    YFATAL(ixstrcat, &$$, $2);
  }
  | strpart
  ;

strpart
  : STR
  {
    $$ = 0;
    YFATAL(ixstrndup, &$$, @1.s, @1.l);
  }
  | CREF
  {
    $$ = 0;
    YFATAL(ixstrndup, &$$, (char*)&$1, 1);
  }
  | HREF
  {
    $$ = 0;
    YFATAL(ixstrndup, &$$, (char*)&$1, 1);
  }
  ;

boolean
  : BOOL
  {
    YFATAL(formula_value_boolean_mk, &@$, &$$, $1);
  }
  ;

posint
  : posint_ugroup
  {
    YFATAL(formula_value_posint_mk, &@$, &$$, $1);
  }
  | posint_igroup
  {
    YFATAL(formula_value_posint_mk, &@$, &$$, $1);
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
    YFATAL(formula_value_negint_mk, &@$, &$$, $1);
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
    YFATAL(formula_value_float_mk, &@$, &$$, $1);
  }
  ;

variable
  : VARIABLE
  {
    YFATAL(formula_value_variable_mk, &@$, &$$, $1.s, $1.l);
  }
  ;

sysvar
  : sysvar-token
  {
    YFATAL(formula_value_sysvar_mk, &@$, &$$, $1);
  }
  ;

sysvar-token
  : BM_TARGET
  | BM_TARGETS
  | BM_SOURCE
  | BM_SOURCES
  | BM_VARIANT
  ;
