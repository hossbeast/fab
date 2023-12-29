/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <string.h>

  #include "types.h"

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/hashtable.h"
  #include "bacon/parser.h"
  #include "bacon/make.h"
  #include "lorien/path_normalize.h"

  #include "config_parser/glue.h"
  #include "config_parser.h"
  #include "config.h"
  #include "configuration.h"
  #include "filesystem.h"
  #include "build.h"
  #include "macros.h"
  #include "pattern.h"

  struct pattern;
  struct bacon_value;
}

%code top {
  int config_yylex(void*, void*, void*);
  #define PARSER ((config_parser*)parser)
  #define CONFIG_YYLTYPE yyu_location

  #include "config_parser.h"
  #include "bacon/value.h"

  static void replace(config_parser *parser, struct bacon_value ** dst, struct bacon_value *val)
  {
    bacon_value_free(parser->bacon_parser, *dst);
    *dst = val;
  }
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {config_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval  yyu;

  struct pattern *pattern;
  struct bacon_value *val;

  // enums
  struct {
    struct bacon_value *v;
    invalidate_type e;
  } invalidate_type;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'

/* terminals */
%token            STR           300 "unquoted-string"
%token <yyu.u8>   CREF          301 "character-escape"
%token <yyu.u8>   HREF          302 "hex-escape"
%token <yyu.b>    BOOL          303 "boolean"
%token <yyu.f>    FLOAT         304 "float"
%token <yyu.t>    VARIABLE      305 "variable"
%token <yyu.umax> UINTMAX8      306
%token <yyu.umax> UINTMAX16     307
%token <yyu.umax> UINTMAX32     308
%token <yyu.umax> UINTMAX64     309
%token <yyu.imax> HEX8          310
%token <yyu.imax> HEX16         311
%token <yyu.imax> HEX32         312
%token <yyu.imax> HEX64         313
%token <yyu.imax> INTMIN8       314
%token <yyu.imax> INTMIN16      315
%token <yyu.imax> INTMIN32      316
%token <yyu.imax> INTMIN64      317
%token <yyu.imax> INTMAX8       318
%token <yyu.imax> INTMAX16      319
%token <yyu.imax> INTMAX32      320
%token <yyu.imax> INTMAX64      321

%type <yyu.umax>  uint16_igroup
%type <yyu.umax>  uint16_ugroup

%token
 BUILD                    "build"
 CONCURRENCY              "concurrency"
 FILESYSTEMS              "filesystems"
 FORMULA                  "formula"
 INVALIDATE               "invalidate"
 PATH                     "path"
 COPY_FROM_ENV            "copy-from-env"
 DIRS                     "dirs"
 STAT                     "stat"
 CONTENT                  "content"
 NOTIFY                   "notify"
 ALWAYS                   "always"
 NEVER                    "never"
 WALKER                   "walker"
 EXCLUDE                  "exclude"
 SPECIAL                  "special"
 INCLUDE                  "include"
 MKDIRS                   "mkdirs"
 READLINKS                "readlinks"
 INDEXED                  "indexed"

%token
 <pattern> INCLUDE_PATTERN  "include-pattern"
 <pattern> MATCH_PATTERN    "match-pattern"

/* nonterminals */
%type <val> quote_words quote_word quoted_string unquoted_string
%type <val> bool uint16 string

/* enums */
%type <invalidate_type> invalidate-type

%destructor { pattern_free($$); } <pattern>
%destructor { bacon_value_free(PARSER->bacon_parser, $$); } <val>

%%
utterance
  : config
  {
    if(PARSER->yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->yyu, &@1);
      YYACCEPT;
    }
  }
  ;

config
  : '{' sections '}'
  | sections
  | %empty
  ;

sections
  : sections section
  | section
  ;

section
  : build-section
  | filesystems-section
  | formula-section
  | walker-section
  ;

build-section
  : BUILD ':' '{' build-mappings-epsilon '}'
  ;

build-mappings-epsilon
  : build-mappings
  | %empty
  ;

build-mappings
  : build-mappings build-mapping
  | build-mapping
  ;

build-mapping
  : CONCURRENCY ':' uint16
  {
    replace(PARSER, &PARSER->cfg->build.concurrency.v, $3);
    PARSER->cfg->build.concurrency.u16 = $3->i;
  }
  ;

formula-section
  : FORMULA ':' '{' formula-mappings '}'
  | FORMULA ':' '{' '}'
  ;

formula-mappings
  : formula-mappings formula-mapping
  | formula-mapping
  ;

formula-mapping
  : formula-path
  ;

formula-path
  : PATH ':' '{' formula-path-mappings '}'
  | PATH ':' '{' '}'
  ;

formula-path-mappings
  : formula-path-mappings formula-path-mapping
  | formula-path-mapping
  ;

formula-path-mapping
  : COPY_FROM_ENV ':' bool
  {
    PARSER->cfg->formula.path.copy_from_env.v = $3;
    PARSER->cfg->formula.path.copy_from_env.b = $3->b;
  }
  | DIRS ':' '{' formula-path-dirs '}'
  | DIRS ':' '{' '}'
  ;

formula-path-dirs
  : formula-path-dirs formula-path-dir
  | formula-path-dir
  ;

formula-path-dir
  : string
  {
    config_parser_fpde_alloc(PARSER, $1);
  }
  ;

filesystems-section
  : FILESYSTEMS ':' '{' filesystems-mappings-epsilon '}'
  ;

filesystems-mappings-epsilon
  : filesystems-mappings
  | %empty
  ;

filesystems-mappings
  : filesystems-mappings filesystems-mapping
  | filesystems-mapping
  ;

filesystems-mapping
  : string ':' '{' { config_parser_fm_alloc(PARSER, $1); } filesystems-entry-mappings '}'
  {
    PARSER->fm = 0;
    bacon_value_free(PARSER->bacon_parser, $1);
  }
  ;

filesystems-entry-mappings
  : filesystems-entry-mappings filesystems-entry-mapping
  | filesystems-entry-mapping
  ;

filesystems-entry-mapping
  : INVALIDATE ':' invalidate-type
  {
    replace(PARSER, &PARSER->fm->val.invalidate.v, $3.v);
    PARSER->fm->val.invalidate.e = $3.e;
  }
  | MKDIRS ':' bool
  {
    replace(PARSER, &PARSER->fm->val.mkdirs.v, $3);
    PARSER->fm->val.mkdirs.b = $3->b;
  }
  | READLINKS ':' bool
  {
    replace(PARSER, &PARSER->fm->val.readlinks.v, $3);
    PARSER->fm->val.readlinks.b = $3->b;
  }
  | INDEXED ':' bool
  {
    replace(PARSER, &PARSER->fm->val.indexed.v, $3);
    PARSER->fm->val.indexed.b = $3->b;
  }
  ;

walker-section
  : WALKER ':' '{' walker-settings-epsilon '}'
  ;

walker-settings-epsilon
  : walker-settings
  | %empty
  ;

walker-settings
  : walker-settings walker-setting
  | walker-setting
  ;

walker-setting
  : INCLUDE ':' '{' walker-include-entry-list-epsilon '}'
  | EXCLUDE ':' '{' walker-exclude-entry-list-epsilon '}'
  | SPECIAL ':' '{' walker-special-entry-list-epsilon '}'
  ;

walker-exclude-entry-list-epsilon
  : walker-exclude-entry-list
  | %empty
  ;

walker-exclude-entry-list
  : walker-exclude-entry-list walker-exclude-entry
  | walker-exclude-entry
  ;

walker-exclude-entry
  : MATCH_PATTERN
  {
    config_parser_we_put(PARSER, $1);
  }
  ;

walker-include-entry-list-epsilon
  : walker-include-entry-list
  | %empty
  ;

walker-include-entry-list
  : walker-include-entry-list walker-include-entry
  | walker-include-entry
  ;

walker-include-entry
  : INCLUDE_PATTERN
  {
    config_parser_wi_put(PARSER, $1);
  }
  ;

walker-special-entry-list-epsilon
  : walker-special-entry-list
  | %empty
  ;

walker-special-entry-list
  : walker-special-entry-list walker-special-entry
  | walker-special-entry
  ;

walker-special-entry
  : string
  {
    config_parser_wse_alloc(PARSER, $1);
  }
  ;

invalidate-type
  : STAT
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATE_STAT;
  }
  | CONTENT
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATE_CONTENT;
  }
  | NOTIFY
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATE_NOTIFY;
  }
  | ALWAYS
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATE_ALWAYS;
  }
  | NEVER
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATE_NEVER;
  }
  ;

bool
  : BOOL
  {
    bacon_boolean_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

uint16
  : uint16_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  | uint16_ugroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

uint16_igroup
  : INTMIN16
  {
    $$ = $1;
  }
  | INTMIN8
  {
    $$ = $1;
  }
  | INTMAX8
  {
    $$ = $1;
  }
  | INTMAX16
  {
    $$ = $1;
  }
  ;

uint16_ugroup
  : UINTMAX8
  | UINTMAX16
  ;

string
  : unquoted_string
  | quoted_string
  ;

/* must be a single token because strings can appear repeatedly together */
unquoted_string
  : word
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

word
  : STR
  | '/'
  | '-'
  | '+'
  | '='
  | ','
  ;

quoted_string
  : '"' quote_words '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$ = 0;
  }
  ;

quote_words
  : quote_words quote_word
  {
    bacon_string_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
    bacon_value_free(PARSER->bacon_parser, $2);
  }
  | quote_word
  ;

quote_word
  : STR
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  | CREF
  {
    bacon_string_mkc(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  | HREF
  {
    bacon_string_mkc(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;
