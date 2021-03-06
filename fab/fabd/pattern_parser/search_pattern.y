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
  #include "valyria/chain.h"

  #include "pattern/alternation.h"
  #include "pattern/byte.h"
  #include "pattern/class.h"
  #include "pattern/group.h"
  #include "pattern/range.h"
  #include "pattern/variants.h"
  #include "pattern/word.h"
  #include "pattern/star.h"

  #include "pattern/section.h"

  #include "pattern_parser.internal.h"
  #include "pattern.internal.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, pattern_parser, search_yyu)
  #define SEARCH_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {search_pattern_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;
  pattern * pattern;
  pattern_segments * segments;
  pattern_section * section;
  pattern_segment * segment;
  pattern_graph graph;
  pattern_axis axis;
}

/* tokens from pattern.l */
%token '{' '}'
%token '[' ']'
%token ':'
%token '?'
%token '~'
%token '+'
%token '$'

%token <yyu.umax>  UINTMAX8   100
%token <yyu.umax>  UINTMAX16  101
%token <yyu.imax>  INTMAX8    102
%token <yyu.imax>  INTMAX16   103

/* terminals */
%token             STR        104 "string"
%token <yyu.u8>    CREF       105 "character-escape"
%token <yyu.u8>    HREF       106 "hex-escape"
%token             STARSTAR   107 "**"
%token             SLASH2     108 "/"
%token <yyu.u8>    CHAR       109
%token MODULE                 110 "module"
%token MODULES                111 "modules"
%token SHADOW                 112 "shadow"
%token UP                     113 "up"
%token DOWN                   114 "down"
%token SELF_OR_BELOW          115 "self-or-below"
%token DIRS                   116 "dirs"
%token REQUIRES               117 "requires"
%token USES                   118 "uses"

%type <yyu.umax>   uint16

/* nonterminals */
%type <pattern> pattern
%type <segments> pattern-qualifiers
%type <segments> pattern-qualifier
%type <segments> pattern-qualifier-list
%type <segments> pattern-dentry pattern-dentry-list

%type <section> pattern-section pattern-sections-list

%type <segment> escape
%type <segments> class-pattern class-parts
%type <segment> class class-part class-range class-char
%type <segment> group
%type <segment> word
%type <segment> variants
%type <segment> star
%type <segment> quoted-string-literal quoted-strpart quoted-strparts
%type <segment> unquoted-string-literal unquoted-strpart
%type <segment> string-literal
%type <segment> pattern-dentry-part pattern-dentry-parts
%type <segment> pattern-qualifier-part pattern-qualifier-parts
%type <segment> alternation
%type <segment> alternation-pattern-part
%type <segment> alternation-pattern-parts
%type <segments> alternation-pattern
%type <segments> alternation-parts-with-epsilon
%type <segments> alternation-parts-without-epsilon

%destructor { pattern_free($$); } <pattern>
%destructor { pattern_section_free($$); } <section>
%destructor { pattern_segment_free($$); } <segment>
%destructor { pattern_segments_free($$); } <segments>

%%
utterance
  : pattern
  {
    PARSER->pattern = $1;
    if(PARSER->search_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->search_yyu, &@1);
      YYACCEPT;
    }
  }
  ;

pattern
  : pattern-sections-list
  {
    YFATAL(pattern_mk, &$$, &@$, $1);
  }
  ;

pattern-sections-list
  : pattern-sections-list '/' pattern-section
  {
    $$ = chain_splice_tail($1, $3, chn);
  }
  | pattern-section
  | SLASH2
  {
    YFATAL(pattern_section_mk, &$$, &@$, PATTERN_NODESET_SHADOW, 0, 0, NULL);
  }
  ;

pattern-section
  : pattern-dentry-list
  {
    YFATAL(pattern_section_mk, &$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, $1);
  }
  | STARSTAR pattern-qualifiers
  {
    YFATAL(pattern_section_mk, &$$, &@$, 0, PATTERN_GRAPH_DIRS, PATTERN_AXIS_SELF_OR_BELOW, $2);
  }
  ;

pattern-dentry-list
  : pattern-dentry pattern-qualifier-list
  {
    $$ = chain_splice_tail($1, $2, chn);
  }
  | pattern-dentry
  ;

pattern-dentry
  : pattern-dentry-parts
  {
    YFATAL(pattern_segments_mk, &$$, &@$, PATTERN_QUALIFIER_TYPE_AND, $1);
  }
  ;

pattern-dentry-parts
  : pattern-dentry-parts pattern-dentry-part
  {
    $$ = chain_add($1, $2, chn);
  }
  | pattern-dentry-part
  ;

 /* appears at any position in a pattern-dirnode section */
pattern-dentry-part
  : alternation
  | class
  | group
  | star
  | string-literal
  | variants
  ;

