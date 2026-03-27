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

  #include "configset_parser/glue.h"
  #include "configset_parser.h"
  #include "configset.h"
  #include "config.h"
  #include "filesystem.h"
  #include "build.h"
  #include "macros.h"
  #include "pattern.h"

  struct pattern;
  struct bacon_value;
}

%code top {
  int configset_yylex(void*, void*, void*);
  #define PARSER ((configset_parser*)parser)
  #define CONFIGSET_YYLTYPE yyu_location

  #include "configset_parser.h"
  #include "bacon/value.h"

  static void replace(configset_parser *parser, struct bacon_value ** dst, struct bacon_value *val)
  {
    bacon_value_free(parser->bacon_parser, *dst);
    *dst = val;
  }
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {configset_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval  yyu;

  struct pattern *pattern;
  struct bacon_value *val;
  struct pattern *pattern_owned;

  // enums
  struct {
    struct bacon_value *v;
    invalidation_type e;
  } invalidation_type;
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

%type <yyu.imax>  uint16_igroup int64_igroup
%type <yyu.umax>  uint16_ugroup int64_ugroup

%token
 BUILD                    "build"
 CONCURRENCY              "concurrency"
 FILESYSTEM               "filesystem"
 FORMULA                  "formula"
 INVALIDATION             "invalidation"
 PATH                     "path"
 COPY_FROM_ENV            "copy-from-env"
 DIRS                     "dirs"
 STAT                     "stat"
 CONTENT                  "content"
 NOTIFY                   "notify"
 ALWAYS                   "always"
 NEVER                    "never"
 FSTREES                  "fstrees"
 EXCLUDE                  "exclude"
 SPECIAL                  "special"
 INCLUDE                  "include"
 MKDIR                    "mkdir"
 READLINK                 "readlink"
 INDEX                    "index"
 MOUNT                    "mount"
 TIMEOUT                  "timeout"
 INTERVAL                 "interval"
 PLUGINS                  "plugins"
 ARGS                     "args"
 ENVS                     "envs"
 REFRESH_ON_REQUEST       "refresh-on-request"
 REFRESH_ON_AUTOBUILD     "refresh-on-autobuild"

%token
 <pattern> INCLUDE_PATTERN  "include-pattern"
 <pattern> MATCH_PATTERN    "match-pattern"

/* nonterminals */
%type <val> quote_words quote_word quoted_string unquoted_string
%type <val> bool uint16 int64 string

/* enums */
%type <invalidation_type> invalidation-type

%type <pattern_owned> fstree-entry-key

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
  | filesystem-section
  | formula-section
  | timeout-section
  | plugins-section
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
    configset_parser_fpde_alloc(PARSER, $1);
  }
  ;

filesystem-section
  : FILESYSTEM ':' '{' filesystem-mappings '}'
  ;

filesystem-mappings
  : filesystem-mappings-list
  | %empty
  ;

filesystem-mappings-list
  : filesystem-mappings-list filesystem-mapping
  | filesystem-mapping
  ;

filesystem-mapping
  : FSTREES ':' '{' fstree-entries '}'
  | SPECIAL ':' '{' special-entries '}'
  | EXCLUDE ':' '{' exclude-entries '}'
  ;

fstree-entries
  : fstree-entry-list
  | %empty
  ;

fstree-entry-list
  : fstree-entry-list fstree-entry
  | fstree-entry
  ;

fstree-entry-key
  : INCLUDE_PATTERN
  {
    $$ = $1;
  }
  ;

fstree-entry
  : fstree-entry-key { configset_parser_fsm_alloc(PARSER, $1); } fstree-entry-value
  {
    PARSER->fsm = 0;
  }
  ;

fstree-entry-value
  : ':' '{' fstree-entry-mappings '}'
  | %empty
  ;

fstree-entry-mappings
  : fstree-entry-mapping-list
  | %empty
  ;

fstree-entry-mapping-list
  : fstree-entry-mapping-list fstree-entry-mapping
  | fstree-entry-mapping
  ;

fstree-entry-mapping
  : INVALIDATION ':' invalidation-type
  {
    replace(PARSER, &PARSER->fsm->val.invalidation.v, $3.v);
    PARSER->fsm->val.invalidation.e = $3.e;
  }
  | MKDIR ':' bool
  {
    replace(PARSER, &PARSER->fsm->val.mkdir.v, $3);
    PARSER->fsm->val.mkdir.b = $3->b;
  }
  | READLINK ':' bool
  {
    replace(PARSER, &PARSER->fsm->val.readlink.v, $3);
    PARSER->fsm->val.readlink.b = $3->b;
  }
  | INDEX ':' bool
  {
    replace(PARSER, &PARSER->fsm->val.index.v, $3);
    PARSER->fsm->val.index.b = $3->b;
  }
  | MOUNT ':' bool
  {
    replace(PARSER, &PARSER->fsm->val.mount.v, $3);
    PARSER->fsm->val.mount.b = $3->b;
  }
  ;

timeout-section
  : TIMEOUT ':' '{' timeout-settings-epsilon '}'
  ;

timeout-settings-epsilon
  : timeout-settings
  | %empty
  ;

timeout-settings
  : timeout-settings timeout-setting
  | timeout-setting
  ;

timeout-setting
  : INTERVAL_SEC ':' int64
  {
    replace(PARSER, &PARSER->cfg->timeout.interval.v, $3);
    PARSER->cfg->timeout.interval.i64 = $3->i;
  }
  | REFRESH_ON_REQUEST ':' boolean
  {
    replace(PARSER, &PARSER->cfg->timeout.refresh_on_request.v, $3);
    PARSER->cfg->timeout.refresh_on_request.b = $3->b;
  }
  | REFRESH_ON_AUTOBUILD ':' boolean
  {
    replace(PARSER, &PARSER->cfg->timeout.refresh_on_autobuild.v, $3);
    PARSER->cfg->timeout.refresh_on_autobuild.b = $3->b;
  }
  ;

plugins-section
  : PLUGINS ':' '{' plugin-sections-list '}'
  ;

plugin-sections-list
  : plugin-sections
  | %empty
  ;

plugin-sections
  : plugin-sections plugin-section
  | plugin-section
  ;

plugin-section
  : plugin-path-section
  | plugin-list-section
  ;

plugin-path-section
  : PATH ':' '{' plugin-path-settings '}'
  ;

plugin-path-settings
  : DIRS ':' '{' plugin-path-dirs '}'
  ;

plugin-path-dirs
  : plugin-path-dirs plugin-path-dir
  | plugin-path-dir
  ;

plugin-path-dir
  : string
  {
    configset_parser_ppde_alloc(PARSER, $1);
  }
  ;

plugin-list-section
  : '[' plugin-entry-list ']'
  ;

plugin-entry-list
  : %empty
  | plugin-entries
  ;

plugin-entries
  : plugin-entries plugin-entry
  | plugin-entry
  ;

plugin-entry
  : '{' { PARSER->pe = configset_parser_plugin_alloc(PARSER); } plugin-entry-mappings '}'
  {
    configset_parser_plugin_put(PARSER, PARSER->pe);
    PARSER->pe = 0;
  }
  ;

plugin-entry-mappings
  : plugin-entry-mappings plugin-entry-mapping
  | plugin-entry-mapping
  ;

plugin-entry-mapping
  : PATH ':' string
  {
    replace(PARSER, &PARSER->pe->path.v, $3);
    PARSER->pe->path.s = $3->s;
    PARSER->pe->path.len = $3->len;
  }
  | ARGS ':' '[' plugin-entry-args ']'
  ;

plugin-entry-args
  : plugin-entry-args plugin-entry-arg
  | plugin-entry-arg
  ;

plugin-entry-arg
  : string
  {
    configset_parser_plugin_arg_put(PARSER, $1);
  }
  ;

exclude-entries
  : exclude-entry-list
  | %empty
  ;

exclude-entry-list
  : exclude-entry-list exclude-entry
  | exclude-entry
  ;

exclude-entry
  : MATCH_PATTERN
  {
    configset_parser_we_put(PARSER, $1);
  }
  ;

special-entries
  : special-entry-list
  | %empty
  ;

special-entry-list
  : special-entry-list special-entry
  | special-entry
  ;

special-entry
  : string
  {
    configset_parser_wse_alloc(PARSER, $1);
  }
  ;

invalidation-type
  : STAT
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATION_STAT;
  }
  | CONTENT
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATION_CONTENT;
  }
  | NOTIFY
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATION_NOTIFY;
  }
  | ALWAYS
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATION_ALWAYS;
  }
  | NEVER
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = INVALIDATION_NEVER;
  }
  ;

bool
  : BOOL
  {
    bacon_boolean_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

int64
  : uint16
  | int64_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  | int64_ugroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
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
  : INTMIN8  { $$ = $1; }
  | INTMAX8  { $$ = $1; }
  | INTMIN16 { $$ = $1; }
  | INTMAX16 { $$ = $1; }
  ;

uint16_ugroup
  : UINTMAX8
  | UINTMAX16
  ;

int64_igroup
  : INTMIN32 { $$ = $1; }
  | INTMAX32 { $$ = $1; }
  | INTMIN64 { $$ = $1; }
  | INTMAX64 { $$ = $1; }
  ;

int64_ugroup
  : UINTMAX32
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
