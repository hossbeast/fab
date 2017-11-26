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
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>

	#include "ff_parser.internal.h"
  #include "ff_node.h"
}

%code top {
	int ff_yylex(void *, void *, void *);

  #define XTRA ((ff_extra*)xtra)
}

%define api.pure
%error-verbose
%locations
%name-prefix "ff_yy"
%parse-param { void* scanner }
%parse-param { yyu_extra* xtra }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
  uint8_t           u8;
  uint32_t          u32;
	struct ff_node*		node;
}

/* terminals with a semantic value */
%token       NAME 
%token       WORD       /* a word definitely does not contain nonprintable characters */
%token       CHAR       /* a char is definitely not a nonprintable character */
%token <u8>  CREF       /* a cref may be a nonprintable character */
%token <u32> POSINT

/* noun keywords */
%token       RULE		  "rule"
%token       ARTIFACT "artifact"
%token       FORMULA  "formula"
%token       TASK     "task"
%token       VAR      "var"

/* verb keywords */
%token       USE      "use"
%token       REQUIRE  "require"

/* nonterminals */
%type <node>
 alternation
 alternation-part
 alternation-part-epsilon
 alternation-parts
 alternation-parts-no-epsilon
 alternation-parts-with-epsilon
 alternation-pattern
 alternation-pattern-parts
 artifact
 artifact-pattern
 artifact-pattern-alternation
 artifact-pattern-alternation-part
 artifact-pattern-alternation-parts
 artifact-pattern-alternation-parts-no-epsilon
 artifact-pattern-alternation-parts-with-epsilon
 artifact-pattern-alternation-pattern
 artifact-pattern-alternation-pattern-parts
 artifact-pattern-class
 artifact-pattern-part
 artifact-pattern-part-inner
 artifact-pattern-parts
 artifact-pattern-parts-list
 char
 class class-parts class-part
 dirsep
 dot
 formula
 pattern
 pattern-list
 pattern-part
 pattern-part-inner
 pattern-parts
 pattern-parts-list
 patterns
 quoted-string quoted-string-literal quoted-strpart quoted-strparts
 range
 require require-pattern require-parts require-part
 rule
 statements statement
 stem
 string string-list string-literal strings
 task
 unquoted-string unquoted-string-literal unquoted-strpart
 use use-pattern use-parts use-part
 variant
 varname vardef varname-variant

%destructor { ffn_free($$); } <node>

%%
utterance
  : statements
  {
    XTRA->root = $1;
  }
  |
  ;

statements
  : statements statement
  {
    $$ = ffn_append($1, $2);
  }
  | statement
  ;

statement
	: rule
  | formula
  | artifact
  | use
  | require
  | task
  | vardef
	;

vardef
  : VAR varname strings
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_VARDEF, $2, $3);
  }
  ;

varname
  : NAME
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_VARNAME, @1.l, @1.s, 0);
  }
  | NAME '.' varname-variant
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_VARNAME, @1.l, @1.s, $3);
  }
  ;

varname-variant
  : varname-variant '.' string-literal
  {
    $$ = ffn_append($1, $3);
  }
  | string-literal
  ;

formula
  : FORMULA
  {
    $$ = 0;
  }
  ;

use
  : USE use-pattern
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_USE, $2);
  }
  ;

use-pattern
  : use-parts
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERN, $1);
  }
  ;

use-parts
  : use-parts use-part
  {
    $$ = ffn_append($1, $2);
  }
  | use-part
  ;

use-part
  : string-literal
  | dirsep
  ;

require
  : REQUIRE require-pattern
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_REQUIRE, $2);
  }
  ;

require-pattern
  : require-parts
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERN, $1);
  }
  ;

require-parts
  : require-parts require-part
  {
    $$ = ffn_append($1, $2);
  }
  | require-part
  ;

require-part
  : string-literal
  | dirsep
  | dot
  ;

task
  : TASK
  {
    $$ = 0;
  }
  ;

artifact
  : ARTIFACT artifact-pattern
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_ARTIFACT, $2);
  }
  ;

 /* artifact patterns do not contain %, and ? has a different meaning */
artifact-pattern
  : artifact-pattern-part
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_ARTIFACT_PATTERN, $1);
  }
  | artifact-pattern-parts
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_ARTIFACT_PATTERN, $1);
  }
  ;

 /* begins and ends with an artifact-pattern-part */
artifact-pattern-parts
  : artifact-pattern-parts-list artifact-pattern-part
  {
    $$ = ffn_append($1, $2);
  }
  ;

artifact-pattern-parts-list
  : artifact-pattern-parts-list artifact-pattern-part-inner
  {
    $$ = ffn_append($1, $2);
  }
  | artifact-pattern-part
  ;

 /* may appear at any position in the artifact-pattern */
artifact-pattern-part
  : artifact-pattern-alternation
  | artifact-pattern-class
  | dot
  | string-literal
  ;

 /* may not be the first or last item in the artifact-pattern */
artifact-pattern-part-inner
  : artifact-pattern-part
  | dirsep
  ;

rule
  : RULE patterns ':' patterns           /* consequent : antecedent */
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_RULE, $2, $4);
  }
  ;

patterns
  : pattern-list ','
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERNS, $1);
  }
  | pattern-list
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERNS, $1);
  }
  ;

pattern-list
  : pattern-list ',' pattern
  {
    $$ = ffn_append($1, $3);
  }
  | pattern
  ;

pattern
  : pattern-part
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERN, $1);
  }
  | pattern-parts
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERN, $1);
  }
  ;

 /* begins and ends with a pattern-part */
pattern-parts
  : pattern-parts-list pattern-part
  {
    $$ = ffn_append($1, $2);
  }
  ;

