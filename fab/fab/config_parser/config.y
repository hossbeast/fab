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
  #include "yyutil/box.h"

  #include "config_parser.internal.h"
  #include "config.internal.h"
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

  struct box_bool * b_bool;
  struct box_int * b_int;
  struct box_int8 * b_int8;
  struct box_int16 * b_int16;
  struct box_uint8 * b_uint8;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'
%token '='

%token <value> VALUE

/* terminals */
%token <yyu.b>     BOOL       "boolean"
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
 ERROR                    "error"
 FORMULA                  "formula"
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
 SUCCESS                  "success"

/* nonterminals */
%type <b_bool> bool
%type <b_int16>  int16

%destructor { box_free(refas($$, bx)); } <b_bool>
%destructor { box_free(refas($$, bx)); } <b_int16>

%%
utterance
  : allocate-config config
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
  : formula-section
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
  : formula-success
  | formula-error
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
