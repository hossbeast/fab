/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <string.h>

  #include "types.h"
  #include "valyria/llist.h"

  #include "pattern/alternation.h"
  #include "pattern/byte.h"
  #include "pattern/class.h"
  #include "pattern/group.h"
  #include "pattern/range.h"
  #include "pattern/word.h"
  #include "pattern/variable.h"
  #include "pattern/section.h"
  #include "pattern/segments.h"

  #include "pattern_parser.internal.h"
  #include "pattern.internal.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, pattern_parser, generate_yyu)
  #define GENERATE_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {generate_pattern_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval  yyu;

  struct {
    char s[64];
    uint8_t l;
  } tag_text;

  pattern * pattern;
  pattern_segments * segments;
  pattern_section * section;
  pattern_segment * segment;
  pattern_nodeset nodeset;
  pattern_graph graph;
  pattern_axis axis;
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
%type <list> alternation-pattern-parts
%type <list> alternation-parts
%type <list> quoted-strparts quoted-string-literal string-literal
%type <list> pattern-sections-list
%type <list> pattern-dentry-segment-list
%type <list> class-segments-list

%type <pattern> pattern

%type <section> pattern-section pattern-section-self
%type <section> pattern-initial-section
%type <section> pattern-section-fs-child

%type <segments> pattern-dentry-segments
%type <segments> class-segments
%type <segments> alternation-pattern

%type <segment> escape
%type <segment> class class-segment class-range class-char
%type <segment> word
%type <segment> variable
%type <segment> quoted-strpart
%type <segment> pattern-dentry-segment
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
    yyu_loc_final(&PARSER->generate_yyu, &@1);
    if(PARSER->generate_yyu.attrs & YYU_PARTIAL)
    {
      YYACCEPT;
    }
  }
  ;

pattern
  : pattern-sections-list
  {
    pattern_mk(&$$, &@$, &$1);
  }
  ;

pattern-sections-list
  : pattern-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | pattern-initial-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

pattern-initial-section
  : '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SHADOW, 0, 0, NULL, false);
  }
  | '$' '^' 'A'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_ANCHOR, 0, 0, NULL, false);
  }
  | '$' '^' 'D'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_MODULE, 0, 0, NULL, false);
  }
  | pattern-section
  ;

pattern-section
  : pattern-section-fs-child
  | pattern-section-self
  ;

pattern-section-self
  : DOT
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SELF, 0, 0, NULL, false);
  }
  ;

pattern-section-fs-child
  : pattern-dentry-segments
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$1->lln);
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, &list, false);
  }
  ;

pattern-dentry-segments
  : pattern-dentry-segment-list
  {
    pattern_segments_mk(&$$, &@$, 0, &$1);
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

 /* may appear at any position in a pattern */
pattern-dentry-segment
  : alternation
  | class
  | variable
  ;

variable
  /* numbered variables resolve to a parenthesized match group by index */
  : '$' uint16
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NUM, $2, NULL, 0);
  }
  /* named variables resolve to a parenthesized match group by name, or to a variable in scope */
  | '$' word-token
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NAME, 0, @2.s, @2.l);
  }
  | '$' '{' uint16 '}'
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NUM, $3, NULL, 0);
  }
  | '$' '{' word-token '}'
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NAME, 0, @3.s, @3.l);
  }
  /* module hash */
  | '$' '^' 'H'
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_AUTO_HASH, 0, 0, 0);
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
    pattern_segments_mk(&$$, &@$, 0, &list);
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
  : '{' alternation-parts '}'
  {
    pattern_alternation_mk(&$$, &@$, &$2, false);
  }
  | '{' ',' alternation-parts '}'
  {
    pattern_alternation_mk(&$$, &@$, &$3, true);
  }
  | '{' alternation-parts ',' '}'
  {
    pattern_alternation_mk(&$$, &@$, &$2, true);
  }
  ;

alternation-parts
  : alternation-parts ',' alternation-pattern
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

alternation-pattern
  : alternation-pattern-parts
  {
    pattern_segments_mk(&$$, &@$, 0, &$1);
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
  : quoted-string-literal
  | word
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
    llist_init(&$$);
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
  : word
  | escape
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
  | 'H'
  | 'F'
  | 'M'
  | '@'
  | '_'
  | '+'
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
