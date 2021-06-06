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

  #include "xlinux/xstring.h"
  #include "xlinux/xstdlib.h"
  #include "valyria/map.h"
  #include "valyria/set.h"
  #include "valyria/list.h"

  #include "config_parser.internal.h"
  #include "config.internal.h"
  #include "build_thread.h"
  #include "yyutil/box.h"
  #include "macros.h"
  #include "pattern.h"

  struct value;
  struct pattern;
}

%code top {
  int config_yylex(void*, void*, void*);
  #define PARSER ((config_parser*)parser)
  #define CONFIG_YYLTYPE yyu_location
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

  struct value * value;
  struct pattern * pattern;

  int       i;
  struct {
    char * s;
    uint16_t len;
  } str;

  // enums
  invalidate_type invalidate_type_e;

  struct box_bool * b_bool;
  struct box_int * b_int;
  struct box_int8 * b_int8;
  struct box_int16 * b_int16;
  struct box_uint8 * b_uint8;
  struct box_uint16 * b_uint16;
  struct box_string * b_string;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'
%token '='

%token <value> VALUE

/* terminals */
%token             STR        "string"
%token <yyu.u8>    CREF       "character-escape"
%token <yyu.u8>    HREF       "hex-escape"
%token <yyu.b>     BOOL       "boolean"
%token <yyu.f>     FLOAT      "float"
%token <yyu.umax>  UINTMAX8
%token <yyu.umax>  UINTMAX16
%token <yyu.umax>  UINTMAX32
%token <yyu.umax>  UINTMAX64
%token <yyu.imax>  INTMIN8
%token <yyu.imax>  INTMIN16
%token <yyu.imax>  INTMIN32
%token <yyu.imax>  INTMIN64
%token <yyu.imax>  INTMAX8
%token <yyu.imax>  INTMAX16
%token <yyu.imax>  INTMAX32
%token <yyu.imax>  INTMAX64

%type <yyu.imax>  int16_igroup
%type <yyu.umax>  int16_ugroup

%token
 BUILD                    "build"
 WORKERS                  "workers"
 CONCURRENCY              "concurrency"
 CONSOLE                  "console"
 ERROR                    "error"
 EXPRS                    "exprs"
 FILESYSTEMS              "filesystems"
 FORMULA                  "formula"
 INVALIDATE               "invalidate"
 LOGFILE                  "logfile"
 PATH                     "path"
 COPY_FROM_ENV            "copy-from-env"
 DIRS                     "dirs"
 STAT                     "stat"
 CONTENT                  "content"
 NOTIFY                   "notify"
 ALWAYS                   "always"
 NEVER                    "never"
 LEADING                  "leading"
 WALKER                   "walker"
 EXCLUDE                  "exclude"
 INCLUDE                  "include"

%token
 <pattern> INCLUDE_PATTERN  "include-pattern"
 <pattern> MATCH_PATTERN    "match-pattern"

%type <pattern> match-pattern
%type <pattern> include-pattern

/* nonterminals */
%type <str> string strpart strparts quoted_string unquoted_string
%type <b_string> bstring
%type <b_bool> bool
%type <b_int16>  int16
%type <b_int> invalidate-type
%type <i> invalidate-type-enum

%destructor { wfree($$.s); } <str>
%destructor { box_free(refas($$, bx)); } <b_bool>
%destructor { box_free(refas($$, bx)); } <b_int>
%destructor { box_free(refas($$, bx)); } <b_int16>
%destructor { box_free(refas($$, bx)); } <b_string>
%destructor { pattern_free($$); } <pattern>

%%
utterance
  : allocate-config config
  {
    if(PARSER->yyu.attrs & YYU_PARTIAL)
    {
      yyu_loc_final(&PARSER->yyu, &@2);
      YYACCEPT;
    }
  }
  ;

allocate-config
  : %empty
  {
    YFATAL(config_create, &PARSER->cfg);
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
  | workers-section
  | filesystems-section
  | formula-section
  | walker-section
  ;

build-section
  : BUILD build-map
  {
    PARSER->cfg->build.merge_significant = true;
  }
  ;

workers-section
  : WORKERS workers-map
  {
    PARSER->cfg->workers.merge_significant = true;
  }
  ;

build-map
  : ':' '{' build-mapping-set-epsilon '}'
  | '=' '{' build-mapping-set-epsilon '}'
  {
    PARSER->cfg->build.merge_overwrite = true;
  }
  ;

build-mapping-set-epsilon
  : build-mapping-set
  | %empty
  ;

build-mapping-set
  : build-mappings
  ;

build-mappings
  : build-mappings build-mapping
  | build-mapping
  ;

build-mapping
  : CONCURRENCY ':' int16
  {
    PARSER->cfg->build.concurrency = $3;
  }
  ;
 
workers-map
  : ':' '{' workers-mapping-set-epsilon '}'
  | '=' '{' workers-mapping-set-epsilon '}'
  {
    PARSER->cfg->workers.merge_overwrite = true;
  }
  ;

workers-mapping-set-epsilon
  : workers-mapping-set
  | %empty
  ;

workers-mapping-set
  : workers-mappings
  ;

workers-mappings
  : workers-mappings workers-mapping
  | workers-mapping
  ;

workers-mapping
  : CONCURRENCY ':' int16
  {
    PARSER->cfg->workers.concurrency = $3;
  }
  ;
  
formula-section
  : FORMULA formula-map
  ;

formula-map
  : ':' '{' formula-mappings-epsilon '}'
  | '=' '{' formula-mappings-epsilon '}'
  {
    PARSER->cfg->formula.merge_overwrite = true;
  }
  ;

formula-mappings-epsilon
  : formula-mappings
  | %empty
  {
    PARSER->cfg->formula.merge_significant = true;
  }
  ;

formula-mappings
  : formula-mappings formula-mapping
  | formula-mapping
  {
    PARSER->cfg->formula.merge_significant = true;
  }
  ;

formula-mapping
  : formula-path
  ;

formula-path
  : PATH ':' '{' formula-path-mappings '}'
  | PATH '=' '{' formula-path-mappings '}'
  {
    PARSER->cfg->formula.path.merge_overwrite = true;
  }
  ;

formula-path-mappings
  : formula-path-mappings formula-path-mapping
  | formula-path-mapping
  {
    PARSER->cfg->formula.path.merge_significant = true;
  }
  ;

formula-path-mapping
  : COPY_FROM_ENV ':' bool
  {
    PARSER->cfg->formula.path.copy_from_env = $3;
  }
  | DIRS ':' '{' formula-path-dirs '}'
  | DIRS '=' '{' formula-path-dirs '}'
  {
    PARSER->cfg->formula.path.dirs.merge_overwrite = true;
  }
  ;

formula-path-dirs
  : formula-path-dirs formula-path-dir
  | formula-path-dir
  {
    PARSER->cfg->formula.path.dirs.merge_significant = true;
  }
  ;

formula-path-dir
  : bstring
  {
    if($1) {
      YFATAL(set_put, PARSER->cfg->formula.path.dirs.entries, $1, 0);
    }
  }
  ;

filesystems-section
  : FILESYSTEMS filesystems-map
  ;

filesystems-map
  : ':' '{' filesystems-mappings-epsilon '}'
  | '=' '{' filesystems-mappings-epsilon '}'
  {
    PARSER->cfg->filesystems.merge_overwrite = true;
  }
  ;

filesystems-mappings-epsilon
  : filesystems-mappings 
  | %empty
  ;

filesystems-mappings
  : filesystems-mappings filesystems-mapping
  | filesystems-mapping
  {
    PARSER->cfg->filesystems.merge_significant = true;
  }
  ;

filesystems-mapping
  : string filesystems-entry-map
  {
    YFATAL(map_put, PARSER->cfg->filesystems.entries, $1.s, $1.len, PARSER->fse, 0);
    PARSER->fse = 0;
    wfree($1.s);
    $1.s = 0;
  }
  ;

filesystems-entry-map
  : ':' '{' filesystems-entry-mappings-epsilon '}'
  | '=' '{' filesystems-entry-mappings-epsilon '}'
  {
    PARSER->fse->merge_overwrite = true;
  }
  ;

filesystems-entry-mappings-epsilon
  : filesystems-entry-mappings
  ;

allocate-filesystems-entry
  : %empty
  {
    YFATAL(xmalloc, &PARSER->fse, sizeof(*PARSER->fse));
  }
  ;

filesystems-entry-mappings
  : filesystems-entry-mappings filesystems-entry-mapping
  | allocate-filesystems-entry filesystems-entry-mapping
  ;

filesystems-entry-mapping
  : INVALIDATE ':' invalidate-type
  {
    PARSER->fse->invalidate = $3;
  }
  ;

walker-section
  : WALKER walker-section-settings
  ;

walker-section-settings
  : ':' '{' walker-settings-epsilon '}'
  | '=' '{' walker-settings-epsilon '}'
  {
    PARSER->cfg->walker.merge_overwrite = true;
  }
  ;

walker-settings-epsilon
  : walker-settings
  {
    PARSER->cfg->walker.merge_significant = 0
      || PARSER->cfg->walker.include.merge_significant
      || PARSER->cfg->walker.exclude.merge_significant
      ;
  }
  | %empty
  ;

walker-settings
  : walker-settings walker-setting
  | walker-setting
  ;

walker-setting
  : INCLUDE walker-include-entries
  | EXCLUDE walker-exclude-entries
  ;

walker-exclude-entries
  : ':' '{' walker-exclude-entry-list-epsilon '}'
  | '=' '{' walker-exclude-entry-list-epsilon '}'
  {
    PARSER->cfg->walker.exclude.merge_overwrite = true;
  }
  ;

walker-exclude-entry-list-epsilon
  : walker-exclude-entry-list
  | %empty
  ;

walker-exclude-entry-list
  : walker-exclude-entry-list walker-exclude-entry
  | walker-exclude-entry
  {
    PARSER->cfg->walker.exclude.merge_significant = true;
  }
  ;

walker-exclude-entry
  : match-pattern
  {
    llist_append(&PARSER->cfg->walker.exclude.list, $1, lln);
  }
  ;

walker-include-entries
  : ':' '{' walker-include-entry-list-epsilon '}'
  | '=' '{' walker-include-entry-list-epsilon '}'
  {
    PARSER->cfg->walker.include.merge_overwrite = true;
  }
  ;

walker-include-entry-list-epsilon
  : walker-include-entry-list
  | %empty
  ;

walker-include-entry-list
  : walker-include-entry-list walker-include-entry
  | walker-include-entry
  {
    PARSER->cfg->walker.include.merge_significant = true;
  }
  ;

walker-include-entry
  : include-pattern
  {
    llist_append(&PARSER->cfg->walker.include.list, $1, lln);
  }
  ;

invalidate-type
  : invalidate-type-enum
  {
    YFATAL(box_int_mk, &@$, &$$, $1);
  }
  ;

invalidate-type-enum
  : STAT
  {
    $$ = INVALIDATE_STAT;
  }
  | CONTENT
  {
    $$ = INVALIDATE_CONTENT;
  }
  | NOTIFY
  {
    $$ = INVALIDATE_NOTIFY;
  }
  | ALWAYS
  {
    $$ = INVALIDATE_ALWAYS;
  }
  | NEVER
  {
    $$ = INVALIDATE_NEVER;
  }
  ;

bool
  : BOOL
  {
    YFATAL(box_bool_mk, &@$, &$$, $1);
  }
  ;

int16
  : int16_igroup
  {
    YFATAL(box_int16_mk, &@$, &$$, $1);
  }
  | int16_ugroup
  {
    YFATAL(box_int16_mk, &@$, &$$, $1);
  }
  ;

int16_igroup
  : INTMIN16
  | INTMIN8
  | INTMAX8
  | INTMAX16
  ;

int16_ugroup
  : UINTMAX8
  ;

bstring
  : string
  {
    if($1.len == 0) {
      $$ = 0;
    } else {
      YFATAL(box_string_mk, &@$, &$$, $1.s, $1.len);
    }
    $1.s = 0;
  }
  ;

include-pattern
  : INCLUDE_PATTERN
  ;

match-pattern
  : MATCH_PATTERN
  ;

string
  : unquoted_string
  | quoted_string
  ;

unquoted_string
  : strpart
  ;

quoted_string
  : '"' strparts '"'
  {
    $$ = $2;
  }
  | '"' '"'
  {
    $$.s = 0;
    $$.len = 0;
  }
  ;

strparts
  : strparts strpart
  {
    $$ = $1;
    YFATAL(ixstrcat, &$$.s, $2.s);
    $$.len += $2.len;
  }
  | strpart
  ;

strpart
  : STR
  {
    $$ = (typeof($$)){};
    YFATAL(ixstrndup, &$$.s, @1.s, @1.l);
    $$.len = @1.l;
  }
  | CREF
  {
    $$ = (typeof($$)){};
    YFATAL(ixstrndup, &$$.s, (char*)&$1, 1);
    $$.len = 1;
  }
  | HREF
  {
    $$ = (typeof($$)){};
    YFATAL(ixstrndup, &$$.s, (char*)&$1, 1);
    $$.len = 1;
  }
  ;
