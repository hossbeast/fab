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
  #include "box.h"
  #include "macros.h"

  struct value;
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

  int       i;
  struct {
    char * s;
    uint16_t len;
  } str;

  // enums
  invalidate_type invalidate_type_e;
  stream_part stream_part_e;

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
%type <yyu.imax>  uint16_igroup
%type <yyu.umax>  uint16_ugroup

%token
 BUILD                    "build"
 CAPTURE_STDERR           "capture-stderr"
 CAPTURE_STDOUT           "capture-stdout"
 CAPTURE_AUXOUT           "capture-auxout"
 CONCURRENCY              "concurrency"
 CONSOLE                  "console"
 ERROR                    "error"
 EXTERN                   "extern"
 EXPRS                    "exprs"
 FILESYSTEMS              "filesystems"
 FORMULA                  "formula"
 INVALIDATE               "invalidate"
 LOGGING                  "logging"
 LOGFILE                  "logfile"
 SHOW_ARGUMENTS           "show-arguments"
 SHOW_COMMAND             "show-command"
 SHOW_CWD                 "show-cwd"
 SHOW_SOURCES             "show-sources"
 SHOW_PATH                "show-path"
 SHOW_TARGETS             "show-targets"
 SHOW_ENVIRONMENT         "show-environment"
 SHOW_STATUS              "show-status"
 SHOW_STDOUT              "show-stdout"
 SHOW_STDOUT_LIMIT_BYTES  "show-stdout-limit-bytes"
 SHOW_STDOUT_LIMIT_LINES  "show-stdout-limit-lines"
 SHOW_STDERR              "show-stderr"
 SHOW_STDERR_LIMIT_BYTES  "show-stderr-limit-bytes"
 SHOW_STDERR_LIMIT_LINES  "show-stderr-limit-lines"
 SHOW_AUXOUT              "show-auxout"
 SHOW_AUXOUT_LIMIT_BYTES  "show-auxout-limit-bytes"
 SHOW_AUXOUT_LIMIT_LINES  "show-auxout-limit-lines"
 STDOUT_BUFFER_SIZE       "stdout-buffer-size"
 STDERR_BUFFER_SIZE       "stderr-buffer-size"
 AUXOUT_BUFFER_SIZE       "auxout-buffer-size"
 SUCCESS                  "success"
 VAR                      "var"
 PATH                     "path"
 COPY_FROM_ENV            "copy-from-env"
 DIRS                     "dirs"
 STAT                     "stat"
 CONTENT                  "content"
 NOTIFY                   "notify"
 ALWAYS                   "always"
 NEVER                    "never"
 LEADING                  "leading"
 TRAILING                 "trailing"
 NONE                     "none"

/*
 EXECUTABLES              "executables"
 */

/* nonterminals */
%type <str> string strpart strparts quoted_string unquoted_string
%type <b_string> bstring
%type <b_bool> bool
%type <b_int16>  int16
%type <b_uint16> uint16
%type <b_int> invalidate-type
%type <i> invalidate-type-enum
%type <b_int> stream-part
%type <i> stream-part-enum

%destructor { wfree($$.s); } <str>
%destructor { box_free(refas($$, bx)); } <b_bool>
%destructor { box_free(refas($$, bx)); } <b_int>
%destructor { box_free(refas($$, bx)); } <b_int16>
%destructor { box_free(refas($$, bx)); } <b_uint16>
%destructor { box_free(refas($$, bx)); } <b_string>

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
  | extern-section
  | filesystems-section
  | formula-section
  | logging-section
  | var-section
  ;

var-section
  : VAR var-section-body
  {
    PARSER->cfg->var.merge_significant = true;
  }
  ;

var-section-body
  : ':' var-section-set
  | '=' var-section-set-epsilon
  {
    PARSER->cfg->var.merge_overwrite = true;
  }
  ;

var-section-set-epsilon
  : var-section-set
  | %empty
  ;

var-section-set
  : VALUE
  {
    PARSER->cfg->var.value = $1;
  }
  ;

build-section
  : BUILD build-map
  {
    PARSER->cfg->build.merge_significant = true;
  }
  ;

build-map
  : ':' '{' build-mapping-set '}'
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
  
formula-section
  : FORMULA formula-map
  ;

