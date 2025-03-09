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
  #include "pattern/qualifier.h"

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

%type <yyu.umax>   uint16

/* nonterminals */
%type <list> alternation-pattern-parts
%type <list> alternation-parts
%type <list> quoted-strparts quoted-string-literal string-literal
%type <list> pattern-sections pattern-absolute-sections-list pattern-relative-sections-list
%type <list> pattern-dentry-segment-list
%type <list> class-segments-list

%type <yyu.umax> type-qualifiers type-qualifier-list type-qualifier

%type <pattern> pattern

%type <section> pattern-section pattern-section-self
%type <section> pattern-absolute-initial-section
%type <section> pattern-section-fs-child
%type <section> sysvar-section
%type <nodeset> sysvar-token

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
  : pattern-sections type-qualifiers
  {
    pattern_section *last;
    last = llist_last(&$1, typeof(*last), lln);
    last->type_qualifiers_mask = $2;
    pattern_mk(&$$, &@$, &$1);
  }
  | pattern-sections
  {
    pattern_mk(&$$, &@$, &$1);
  }
  ;

/* generate pattern may have one type qualifier following the last segment
 * this controls what kind of file nodes it generates */

type-qualifiers
  : '+' '(' type-qualifier-list ')'
  {
    $$ = $3;
  }
  ;

type-qualifier-list
  : type-qualifier
  ;

type-qualifier
  : '/'
  {
    $$ = PATTERN_QUALIFIER_TYPE_DIR;
  }
  | '.'
  {
    $$ = PATTERN_QUALIFIER_TYPE_FILE;
  }
  | DOT
  {
    $$ = PATTERN_QUALIFIER_TYPE_FILE;
  }
  | '@'
  {
    $$ = PATTERN_QUALIFIER_TYPE_SYMLINK;
  }
  ;

pattern-sections
  : pattern-absolute-initial-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  | sysvar-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
  }
  | pattern-absolute-sections-list
  | pattern-relative-sections-list
  ;

pattern-absolute-sections-list
  : pattern-absolute-sections-list '/' pattern-section
  {
    $$ = $1;
    llist_push(&$$, &$3->lln);
  }
  | sysvar-section '/' pattern-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
    llist_push(&$$, &$3->lln);
  }
  | pattern-absolute-initial-section pattern-section
  {
    llist_init(&$$);
    llist_push(&$$, &$1->lln);
    llist_push(&$$, &$2->lln);
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
  }
  ;

pattern-absolute-initial-section
  : '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_ROOT, 0, 0, NULL, 0);
  }
  | '/' '/'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SHADOW, 0, 0, NULL, 0);
  }
  ;

sysvar-section
  : '$' sysvar-token
  {
    pattern_section_mk(&$$, &@$, $2, 0, 0, NULL, false);
  }
  | '$' '{' sysvar-token '}'
  {
    pattern_section_mk(&$$, &@$, $3, 0, 0, NULL, false);
  }
  ;

sysvar-token
  : BM_ANCHOR_REL
  {
    $$ = PATTERN_NODESET_ANCHOR_REL;
  }
  | '^' 'A'
  {
    $$ = PATTERN_NODESET_ANCHOR_REL;
  }
  | BM_BUILD_REL
  {
    $$ = PATTERN_NODESET_BUILD_REL;
  }
  | '^' 'D'
  {
    $$ = PATTERN_NODESET_BUILD_REL;
  }
  | BM_ANCHOR
  {
    $$ = PATTERN_NODESET_ANCHOR;
  }
  | BM_BUILD
  {
    $$ = PATTERN_NODESET_BUILD;
  }
  | BM_BASEDIR
  {
    $$ = PATTERN_NODESET_BASEDIR;
  }
  | BM_HOMEDIR
  {
    $$ = PATTERN_NODESET_HOMEDIR;
  }
  | BM_IPCDIR
  {
    $$ = PATTERN_NODESET_IPCDIR;
  }
  ;

pattern-section
  : pattern-section-fs-child
  | pattern-section-self
  ;

pattern-section-self
  : DOT
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SELF, 0, 0, NULL, 0);
  }
  | '.'
  {
    pattern_section_mk(&$$, &@$, PATTERN_NODESET_SELF, 0, 0, NULL, 0);
  }
  ;

pattern-section-fs-child
  : pattern-dentry-segments
  {
    llist list;
    llist_init(&list);
    llist_push(&list, &$1->lln);
    pattern_section_mk(&$$, &@$, 0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN, &list, 0);
  }
  ;

pattern-dentry-segments
  : pattern-dentry-segment-list
  {
    pattern_segments_mk(&$$, &@$, 0, 0, &$1);
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
    pattern_segments_mk(&$$, &@$, 0, 0, &list);
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
    pattern_segments_mk(&$$, &@$, 0, 0, &$1);
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
  | 'F'
  | '@'
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
