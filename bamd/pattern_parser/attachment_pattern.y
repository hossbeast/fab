/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <string.h>

  #include "types.h"

  #include "pattern/byte.h"
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
  #define PARSER containerof(parser, pattern_parser, attachment_yyu)
  #define ATTACHMENT_PATTERN_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {attachment_pattern_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); (void)yynerrs; }

%union {
  yyu_lval yyu;
  pattern * pattern;
  pattern_segments * segments;
  pattern_section * section;
  pattern_segment * segment;
  llist list;
  enum pattern_nodeset nodeset;
}

/* tokens from pattern.l */
%token '(' ')'      // groups
%token '{' '}'      // alternations
%token '[' ']'      // classes
%token ':'
%token '?'          // capture group name
%token '~'          // not-qualifiers
%token '+'          // and-qualifiers
%token '$'          // variable
%token '.'          // file qualifier
%token '@'          // symlink qualifier

%token <yyu.imax> INTMAX8       306
%token <yyu.imax> INTMAX16      307
%token <yyu.imax> INTMAX32      308
%token <yyu.imax> INTMAX64      309
%token <yyu.umax> UINTMAX8      310
%token <yyu.umax> UINTMAX16     311
%token <yyu.umax> UINTMAX32     312
%token <yyu.umax> UINTMAX64     313
%token <yyu.umax> HEX8          314
%token <yyu.umax> HEX16         315
%token <yyu.umax> HEX32         316
%token <yyu.umax> HEX64         317

/* terminals */
%token             STR        204 "string"
%token <yyu.u8>    CREF       205 "character-escape"
%token <yyu.u8>    HREF       206 "hex-escape"
%token             STARSTAR   207 "**"
%token             DOT        208 "."
%token             DOT2       209 ".."
%token <yyu.u8>    CHAR       210
%token MODULE                 211 "module"
%token MODULES                212 "modules"
%token SHADOW                 213 "shadow"
%token UP                     214 "up"
%token DOWN                   215 "down"
%token SELF_OR_BELOW          216 "self-or-below"
%token DIRS                   217 "dirs"

 /* nodesets - global sysvars */
%token BM_BASEDIR              218 "bm_basedir"
%token BM_HOMEDIR              219 "bm_homedir"
%token BM_IPCDIR               220 "bm_ipcdir"
 /* nodesets - module sysvars */
%token BM_ANCHOR_REL           221 "bm_anchor_rel"
%token BM_BUILD_REL            222 "bm_build_rel"
%token BM_ANCHOR               223 "bm_anchor"
%token BM_BUILD                224 "bm_build"
 /* segment - global sysvars */
%token BM_BAMD_ID              225 "bm_bamd_id"
%token BM_BAMD_PID             226 "bm_bamd_pid"
 /* segment - module sysvars */
%token BM_MODULE_ID            227 "bm_module_id"
%token BM_MODULE_NAME          228 "bm_module_name"
%token BM_MODULE_BUILDDIR_NAME 229 "bm_module_builddir_name"

/* nonterminals */
%type <list> quoted-strparts quoted-string-literal unquoted-string-literal string-literal
%type <list> pattern-absolute-shadow-sections-list
%type <list> pattern-dentry-segments-list
%type <list> pattern-dentry-segment-list

%type <pattern> pattern

%type <segments> pattern-dentry-segments

%type <section> pattern-section pattern-absolute-shadow-initial-section

%type <segment> escape
%type <segment> pattern-dentry-segment
%type <segment> word
%type <segment> variable
%type <segment> quoted-strpart
%type <segment> unquoted-strpart

%destructor { pattern_free($$); } <pattern>
%destructor { pattern_section_free($$); } <section>
%destructor { pattern_segment_free($$); } <segment>
%destructor { pattern_segments_free($$); } <segments>

%%
utterance
  : pattern
  {
    PARSER->pattern = $1;
    if(PARSER->attachment_yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->attachment_yyu, &@1);
      YYACCEPT;
    }
  }
  ;

pattern
  : pattern-absolute-shadow-sections-list
  {
    pattern_mk(&$$, &@$, &$1);
  }
  | pattern-absolute-shadow-initial-section
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$1->lln);
    pattern_mk(&$$, &@$, &list);
  }
  ;

pattern-absolute-shadow-sections-list
  : pattern-absolute-shadow-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | pattern-absolute-shadow-initial-section pattern-section
  { 
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
    llist_push(&$$, &$2->lln);
  }
  ;

pattern-absolute-shadow-initial-section
  : '/' '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SHADOW, 0, 0, NULL, 0);
  }
  ;

pattern-section
  : pattern-dentry-segments-list
  {
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, &$1, 0);
  }
  | DOT2
  {
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_UP, NULL, 0);
  }
  | '.'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SELF, 0, 0, NULL, 0);
  }
  | DOT
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SELF, 0, 0, NULL, 0);
  }
  ;

pattern-dentry-segments-list
  : pattern-dentry-segments
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  ;

pattern-dentry-segments
  : pattern-dentry-segment-list
  {
    pattern_segments_mk(&$$, &@$, PATTERN_QUALIFIER_TYPE_AND, 0, &$1);
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
  : variable
  ;

variable
  : '$' word-token
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NAME, 0, @2.s, @2.l);
  }
  | '$' '{' word-token '}'
  {
    pattern_variable_mk(&$$, &@$, PATTERN_VARIABLE_TYPE_NAME, 0, @3.s, @3.l);
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
  : word-token
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
  | '@'
  | '+'
  | INTMAX8
  | INTMAX16
  | UINTMAX8
  | UINTMAX16
  /* segment sysvars */
  | BM_MODULE_ID
  | BM_BAMD_ID
  | BM_BAMD_PID
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
