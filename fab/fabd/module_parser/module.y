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
  #include <string.h>

  #include "types.h"

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/map.h"
  #include "valyria/set.h"
  #include "valyria/list.h"
  #include "valyria/chain.h"

  #include "rule.h"
  #include "variant.h"

  #include "module_parser.internal.h"
  #include "module.internal.h"
  #include "pattern.h"

  #include "macros.h"

  struct pattern;
  struct value;
}

%code top {
  int module_yylex(void*, void*, void*);
  #define PARSER ((module_parser*)parser)
  #define MODULE_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {module_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval  yyu;

  struct rule * rule;
  variant * variant;
  struct pattern * pattern;
  struct value * value;
  struct node * node;

  // enums
  edge_type relation_type;
}

/* tokens */
%token '[' ']'
%token ':'
%token '<' '>'

/* terminals */
%token             STR        104 "string"
%token <yyu.u8>    CREF       105 "character-escape"
%token <yyu.u8>    HREF       106 "hex-escape"

%token
 AS       "as"
 CONDUIT  "conduit"
 USE      "use"
 REQUIRE  "require"
 IMPORT   "import"
 RELATION "relation"
 STRONG   "strong"
 RULE     "rule"
 VARIANT  "variant"
 VAR      "var"
 LARROW   "<-"
 RARROW   "->"
 DASHES   "--"
 STARBOX  "[*]"

%token
 <pattern> MATCH_PATTERN      "match-pattern"
 <pattern> GENERATE_PATTERN   "generate-pattern"
 <pattern> REFERENCE_PATTERN  "reference-pattern"
 <pattern> LOOKUP_PATTERN     "lookup-pattern"
 <value> VALUE "var-set"

%type <yyu.s> string quoted-string unquoted-string strpart strparts
%type <rule> rule
%type <yyu.u16> connector
%type <pattern> match-pattern
%type <pattern> generate-pattern
%type <pattern> reference-pattern
%type <pattern> lookup-pattern
%type <relation_type> relation-type rule-bacon
%type <node> formula-reference

// %destructor { module_free($$); } <module>
%destructor { pattern_free($$); } <pattern>

/* nonterminals */

%%
utterance
  : allocate-module statements
  | %empty
  ;

allocate-module
  : %empty
  {
    YFATAL(module_parser_block_alloc, PARSER, &PARSER->unscoped_block);
    PARSER->block = PARSER->unscoped_block;
    PARSER->var_value = 0;
    PARSER->var_merge_overwrite = 0;
  }
  ;

statements
  : statements statement
  | statement
  ;

statement
  : use
  | require
  | import
  | variant
  | block-statement
  | block
  | var-section
  ;

block-statement-list
  : block-statements
  {

  }
  ;

block-statements
  : block-statements block-statement
  | block-statement
  ;

block-statement
  : rule
  {
    llist_append(&PARSER->block->rules, $1, lln);
  }
  ;

use
  : USE lookup-pattern
  {
    /* frees $2 */
    YFATAL(PARSER->use_resolve, PARSER, $2);
  }
  ;

import
  : IMPORT lookup-pattern AS string
  {
    /* frees $2 and $4 */
    YFATAL(PARSER->import_resolve, PARSER, $2, true, $4, strlen($4));
  }
  | IMPORT lookup-pattern AS
  {
    /* frees $2 */
    YFATAL(PARSER->import_resolve, PARSER, $2, false, NULL, 0);
  }
  | IMPORT lookup-pattern
  {
    /* frees $2 */
    YFATAL(PARSER->import_resolve, PARSER, $2, true, NULL, 0);
  }
  ;

require
  : REQUIRE lookup-pattern
  {
    /* frees $2 */
    YFATAL(PARSER->require_resolve, PARSER, $2);
  }
  ;

var-section
  : VAR var-section-body
  ;

var-section-body
  : ':' var-section-set
  | '=' var-section-set-epsilon
  {
    PARSER->var_merge_overwrite = true;
  }
  ;

var-section-set-epsilon
  : var-section-set
  | %empty
  ;

var-section-set
  : VALUE
  {
    PARSER->var_value = $1;
  }
  ;

variant
  : VARIANT lookup-pattern
  {
    YFATAL(module_block_variants, PARSER->block, $2);
    /* no longer needed */
    pattern_free($2);
  }
  ;

open-block
  : %empty
  {
    YFATAL(module_parser_block_alloc, PARSER, &PARSER->block);
  }
  ;

block
  : VARIANT lookup-pattern ':' open-block '[' block-statement-list ']'
  {
    YFATAL(module_block_variants, PARSER->block, $2);
    /* no longer needed */
    pattern_free($2);

    /* restore */
    llist_append(&PARSER->scoped_blocks, PARSER->block, lln);
    PARSER->block = PARSER->unscoped_block;
  }
  ;

formula-reference
  : reference-pattern
  {
    YFATAL(PARSER->formula_resolve, PARSER, $1, &$$);
  }
  ;

rule
 /* (match) antecedent(s) -> (generate) consequent(s) : (reference) formula */
  : RULE rule-bacon match-pattern connector generate-pattern ':' formula-reference
  {
    YFATAL(rule_mk, &$$, PARSER->g, $3, $5, $7, $4 | $2);
  }
  | RULE rule-bacon match-pattern connector generate-pattern
  {
    YFATAL(rule_mk, &$$, PARSER->g, $3, $5, NULL, $4 | $2);
  }
 /* match only */
  | RULE rule-bacon match-pattern DASHES ':' formula-reference
  {
    YFATAL(rule_mk, &$$, PARSER->g, $3, NULL, $6, RULE_ZERO_TO_ONE | $2);
  }
  | RULE rule-bacon match-pattern STARBOX DASHES ':' formula-reference
  {
    YFATAL(rule_mk, &$$, PARSER->g, $3, NULL, $7, RULE_ZERO_TO_MANY | $2);
  }
 /* generate only */
  | RULE rule-bacon DASHES generate-pattern ':' formula-reference
  {
    YFATAL(rule_mk, &$$, PARSER->g, NULL, $4, $6, RULE_ZERO_TO_ONE | $2);
  }
  | RULE rule-bacon DASHES STARBOX generate-pattern ':' formula-reference
  {
    YFATAL(rule_mk, &$$, PARSER->g, NULL, $5, $7, RULE_ZERO_TO_MANY | $2);
  }
  ;

rule-bacon
  : '{' RELATION ':' relation-type '}'
  {
    $$ = $4;
  }
  | %empty
  {
    $$ = EDGE_TYPE_STRONG;
  }
  ;

relation-type
  : CONDUIT   { $$ = EDGE_TYPE_CONDUIT; }
  | STRONG    { $$ = EDGE_TYPE_STRONG; }
  ;

connector
  : LARROW          /* <- */
  {
    $$ = RULE_RTL | RULE_ONE_TO_ONE;
  }
  | LARROW STARBOX  /* <- [*] */
  {
    $$ = RULE_RTL | RULE_ONE_TO_MANY;
  }
  | RARROW          /* -> */
  {
    $$ = RULE_LTR | RULE_ONE_TO_ONE;
  }
  | RARROW STARBOX  /* -> [*] */
  {
    $$ = RULE_LTR | RULE_ONE_TO_MANY;
  }
  ;

match-pattern
  : MATCH_PATTERN
  ;

generate-pattern
  : GENERATE_PATTERN
  ;

reference-pattern
  : REFERENCE_PATTERN
  ;

lookup-pattern
  : LOOKUP_PATTERN
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
