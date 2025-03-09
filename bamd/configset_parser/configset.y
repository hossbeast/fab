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
  #include "exec_value.h"

  struct pattern;
  struct bacon_value;
  struct lookup_pattern;
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

  struct include_pattern *include_pattern;
  struct lookup_pattern *lookup_pattern;
  struct match_pattern *match_pattern;
  struct bacon_value *value;
  struct include_pattern *pattern_owned;

  // enums
  struct {
    struct bacon_value *v;
    invalidation_type e;
  } invalidation_type;

  struct {
    struct bacon_value *v;
    readlink_type e;
  } readlink_type;
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
%token <yyu.umax> HEX8          310
%token <yyu.umax> HEX16         311
%token <yyu.umax> HEX32         312
%token <yyu.umax> HEX64         313
%token <yyu.imax> INTMIN8       314
%token <yyu.imax> INTMIN16      315
%token <yyu.imax> INTMIN32      316
%token <yyu.imax> INTMIN64      317
%token <yyu.imax> INTMAX8       318
%token <yyu.imax> INTMAX16      319
%token <yyu.imax> INTMAX32      320
%token <yyu.imax> INTMAX64      321

 /* global sysvars */
%token BM_BASEDIR               322 "bm_basedir"
%token BM_HOMEDIR               323 "bm_homedir"
%token BM_IPCDIR                324 "bm_ipcdir"
%token BM_BAMD_ID               325 "bm_bamd_id"
%token BM_BAMD_PID              326 "bm_bamd_pid"

%type <yyu.imax> uint16_igroup uint32_igroup int64_igroup
%type <yyu.umax> uint16_ugroup

%type <yyu.umax> posint_ugroup
%type <yyu.imax> posint_igroup
%type <yyu.imax> negint_igroup

%token
 BUILD                    "build"
 CONCURRENCY              "concurrency"
 FILESYSTEM               "filesystem"
 INVALIDATION             "invalidation"
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
 UNLINK                   "unlink"
 READLINK                 "readlink"
 YES                      "yes"
 NO                       "no"
 REQUIRED                 "required"
 MOUNT                    "mount"
 TIMEOUT                  "timeout"
 INTERVAL_MSEC            "interval-msec"
 INTERVAL_SEC             "interval-sec"
 PLUGINS                  "plugins"
 MODULES                  "modules"
 VARSETS                  "varsets"
 RULESETS                 "rulesets"
 NAME                     "name"
 FILETOKEN                "file"
 ARGS                     "args"
 ENVS                     "envs"
 LOOKUP                   "lookup"
 PATTERN                  "pattern"
 NAMESPACE                "namespace"
 NAMESPACES               "namespaces"
 REFRESH_ON_REQUEST       "refresh-on-request"
 REFRESH_ON_AUTOBUILD     "refresh-on-autobuild"
 STDIN_BUFFER_SIZE        "stdin-buffer-size"
 STDOUT_BUFFER_SIZE       "stdout-buffer-size"
 STDERR_BUFFER_SIZE       "stderr-buffer-size"
 AUXOUT_BUFFER_SIZE       "auxout-buffer-size"
 RETENTION                "retention"
 MAX_ENTRIES              "max-entries"

%token
 <include_pattern> INCLUDE_PATTERN     "include-pattern"
 <match_pattern> MATCH_PATTERN         "match-pattern"
 <lookup_pattern> LOOKUP_PATTERN       "lookup-pattern"

/* nonterminals */
%type <value> quote_words quote_word quoted_string unquoted_string
%type <value> uint16 uint32 int64 string
%type <value> namespaces

%type <value> primitive boolean float negint posint variable
%type <value> exec-value exec-value-lookup
%type <value> plugin-entry-args
%type <value> plugin-entry-envs
%type <value> plugin-entry-env

/* enums */
%type <invalidation_type> invalidation-type
%type <readlink_type> readlink-type

%type <pattern_owned> fstree-entry-key

%destructor { pattern_free(&$$->pattern); } <match_pattern>
%destructor { pattern_free(&$$->pattern); } <include_pattern>
%destructor { pattern_free(&$$->pattern); } <lookup_pattern>
%destructor { bacon_value_free(PARSER->bacon_parser, $$); } <value>

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
  | timeout-section
  | plugins-section
  | modules-section
  | notify-section
  | namespaces-section
  ;

build-section
  : BUILD ':' '{' build-mappings '}'
  ;

build-mappings
  : build-mappings-list
  | %empty
  ;

build-mappings-list
  : build-mappings-list build-mapping
  | build-mapping
  ;