pattern-parts-list
  : pattern-parts-list pattern-part-inner
  {
    $$ = ffn_append($1, $2);
  }
  | pattern-part
  ;

 /* may appear at any position in the pattern */
pattern-part
  : alternation
  | class
  | dot
  | string-literal
  | stem
  | variant
  ;

 /* may not be the first or last item in the pattern */
pattern-part-inner
  : pattern-part
  | dirsep
  ;

dirsep
  : '/'   /* matches a directory separator */
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_DIRSEP, '/');
  }
  ;

variant
  : '?'   /* matches the variant suffix */
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_VARIANT);
  }
  ;

dot
  : '.'
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_CHAR, @1.s[0]);
  }
  ;

stem
  : '%'   /* matches 0+ characters, not including the directory separator */
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_STEM, 0);
  }
  | '%' '(' POSINT ')'
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_STEM, $3);
  }
  ;

class
  : '[' class-parts ']'        /* matches one byte in the class */
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_CLASS, $2, 0);
  }
  | '[' '^' class-parts ']'    /* matches one byte not in the class */
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_CLASS, $3, FFN_INVERT);
  }
  ;

class-parts
  : class-parts class-part
  {
    $$ = ffn_append($1, $2);
  }
  | class-part
  ;

class-part
  : range
  | char
  ;

range
  : char '-' char
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_RANGE, $1, $3);
  }
  ;

alternation
  : '{' alternation-parts '}'
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_ALTERNATION, $2, 0);
  }
  ;

alternation-parts
  : alternation-parts-with-epsilon
  | alternation-parts-no-epsilon
  | alternation-part-epsilon ',' alternation-parts-no-epsilon
  {
    $$ = ffn_append($1, $3);
  }
  ;

alternation-parts-no-epsilon
  : alternation-parts-no-epsilon ',' alternation-part
  {
    $$ = ffn_append($1, $3);
  }
  | alternation-part
  ;

alternation-parts-with-epsilon
  : alternation-parts-with-epsilon ',' alternation-part
  {
    $$ = ffn_append($1, $3);
  }
  | alternation-parts-no-epsilon ',' alternation-part-epsilon
  {
    $$ = ffn_append($1, $3);
  }
  ;

alternation-part-epsilon
  :
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_STRING, 0);
  }
  ;

alternation-part
  : alternation-pattern
  ;

alternation-pattern
  : alternation-pattern-parts
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERN, $1);
  }
  ;

alternation-pattern-parts
  : alternation-pattern-parts pattern-part
  {
    $$ = ffn_append($1, $2);
  }
  | pattern-part
  ;

artifact-pattern-alternation
  : '{' artifact-pattern-alternation-parts '}'
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_ALTERNATION, $2, 0);
  }
  | '{' artifact-pattern-alternation-parts '}' '?'
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_ALTERNATION, $2, FFN_VARIANT_GROUP);
  }
  ;

artifact-pattern-alternation-parts
  : artifact-pattern-alternation-parts-with-epsilon
  | artifact-pattern-alternation-parts-no-epsilon
  | alternation-part-epsilon ',' artifact-pattern-alternation-parts-no-epsilon
  {
    $$ = ffn_append($1, $3);
  }
  ;

artifact-pattern-alternation-parts-no-epsilon
  : artifact-pattern-alternation-parts-no-epsilon ',' artifact-pattern-alternation-part
  {
    $$ = ffn_append($1, $3);
  }
  | artifact-pattern-alternation-part
  ;

artifact-pattern-alternation-parts-with-epsilon
  : artifact-pattern-alternation-parts-with-epsilon ',' artifact-pattern-alternation-part
  {
    $$ = ffn_append($1, $3);
  }
  | artifact-pattern-alternation-parts-no-epsilon ',' alternation-part-epsilon
  {
    $$ = ffn_append($1, $3);
  }
  ;

artifact-pattern-alternation-part
  : artifact-pattern-alternation-pattern
  ;

artifact-pattern-alternation-pattern
  : artifact-pattern-alternation-pattern-parts
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_PATTERN, $1);
  }
  ;

artifact-pattern-alternation-pattern-parts
  : artifact-pattern-alternation-pattern-parts artifact-pattern-part
  {
    $$ = ffn_append($1, $2);
  }
  | artifact-pattern-part
  ;

artifact-pattern-class
  : class
  | class '?'
  {
    $$ = $1;
    $$->attrs |= FFN_VARIANT_GROUP;
  }
  ;

strings
  : string-list
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_STRINGS, $1);
  }
  ;

string-list
  : string-list string
  {
    $$ = ffn_append($1, $2);
  }
  | string
  ;

string
  : quoted-string
  | unquoted-string
  ;

quoted-string
  : quoted-string-literal
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_STRING, $1);
  }
  ;

unquoted-string
  : unquoted-string-literal
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_STRING, $1);
  }
  ;

string-literal
  : unquoted-string-literal
  | quoted-string-literal
  ;

unquoted-string-literal
  : unquoted-strpart
  ;

quoted-string-literal
  : '"' quoted-strparts '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$ = 0;
  }
  ;

quoted-strparts
  : quoted-strparts quoted-strpart
  {
    $$ = ffn_append($1, $2);
  }
  | quoted-strpart
  ;

quoted-strpart
  : unquoted-strpart
  | char
  ;

unquoted-strpart
  : NAME
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_WORD, @1.l, @1.s, 0);
  }
  | WORD
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_WORD, @1.l, @1.s, 0);
  }
  | POSINT
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_WORD, @1.l, @1.s, 0);
  }
  | '$' NAME
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_USRVAR, @2.l, @2.s);
  }
  ;

char
  : CHAR
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_CHAR, @1.s[0]);
  }
  | CREF
  {
    YFATAL(ffn_mknode, &$$, &@$, FFN_CHAR, $1);
  }
  ;