group
  : '(' pattern-dentry-list ')'
  {
    YFATAL(pattern_group_mk, &$$, &@$, $2, NULL, 0, ++PARSER->group_counter);
  }
  | '(' '?' '<' word-tokens '>'  pattern-dentry-list ')'
  {
    YFATAL(pattern_group_mk, &$$, &@$, $6, @4.s, @4.l, ++PARSER->group_counter);
  }
  ;

pattern-qualifiers
  : pattern-qualifier-list
  | %empty
  {
    $$ = NULL;
  }
  ;

pattern-qualifier-list
  : pattern-qualifier-list pattern-qualifier
  {
    $$ = chain_add($1, $2, chn);
  }
  | pattern-qualifier
  ;

pattern-qualifier
  : '+' pattern-qualifier-parts
  {
    YFATAL(pattern_segments_mk, &$$, &@$, PATTERN_QUALIFIER_TYPE_AND, $2);
  }
  | '~' pattern-qualifier-parts
  {
    YFATAL(pattern_segments_mk, &$$, &@$, PATTERN_QUALIFIER_TYPE_NOT, $2);
  }
  ;

pattern-qualifier-parts
  : pattern-qualifier-parts pattern-qualifier-part
  {
    $$ = chain_add($1, $2, chn);
  }
  | pattern-qualifier-part
  ;

pattern-qualifier-part
  : alternation
  | class
  | group
  | string-literal
  ;

variants
  : '?'
  {
    YFATAL(pattern_variants_mk, &$$, &@$);
  }
  ;

star
  : '*'
  {
    YFATAL(pattern_star_mk, &$$, &@$);
  }
  ;

class
  : '[' class-parts ']'
  {
    YFATAL(pattern_class_mk, &$$, &@$, $2, false);
  }
  | '[' '!' class-parts ']'
  {
    YFATAL(pattern_class_mk, &$$, &@$, $3, true);
  }
  ;

class-parts
  : class-parts class-pattern
  {
    $$ = chain_add($1, $2, chn);
  }
  | class-pattern
  ;

class-pattern
  : class-part
  {
    YFATAL(pattern_segments_mk, &$$, &@$, PATTERN_QUALIFIER_TYPE_AND, $1);
  }
  ;

class-part
  : class-range
  | class-char
  ;

class-range
  : CHAR '-' CHAR
  {
    YFATAL(pattern_range_mk, &$$, &@$, @1.s[0], @3.s[0]);
  }
  ;

class-char
  : CHAR
  {
    YFATAL(pattern_byte_mk, &$$, &@$, @1.s[0]);
  }
  ;

alternation
  : '{' alternation-parts-with-epsilon '}'
  {
    YFATAL(pattern_alternation_mk, &$$, &@$, $2, true);
  }
  | '{' alternation-parts-without-epsilon '}'
  {
    YFATAL(pattern_alternation_mk, &$$, &@$, $2, false);
  }
  ;

alternation-parts-without-epsilon
  : alternation-parts-without-epsilon ',' alternation-pattern
  {
    $$ = chain_add($1, $3, chn);
  }
  | alternation-pattern
  ;

alternation-parts-with-epsilon
  : alternation-parts-without-epsilon ',' alternation-part-epsilon
  {
    $$ = $1;
  }
  | alternation-part-epsilon ',' alternation-parts-without-epsilon
  {
    $$ = $3;
  }
  ;

alternation-part-epsilon
  : %empty
  ;

alternation-pattern
  : alternation-pattern-parts
  {
    YFATAL(pattern_segments_mk, &$$, &@$, PATTERN_QUALIFIER_TYPE_AND, $1);
  }
  ;

alternation-pattern-parts
  : alternation-pattern-parts alternation-pattern-part
  {
    $$ = chain_add($1, $2, chn);
  }
  | alternation-pattern-part
  ;

alternation-pattern-part
  : alternation
  | class
  | string-literal
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
    $$ = (void*)0;
  }
  ;

quoted-strparts
  : quoted-strparts quoted-strpart
  {
    $$ = chain_add($1, $2, chn);
  }
  | quoted-strpart
  ;

quoted-strpart
  : unquoted-strpart
  | escape
  ;

unquoted-strpart
  : word
  ;

word
  : word-tokens
  {
    YFATAL(pattern_word_mk, &$$, &@$, @1.s, @1.l);
  }
  | uint16
  {
    YFATAL(pattern_word_mk, &$$, &@$, @1.s, @1.l);
  }
  ;

word-tokens
  : STR
  | MODULE
  | MODULES
  | SHADOW
  | SELF_OR_BELOW
  | REQUIRES
  | USES
  | 'D'
  | '.'
  ;

escape
  : CREF
  {
    YFATAL(pattern_byte_mk, &$$, &@$, $1);
  }
  | HREF
  {
    YFATAL(pattern_byte_mk, &$$, &@$, $1);
  }
  ;

uint16
  : INTMAX8
  {
    $$ = $1;
  }
  | INTMAX16
  {
    $$ = $1;
  }
  | UINTMAX8
  | UINTMAX16
  ;
