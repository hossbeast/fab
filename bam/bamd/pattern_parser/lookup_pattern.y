/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <string.h>

  #include "types.h"

  #include "pattern/alternation.h"
  #include "pattern/byte.h"
  #include "pattern/class.h"
  #include "pattern/range.h"
  #include "pattern/word.h"
  #include "pattern/variable.h"
  #include "pattern/group.h"
  #include "pattern/qualifier.h"
  #include "pattern/segments.h"
  #include "pattern/section.h"

  #include "pattern_parser.internal.h"
  #include "pattern.internal.h"

  #include "macros.h"
}

%code top {
  #define PARSER containerof(parser, pattern_parser, lookup_yyu)
  #define LOOKUP_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {lookup_pattern_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

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
%type <list> class-segments-list
%type <list> pattern-dentry-segments-list
%type <list> pattern-dentry-segment-list
%type <list> type-qualifier-segment-list
%type <list> pattern-type-qualifier-segment-list
%type <list> type-qualifiers-list

%type <pattern> pattern

%type <segments> class-segments
%type <segments> alternation-pattern
%type <segments> pattern-dentry-segments
%type <segments> type-qualifier-segments pattern-type-qualifier

%type <section> pattern-section pattern-initial-section

%type <segment> escape
%type <segment> class-segment
%type <segment> class class-range class-char
%type <segment> pattern-dentry-segment
%type <segment> word
%type <segment> variable
%type <segment> quoted-strpart
%type <segment> unquoted-strpart
%type <segment> alternation
%type <segment> alternation-pattern-part
%type <segment> type-qualifier type-qualifiers

%destructor { pattern_free($$); } <pattern>
%destructor { pattern_section_free($$); } <section>
%destructor { pattern_segment_free($$); } <segment>
%destructor { pattern_segments_free($$); } <segments>

%%
utterance
  : pattern
  {
    PARSER->pattern = $1;
    if(PARSER->lookup_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->lookup_yyu, &@1);
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
  | pattern-initial-section
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$1->lln);
    pattern_mk(&$$, &@$, &list);
  }
  ;

pattern-relative-sections-list
  : pattern-relative-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
    PARSER->section_has_type_qualifiers = false;
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
  }
  ;

pattern-initial-section
  : '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_ROOT, 0, 0, NULL, PARSER->section_has_type_qualifiers);
  }
  | '/' '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SHADOW, 0, 0, NULL, PARSER->section_has_type_qualifiers);
  }
  ;

pattern-section
  : pattern-dentry-segments-list
  {
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, &$1, PARSER->section_has_type_qualifiers);
  }
  | DOT
  {
    pattern_section_mk(&$$, &@$, 0, 0, PATTERN_AXIS_SELF, NULL, PARSER->section_has_type_qualifiers);
  }
  | DOT2
  {
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_UP, NULL, PARSER->section_has_type_qualifiers);
  }
  ;

pattern-dentry-segments-list
  : pattern-dentry-segments pattern-type-qualifier-segment-list
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
  | variable
  ;

variable
  : '$' word-tokens
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NAME, 0, @2.s, @2.l);
  }
  | '$' '{' word-tokens '}'
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NAME, 0, @3.s, @3.l);
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

pattern-type-qualifier-segment-list
  : pattern-type-qualifier-segment-list pattern-type-qualifier
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | pattern-type-qualifier
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

pattern-type-qualifier
  : '+' type-qualifiers-list
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, &$2);
  }
  | '~' type-qualifiers-list
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_NOT, &$2);
  }
  ;

type-qualifiers-list
  : type-qualifiers-list type-qualifiers
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | type-qualifiers
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

type-qualifiers
  : '(' type-qualifier-segments ')'
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$2->lln);
    pattern_group_mk(&$$, &@$, &list, NULL, 0, ++PARSER->group_counter, ++PARSER->capture_group_counter);
    $$->group.has_type_qualifiers = true;
    PARSER->section_has_type_qualifiers = true;
  }
  ;

type-qualifier-segments
  : type-qualifier-segment-list
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, &$1);
  }
  ;

type-qualifier-segment-list
  : type-qualifier-segment-list type-qualifier
  {
    $$ = $1;
    llist_push(&$$, &$2->lln);
  }
  | type-qualifier
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

type-qualifier
  : '/'
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_DIR);
  }
  | 'F'
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_DIR_NONEMPTY);
  }
  | '.'
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_FILE);
  }
  | DOT
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_FILE);
  }
  | '@'
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_SYMLINK);
  }
  | '^' 'A'
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_ANCHOR);
  }
  | '^' 'M'
  {
    pattern_qualifier_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_MODULE);
  }
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
  : unquoted-strpart
  | escape
  ;

unquoted-strpart
  : word
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