formula-map
  : ':' '{' formula-mappings '}'
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
  : CAPTURE_STDERR ':' stream-part
  {
    PARSER->cfg->formula.capture_stderr = $3;
  }
  | CAPTURE_STDOUT ':' stream-part
  {
    PARSER->cfg->formula.capture_stdout = $3;
  }
  | CAPTURE_AUXOUT ':' stream-part
  {
    PARSER->cfg->formula.capture_auxout = $3;
  }
  | STDOUT_BUFFER_SIZE ':' uint16
  {
    PARSER->cfg->formula.stdout_buffer_size = $3;
  }
  | STDERR_BUFFER_SIZE ':' uint16
  {
    PARSER->cfg->formula.stderr_buffer_size = $3;
  }
  | AUXOUT_BUFFER_SIZE ':' uint16
  {
    PARSER->cfg->formula.auxout_buffer_size = $3;
  }
  | formula-success
  | formula-error
  | formula-path
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
  ;

formula-path-dir
  : bstring
  {
    if($1) {
      YFATAL(set_put, PARSER->cfg->formula.path.dirs.entries, $1, 0);
    }
  }
  ;

formula-success
  : SUCCESS ':' '{' formula-show '}'
  {
    PARSER->cfg->formula.success = PARSER->show_settings;
  }
  | SUCCESS '=' '{' formula-show-epsilon '}'
  {
    PARSER->cfg->formula.success = PARSER->show_settings;
    PARSER->cfg->formula.success.merge_overwrite = true;
  }
  ;

formula-error
  : ERROR ':' '{' formula-show '}'
  {
    PARSER->cfg->formula.error = PARSER->show_settings;
  }
  | ERROR '=' '{' formula-show-epsilon '}'
  {
    PARSER->cfg->formula.error = PARSER->show_settings;
    PARSER->cfg->formula.error.merge_overwrite = true;
  }
  ;

allocate-formula-show
  : %empty
  {
    memset(&PARSER->show_settings, 0, sizeof(PARSER->show_settings));
  }
  ;

formula-show-epsilon
  : formula-show
  | %empty
  {
    PARSER->show_settings.merge_significant = true;
  }
  ;

formula-show
  : formula-show formula-show-mapping
  | allocate-formula-show formula-show-mapping
  {
    PARSER->show_settings.merge_significant = true;
  }
  ;

formula-show-mapping
  : SHOW_STDOUT ':' bool
  {
    PARSER->show_settings.show_stdout = $3;
  }
  | SHOW_STDOUT_LIMIT_LINES ':' int16
  {
    PARSER->show_settings.show_stdout_limit_lines = $3;
  }
  | SHOW_STDOUT_LIMIT_BYTES ':' int16
  {
    PARSER->show_settings.show_stdout_limit_bytes = $3;
  }
  | SHOW_STDERR ':' bool
  {
    PARSER->show_settings.show_stderr = $3;
  }
  | SHOW_STDERR_LIMIT_LINES ':' int16
  {
    PARSER->show_settings.show_stderr_limit_lines = $3;
  }
  | SHOW_STDERR_LIMIT_BYTES ':' int16
  {
    PARSER->show_settings.show_stderr_limit_bytes = $3;
  }
  | SHOW_AUXOUT ':' bool
  {
    PARSER->show_settings.show_auxout = $3;
  }
  | SHOW_AUXOUT_LIMIT_LINES ':' int16
  {
    PARSER->show_settings.show_auxout_limit_lines = $3;
  }
  | SHOW_AUXOUT_LIMIT_BYTES ':' int16
  {
    PARSER->show_settings.show_auxout_limit_bytes = $3;
  }
  | SHOW_ARGUMENTS ':' bool
  {
    PARSER->show_settings.show_arguments = $3;
  }
  | SHOW_CWD ':' bool
  {
    PARSER->show_settings.show_cwd = $3;
  }
  | SHOW_COMMAND ':' bool
  {
    PARSER->show_settings.show_command = $3;
  }
  | SHOW_SOURCES ':' bool
  {
    PARSER->show_settings.show_sources = $3;
  }
  | SHOW_PATH ':' bool
  {
    PARSER->show_settings.show_path = $3;
  }
  | SHOW_TARGETS ':' bool
  {
    PARSER->show_settings.show_targets = $3;
  }
  | SHOW_ENVIRONMENT ':' bool
  {
    PARSER->show_settings.show_environment = $3;
  }
  | SHOW_STATUS ':' bool
  {
    PARSER->show_settings.show_status = $3;
  }
  ;