build-mapping
  : CONCURRENCY ':' uint16
  {
    replace(PARSER, &PARSER->cfg->build.concurrency.v, $3);
    PARSER->cfg->build.concurrency.u16 = $3->i;
  }
  | STDOUT_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->build.stdout_buffer_size.v, $3);
    PARSER->cfg->build.stdout_buffer_size.u32 = $3->i;
  }
  | STDERR_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->build.stderr_buffer_size.v, $3);
    PARSER->cfg->build.stderr_buffer_size.u32 = $3->i;
  }
  | AUXOUT_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->build.auxout_buffer_size.v, $3);
    PARSER->cfg->build.auxout_buffer_size.u32 = $3->i;
  }
  | RETENTION ':' '{' build-retention-mappings '}'
  ;

build-retention-mappings
  : build-retention-mappings-list
  | %empty
  ;

build-retention-mappings-list
  : build-retention-mappings-list build-retention-mapping
  | build-retention-mapping
  ;

build-retention-mapping
  : MAX_ENTRIES ':' uint32
  {
    replace(PARSER, &PARSER->cfg->build.retention.max_entries.v, $3);
    PARSER->cfg->build.retention.max_entries.u32 = $3->i;
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
  | MKDIR ':' boolean
  {
    replace(PARSER, &PARSER->fsm->val.mkdir.v, $3);
    PARSER->fsm->val.mkdir.b = $3->b;
  }
  | UNLINK ':' boolean
  {
    replace(PARSER, &PARSER->fsm->val.unlink.v, $3);
    PARSER->fsm->val.unlink.b = $3->b;
  }
  | READLINK ':' readlink-type
  {
    replace(PARSER, &PARSER->fsm->val.readlink.v, $3.v);
    PARSER->fsm->val.readlink.e = $3.e;
  }
  | MOUNT ':' boolean
  {
    replace(PARSER, &PARSER->fsm->val.mount.v, $3);
    PARSER->fsm->val.mount.b = $3->b;
  }
  | NAMESPACES ':' '{' namespaces '}'
  {
    replace(PARSER, &PARSER->fsm->val.namespaces.list, $4);
  }
  ;

readlink-type
  : YES
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = READLINK_YES;
  }
  | NO
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = READLINK_NO;
  }
  | REQUIRED
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$.v, @1.s, @1.l);
    $$.e = READLINK_REQUIRED;
  }
  ;

namespaces
  : namespaces unquoted_string
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | unquoted_string
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

notify-section
  : NOTIFY ':' '{' notify-settings-set '}'
  ;

notify-settings-set
  : notify-settings
  | %empty
  ;

notify-settings
  : notify-settings notify-setting
  | notify-setting
  ;

notify-setting
  : INTERVAL_MSEC ':' uint32
  {
    replace(PARSER, &PARSER->cfg->notify.interval.v, $3);
    PARSER->cfg->notify.interval.u32 = $3->i;
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
  : plugin-list-section
  | plugin-mapping
  ;

plugin-mapping
  : STDIN_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->plugins.stdin_buffer_size.v, $3);
    PARSER->cfg->plugins.stdin_buffer_size.u32 = $3->i;
  }
  | STDOUT_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->plugins.stdout_buffer_size.v, $3);
    PARSER->cfg->plugins.stdout_buffer_size.u32 = $3->i;
  }
  | STDERR_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->plugins.stderr_buffer_size.v, $3);
    PARSER->cfg->plugins.stderr_buffer_size.u32 = $3->i;
  }
  | AUXOUT_BUFFER_SIZE ':' uint32
  {
    replace(PARSER, &PARSER->cfg->plugins.auxout_buffer_size.v, $3);
    PARSER->cfg->plugins.auxout_buffer_size.u32 = $3->i;
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
  : NAME ':' string
  {
    bacon_value_free(PARSER->bacon_parser, PARSER->pe->name);
    PARSER->pe->name= $3;
  }
  | FILETOKEN ':' '{' exec-value-lookup '}'
  {
    bacon_value_free(PARSER->bacon_parser, PARSER->pe->file.lookup);
    PARSER->pe->file.lookup = $4;
  }
  | ARGS ':' '[' plugin-entry-args ']'
  {
    PARSER->pe->args = $4;
  }
  | ENVS ':' '{' plugin-entry-envs '}'
  {
    PARSER->pe->envs = $4;
  }
  ;

plugin-entry-args
  : plugin-entry-args exec-value
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | exec-value
  {
    bacon_list_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

plugin-entry-envs
  : plugin-entry-envs plugin-entry-env
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, $1, &$$, $2);
  }
  | plugin-entry-env
  {
    bacon_set_mkv(PARSER->bacon_parser, &@$, 0, &$$, $1);
  }
  ;

