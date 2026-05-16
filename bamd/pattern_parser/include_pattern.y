/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <string.h>

  #include "types.h"

  #include "pattern/byte.h"
  #include "pattern/word.h"
  #include "pattern/segments.h"
  #include "pattern/section.h"
  #include "pattern_parser.internal.h"
  #include "pattern.internal.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, pattern_parser, include_yyu)
  #define INCLUDE_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {include_pattern_yy}
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
%type <list> quoted-strparts quoted-string-literal string-literal
%type <list> pattern-sections-list
%type <list> pattern-dentry-segment-list

%type <pattern> pattern

%type <section> pattern-section pattern-initial-section
%type <section> pattern-section-fs-child

%type <segments> pattern-dentry-segments

%type <segment> escape
%type <segment> word
%type <segment> quoted-strpart

%destructor { pattern_free($$); } <pattern>
%destructor { pattern_section_free($$); } <section>
%destructor { pattern_segment_free($$); } <segment>
%destructor { pattern_segments_free($$); } <segments>

%%
utterance
  : pattern
  {
    PARSER->pattern = $1;
    if(PARSER->include_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->include_yyu, &@1);
      YYACCEPT;
    }
  }
  ;

pattern
  : pattern-sections-list
  {
    pattern_mk(&$$, &@$, &$1);
  }
  | pattern-initial-section
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$1->lln);
    pattern_mk(&$$, &@$, &list);
  }
  ;

pattern-sections-list
  : pattern-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | pattern-initial-section pattern-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
    llist_push(&$$, &$2->lln);
  }
  ;

/* include patterns are absolute references */
pattern-initial-section
  : '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_ROOT, 0, 0, NULL, false);
  }
  ;

pattern-section
  : pattern-section-fs-child
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
  : pattern-dentry-segment-list string-literal
  {
    $$ = $1;
    llist_splice_tail(&$$, &$2);
  }
  | string-literal
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
  : word
  | escape
  ;

word
  : word-tokens
  {
    pattern_word_mk(&$$, &@$, @1.s, @1.l);
  }
  | uint16
  {
    pattern_word_mk(&$$, &@$, @1.s, @1.l);
  }
  ;

word-tokens
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
  | '+'
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