logging-section
  : LOGGING logging-map
  ;

logging-map
  : ':' '{' logging-mappings '}'
  | '=' '{' logging-mappings-epsilon '}'
  {
    PARSER->cfg->logging.merge_overwrite = true;
  }
  ;

logging-mappings-epsilon
  : logging-mappings
  | %empty
  {
    PARSER->cfg->logging.merge_significant = true;
  }
  ;

logging-mappings
  : logging-mappings logging-mapping
  | logging-mapping
  {
    PARSER->cfg->logging.merge_significant = true;
  }
  ;

allocate-console-logging-settings
  : %empty
  {
    PARSER->logging_section = &PARSER->cfg->logging.console;
  }
  ;

console-logging-settings
  : allocate-console-logging-settings CONSOLE
  {
    PARSER->cfg->logging.console.merge_significant = true;
  }
  ;

allocate-logfile-logging-settings
  : %empty
  {
    PARSER->logging_section = &PARSER->cfg->logging.logfile;
  }
  ;

logfile-logging-settings
  : allocate-logfile-logging-settings LOGFILE
  {
    PARSER->cfg->logging.logfile.merge_significant = true;
  }
  ;

logging-mapping
  : console-logging-settings ':' '{' logging-settings '}'
  | console-logging-settings '=' '{' logging-settings-epsilon '}'
  {
    PARSER->cfg->logging.console.merge_overwrite = true;
  }
  | logfile-logging-settings ':' '{' logging-settings '}'
  | logfile-logging-settings '=' '{' logging-settings-epsilon '}'
  {
    PARSER->cfg->logging.logfile.merge_overwrite = true;
  }
  ;

logging-settings-epsilon
  : logging-settings
  {
    PARSER->logging_section->merge_significant = true;
  }
  | %empty
  {
    PARSER->logging_section->merge_significant = true;
  }
  ;

logging-settings
  : EXPRS ':' '[' logging-exprs ']'
  {
    PARSER->logging_section->merge_significant = true;
  }
  | EXPRS '=' '[' logging-exprs-epsilon ']'
  {
    PARSER->logging_section->exprs.merge_overwrite = true;
  }
  ;

logging-exprs-epsilon
  : logging-exprs
  | %empty
  {
    PARSER->logging_section->exprs.merge_significant = true;
  }
  ;

logging-exprs
  : logging-exprs logging-expr
  | logging-expr
  {
    PARSER->logging_section->exprs.merge_significant = true;
  }
  ;

logging-expr
  : bstring
  {
    if($1) {
      YFATAL(list_push, PARSER->logging_section->exprs.items, $1, 0);
    }
  }
  ;

filesystems-section
  : FILESYSTEMS filesystems-map
  ;

filesystems-map
  : ':' '{' filesystems-mappings '}'
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
  : ':' '{' filesystems-entry-mappings '}'
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

extern-section
  : EXTERN ':' '{' extern-entry-list '}'
  | EXTERN '=' '{' extern-entry-list-epsilon '}'
  {
    PARSER->cfg->extern_section.merge_overwrite = true;
  }
  ;

extern-entry-list-epsilon
  : extern-entry-list
  | %empty
  ;

extern-entry-list
  : extern-entry-list extern-entry
  | extern-entry
  {
    PARSER->cfg->extern_section.merge_significant = true;
  }
  ;

extern-entry
  : bstring
  {
    if($1) {
      YFATAL(set_put, PARSER->cfg->extern_section.entries, $1, 0);
    }
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

stream-part
  : stream-part-enum
  {
    YFATAL(box_int_mk, &@$, &$$, $1);
  }
  ;

stream-part-enum
  : LEADING
  {
    $$ = STREAM_PART_LEADING;
  }
  | TRAILING
  {
    $$ = STREAM_PART_TRAILING;
  }
  | NONE
  {
    $$ = STREAM_PART_NONE;
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

uint16
  : uint16_igroup
  {
    YFATAL(box_uint16_mk, &@$, &$$, $1);
  }
  | uint16_ugroup
  {
    YFATAL(box_uint16_mk, &@$, &$$, $1);
  }
  ;

uint16_igroup
  : INTMAX8
  | INTMAX16
  ;

uint16_ugroup
  : UINTMAX8
  | UINTMAX16
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