plugin-entry-env
  : unquoted_string ':' exec-value
  {
    bacon_mapping_mk(PARSER->bacon_parser, &@$, &$$, $1, $3);
  }
  ;

modules-section
  : MODULES ':' '{' module-sections '}'
  ;

module-sections
  : module-section-list
  | %empty
  ;

module-section-list
  : module-section-list module-section
  | module-section
  ;

module-section
  : module-varsets-section
  | module-rulesets-section
  ;

module-varsets-section
  : VARSETS ':' '{' module-varsets '}'
  ;

module-varsets
  : module-varset-list
  | %empty
  ;

module-varset-list
  : module-varset-list module-varset
  | module-varset
  ;

module-varset
  : string
  {
    configset_parser_varsets_alloc(PARSER, $1);
  }
  ;

module-rulesets-section
  : RULESETS ':' '{' module-rulesets '}'
  ;

module-rulesets
  : module-ruleset-list
  | %empty
  ;

module-ruleset-list
  : module-ruleset-list module-ruleset
  | module-ruleset
  ;

module-ruleset
  : string
  {
    configset_parser_rulesets_alloc(PARSER, $1);
  }
  ;

exec-value
  : primitive
  | exec-value-lookup
  ;

exec-value-lookup
  : LOOKUP ':' '{' PATTERN ':' LOOKUP_PATTERN NAMESPACE ':' strtoken '}'
  {
    exec_value_lookup_mk(PARSER->bacon_parser, &@$, &$$, $6, @9.s, @9.l);
  }
  | LOOKUP ':' '{' NAMESPACE ':' strtoken PATTERN ':' LOOKUP_PATTERN '}'
  {
    exec_value_lookup_mk(PARSER->bacon_parser, &@$, &$$, $9, @6.s, @6.l);
  }
  | LOOKUP ':' '{' PATTERN ':' LOOKUP_PATTERN '}'
  {
    exec_value_lookup_mk(PARSER->bacon_parser, &@$, &$$, $6, 0, 0);
  }
  ;

namespaces-section
  : NAMESPACES ':' '{' namespace-sections-list '}'
  ;

namespace-sections-list
  : namespace-sections
  | %empty
  ;

namespace-sections
  : namespace-sections namespace-section
  | namespace-section
  ;

namespace-section
  : unquoted_string { configset_parser_namespace_alloc(PARSER, $1); }
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

/*
specified as pattern so that the same character quoting rules are in effect
*/
special-entry
  : MATCH_PATTERN
  {
    configset_parser_wse_put(PARSER, $1);
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

primitive
  : boolean
  | float
  | negint
  | posint
  | variable
  | string
  ;

boolean
  : BOOL
  {
    bacon_boolean_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

float
  : FLOAT
  {
    bacon_float_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

posint
  : posint_ugroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  | posint_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

posint_ugroup
  : UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  | HEX8
  | HEX16
  | HEX32
  | HEX64
  ;

posint_igroup
  : INTMAX8
  | INTMAX16
  | INTMAX32
  | INTMAX64
  ;

negint
  : negint_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

negint_igroup
  : INTMIN8
  | INTMIN16
  | INTMIN32
  | INTMIN64
  ;

int64
  : uint32
  {
    $$ = $1;
  }
  | int64_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  ;

int64_igroup
  : INTMIN64 { $$ = $1; }
  | INTMAX64 { $$ = $1; }
  ;

uint32
  : uint32_igroup
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  | UINTMAX32
  {
    bacon_integer_mk(PARSER->bacon_parser, &@$, &$$, $1);
  }
  | uint16
  ;

uint32_igroup
  : INTMIN32 { $$ = $1; }
  | INTMAX32 { $$ = $1; }
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

variable
  : global-sysvar-token
  {
    bacon_variable_mkw(PARSER->bacon_parser, &@$, &$$, @1.s + 1, @1.l - 1);
  }
  ;

global-sysvar-token
  : BM_BASEDIR
  | BM_HOMEDIR
  | BM_IPCDIR
  | BM_BAMD_ID
  | BM_BAMD_PID
  ;

string
  : unquoted_string
  | quoted_string
  ;

/* must be a single token because strings can appear repeatedly together */
unquoted_string
  : strtoken
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  | wordtoken
  {
    bacon_string_mkw(PARSER->bacon_parser, &@$, 0, &$$, @1.s, @1.l);
  }
  ;

wordtoken
  : '/'
  | '-'
  | '+'
  | '='
  | ','
  | '@'
  | PATTERN
  | BUILD
  | MODULES
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
  : strtoken
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

strtoken
  : STR
  | PLUGINS
  ;
