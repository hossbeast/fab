/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include "types.h"

  #include "pattern/alternation.h"
  #include "pattern/byte.h"
  #include "pattern/class.h"
  #include "pattern/range.h"
  #include "pattern/word.h"
  #include "pattern/star.h"
  #include "pattern/section.h"
  #include "pattern/segments.h"

  #include "pattern_parser.internal.h"
  #include "pattern.internal.h"

#include <stdlib.h>
#include <unistd.h>
#include "types.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, pattern_parser, match_yyu)
  #define MATCH_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {match_pattern_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;
  pattern * pattern;
  pattern_segments * segments;
  pattern_section * section;
  pattern_segment * segment;
  llist list;
}

/* tokens from pattern.l */
%token '(' ')'      // groups
%token '{' '}'      // alternations
%token '[' ']'      // classes
%token ':'
%token '?'
%token '~'
%token '+'
%token '$'
%token '.'
%token '@'
%token '_'

%token <yyu.umax>  UINTMAX8   100
%token <yyu.umax>  UINTMAX16  101
%token <yyu.imax>  INTMAX8    102
%token <yyu.imax>  INTMAX16   103

/* terminals */
%token             STR        104 "string"
%token <yyu.u8>    CREF       105 "character-escape"
%token <yyu.u8>    HREF       106 "hex-escape"
%token             STARSTAR   107 "**"
%token             DOT        108 "."
%token             DOT2       109 ".."
%token <yyu.u8>    CHAR       110
%token MODULE                 111 "module"
%token MODULES                112 "modules"
%token SHADOW                 113 "shadow"
%token UP                     114 "up"
%token DOWN                   115 "down"
%token SELF_OR_BELOW          116 "self-or-below"
%token DIRS                   117 "dirs"

%type <yyu.umax>   uint16

/* nonterminals */
%type <list> alternation-parts-without-epsilon
%type <list> alternation-parts-with-epsilon
%type <list> alternation-pattern-parts
%type <list> quoted-strparts quoted-string-literal unquoted-string-literal string-literal
%type <list> pattern-relative-sections-list pattern-absolute-sections-list
%type <list> pattern-dentry-segments-list
%type <list> pattern-dentry-segment-list
%type <list> class-segments-list
%type <list> pattern-qualifier-list
%type <list> pattern-qualifiers
%type <list> pattern-qualifier-part-list pattern-qualifier-parts

%type <pattern> pattern

%type <section> pattern-section
%type <section> pattern-initial-section

%type <segments> alternation-pattern
%type <segments> class-segments
%type <segments> pattern-dentry-segments
%type <segments> pattern-qualifier

%type <segment> escape
%type <segment> class class-segment class-range class-char
%type <segment> word
%type <segment> star
%type <segment> quoted-strpart unquoted-strpart
%type <segment> pattern-dentry-segment
%type <segment> pattern-qualifier-part
%type <segment> alternation
%type <segment> alternation-pattern-part

%destructor { pattern_free($$); } <pattern>
%destructor { pattern_section_free($$); } <section>
%destructor { pattern_segment_free($$); } <segment>
%destructor { pattern_segments_free($$); } <segments>

%%
utterance
  : pattern
  {
    PARSER->pattern = $1;
    if(PARSER->match_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->match_yyu, &@1);
      YYACCEPT;
    }
  }
  ;

pattern
  : pattern-relative-sections-list
  {
    pattern_mk(&$$, &@$, &$1);
  }
  | pattern-absolute-sections-list
  {
    pattern_mk(&$$, &@$, &$1);
  }
  ;

pattern-relative-sections-list
  : pattern-relative-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | pattern-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
    PARSER->section_has_type_qualifiers = false;
  }
  ;

pattern-absolute-sections-list
  : pattern-absolute-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | pattern-initial-section pattern-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
    llist_push(&$$, &$2->lln);
    PARSER->section_has_type_qualifiers = false;
  }
  ;

pattern-initial-section
  : '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_ROOT, 0, 0, NULL, false);
  }
  ;

pattern-section
  : pattern-dentry-segments-list
  {
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, &$1, false);
  }
  | STARSTAR pattern-qualifiers
  {
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_DIRS, PATTERN_AXIS_SELF_OR_BELOW, &$2, false);
  }
  ;

