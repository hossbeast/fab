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

  #include "pattern/byte.internal.h"
  #include "pattern/range.internal.h"
  #include "pattern/word.internal.h"

  #include "pattern/section.internal.h"

  #include "pattern_parser.internal.h"
  #include "pattern.internal.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, pattern_parser, reference_yyu)
  #define REFERENCE_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {reference_pattern_yy}
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
%type <segments> pattern-dentry pattern-dentry-list

%type <section> pattern-section pattern-sections-list

%type <segment> escape
%type <segment> word
%type <segment> quoted-string-literal quoted-strpart quoted-strparts
%type <segment> unquoted-string-literal unquoted-strpart
%type <segment> string-literal
%type <segment> pattern-dentry-part pattern-dentry-parts

%destructor { pattern_free($$); } <pattern>
%destructor { pattern_section_free($$); } <section>
%destructor { pattern_segment_free($$); } <segment>
%destructor { pattern_segments_free($$); } <segments>

%%
utterance
  : pattern
  {
    PARSER->pattern = $1;
    if(PARSER->reference_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->reference_yyu, &@1);
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
  ;

pattern-section
  : pattern-dentry-list
  {
    YFATAL(pattern_section_mk, &$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, $1);
  }
  | '.'
  {
    YFATAL(pattern_section_mk, &$$, &@$, PATTERN_NODESET_SELF, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, NULL);
  }
  ;

pattern-dentry-list
  : pattern-dentry
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
  : string-literal
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
$1 = 0;
    YFATAL(pattern_word_mk, &$$, &@$, @1.s, @1.l);
  }
  ;

word-tokens
  : STR
  | 'D'
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