pattern-dentry-segments-list
  : pattern-dentry-segments pattern-qualifier-list
  {
    $$ = $2;
    llist_unshift(&$$, &$1->lln);
  }
  | pattern-dentry-segments
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

pattern-dentry-segments
  : pattern-dentry-segment-list
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, &$1);
  }
  ;

pattern-dentry-segment-list
  : pattern-dentry-segment-list pattern-dentry-segment
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | pattern-dentry-segment-list string-literal
  {
    $$ = $1;
    llist_splice_tail(&$$, &$2);
  }
  | pattern-dentry-segment
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  | string-literal
  ;

 /* appears at any position in a pattern-dirnode section */
pattern-dentry-segment
  : alternation
  | class
  | star
  ;

pattern-qualifiers
  : pattern-qualifier-list
  | %empty
  {
    $$ = (typeof($$)){ };
  }
  ;

pattern-qualifier-list
  : pattern-qualifier-list pattern-qualifier
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | pattern-qualifier
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

pattern-qualifier
  : '+' pattern-qualifier-parts
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, &$2);
  }
  | '~' pattern-qualifier-parts
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_NOT, &$2);
  }
  ;

pattern-qualifier-parts
  : pattern-qualifier-part-list
  ;

pattern-qualifier-part-list
  : pattern-qualifier-part-list pattern-qualifier-part
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | pattern-qualifier-part-list string-literal
  {
    $$ = $1;
    llist_splice_tail(&$$, &$2);
  }
  | pattern-qualifier-part
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  | string-literal
  ;

pattern-qualifier-part
  : alternation     // { }
  | class           // [ ]
  ;

star
  : '*'
  {
    pattern_star_mk(&$$, &@$);
  }
  ;

class
  : '[' class-segments-list ']'
  {
    pattern_class_mk(&$$, &@$, &$2, false);
  }
  | '[' '!' class-segments-list ']'
  {
    pattern_class_mk(&$$, &@$, &$3, true);
  }
  ;

class-segments-list
  : class-segments-list class-segments
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | class-segments
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

class-segments
  : class-segment
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$1->lln);
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, &list);
  }
  ;

class-segment
  : class-range
  | class-char
  ;

class-range
  : CHAR '-' CHAR
  {
    pattern_range_mk(&$$, &@$, @1.s[0], @3.s[0]);
  }
  ;

class-char
  : CHAR
  {
    pattern_byte_mk(&$$, &@$, @1.s[0]);
  }
  ;

alternation
  : '{' alternation-parts-with-epsilon '}'
  {
    pattern_alternation_mk(&$$, &@$, &$2, true);
  }
  | '{' alternation-parts-without-epsilon '}'
  {
    pattern_alternation_mk(&$$, &@$, &$2, false);
  }
  ;

alternation-parts-without-epsilon
  : alternation-parts-without-epsilon ',' alternation-pattern
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | alternation-pattern
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
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
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, &$1);
  }
  ;

alternation-pattern-parts
  : alternation-pattern-parts alternation-pattern-part
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | alternation-pattern-parts string-literal
  {
    $$ = $1;
    llist_splice_tail(&$$, &$2);
  }
  | alternation-pattern-part
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  | string-literal
  ;

alternation-pattern-part
  : alternation
  | class
  ;

string-literal
  : unquoted-string-literal
  | quoted-string-literal
  ;

unquoted-string-literal
  : unquoted-strpart
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

quoted-string-literal
  : '"' quoted-strparts '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$ = (typeof($$)){ };
  }
  ;

quoted-strparts
  : quoted-strparts quoted-strpart
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | quoted-strpart
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

quoted-strpart
  : unquoted-strpart
  | escape
  ;

unquoted-strpart
  : word
  ;

word
  : word-token
  {
    pattern_word_mk(&$$, &@$, @1.s, @1.l);
  }
  | uint16
  {
    pattern_word_mk(&$$, &@$, @1.s, @1.l);
  }
  ;

word-token
  : STR
  | MODULE
  | MODULES
  | SHADOW
  | UP
  | DOWN
  | SELF_OR_BELOW
  | DIRS
  | 'A'
  | 'D'
  | 'F'
  | 'H'
  | 'M'
  | '@'
  | '_'
  | '.'
  ;

escape
  : CREF
  {
    pattern_byte_mk(&$$, &@$, $1);
  }
  | HREF
  {
    pattern_byte_mk(&$$, &@$, $1);
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
