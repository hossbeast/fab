/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <inttypes.h>
  #include "types.h"

  #include "xlinux/xstdlib.h"

  #include "request_parser.internal.h"
  #include "request.h"
  #include "selector.h"
  #include "fsent.h"

  #include "macros.h"
  #include "zcat.h"
}

%code top {
  int request_yylex(void *, void*, void*);
  #define PARSER containerof(parser, request_parser, yyu)
  #define REQUEST_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {request_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;

  struct selector *selector;
  struct request *request;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'
%token '='

%token            STR           "string"
%token <selector> SELECTOR
%token <yyu.umax> UINTMAX8
%token <yyu.umax> UINTMAX16
%token <yyu.umax> UINTMAX32
%token <yyu.umax> UINTMAX64
%token <yyu.umax> HEX8
%token <yyu.umax> HEX16
%token <yyu.umax> HEX32
%token <yyu.umax> HEX64
%token <yyu.imax> INTMIN8
%token <yyu.imax> INTMIN16
%token <yyu.imax> INTMIN32
%token <yyu.imax> INTMIN64
%token <yyu.imax> INTMAX8
%token <yyu.imax> INTMAX16
%token <yyu.imax> INTMAX32
%token <yyu.imax> INTMAX64

%type <yyu.umax> uint

%token
  AUTORUN             "autorun"
  BUILD               "build"
  CONSOLE             "console"
  CONFIG_DESCRIBE     "config-describe"
  VARS_DESCRIBE       "vars-describe"
  ENVS_DESCRIBE       "envs-describe"
  RMA_DESCRIBE        "rma-describe"
  RULES_DESCRIBE      "rules-describe"
  FSTREE_DESCRIBE     "fstree-describe"
  FILESYSTEM_DESCRIBE "filesystem-describe"
  DESCRIBE            "describe"
  INVALIDATE          "invalidate"
  GLOBAL_INVALIDATE   "global-invalidate"
  LIST                "list"
  RESET_SELECTION     "reset-selection"
  RUN                 "run"
  SCRIPT              "script"
  SELECT              "select"
  GLOBAL_STATS_READ   "global-stats-read"
  GLOBAL_STATS_RESET  "global-stats-reset"
  STATS_READ          "stats-read"
  STATS_RESET         "stats-reset"
  METADATA            "metadata"
  RECONCILE           "reconcile"
  GOALS               "goals"
  TARGET_DIRECT       "target-direct"
  TARGET_TRANSITIVE   "target-transitive"
  SPECIAL_ONLY        "special-only"
  NO_SPECIAL          "no-special"
  CONTEXT             "context"
  MODULE              "module"

%destructor { selector_free($$); } <selector>

%%
utterance
  : '[' request ']'
  | request
  ;

allocate-request
  : %empty
  ;

request
  : allocate-request commands
  ;

allocate-command
  : %empty
  {
    request_command_alloc(PARSER->request, &PARSER->command);
  }
  ;

commands
  : commands command
  | command
  ;

command
  : allocate-command command-branch
  ;

command-branch
  : describe-cmd
  | list-cmd
  | invalidate-cmd
  | global-invalidate-cmd
  | stats-cmd
  | global-stats-cmd
  | context-cmd
  | select-cmd
  | reset-selection-cmd
  | goals-cmd
  | metadata-cmd
  | config-describe-cmd
  | vars-describe-cmd
  | envs-describe-cmd
  | rma-describe-cmd
  | rules-describe-cmd
  | fstree-describe-cmd
  | filesystem-describe-cmd
  | run-cmd
  | autorun-cmd
  | reconcile-cmd
  ;

context-cmd
  : CONTEXT ':' '{' context-list '}'
  {
    PARSER->command->type = COMMAND_CONTEXT;
  }
  ;

context-list
  : context-list context-mapping
  | context-mapping
  ;

context-mapping
  : MODULE ':' uint
  {
    PARSER->command->context.module_id = $3;
  }
  ;

select-cmd
  : SELECT ':' SELECTOR
  {
    PARSER->command->type = COMMAND_SELECT;
    PARSER->command->selector = $3;
  }
  ;

run-cmd
  : RUN { PARSER->command->type = COMMAND_RUN; }
  ;

autorun-cmd
  : AUTORUN { PARSER->command->type = COMMAND_AUTORUN; }
  ;

reset-selection-cmd
  : RESET_SELECTION { PARSER->command->type = COMMAND_RESET_SELECTION; }
  ;

describe-cmd
  : DESCRIBE { PARSER->command->type = COMMAND_DESCRIBE; }
  ;

invalidate-cmd
  : INVALIDATE { PARSER->command->type = COMMAND_INVALIDATE ; }
  ;

global-invalidate-cmd
  : GLOBAL_INVALIDATE { PARSER->command->type = COMMAND_GLOBAL_INVALIDATE ; }
  ;

list-cmd
  : LIST { PARSER->command->type = COMMAND_LIST; }
  ;

config-describe-cmd
  : CONFIG_DESCRIBE { PARSER->command->type = COMMAND_CONFIG_DESCRIBE; }
  ;

vars-describe-cmd
  : VARS_DESCRIBE { PARSER->command->type = COMMAND_VARS_DESCRIBE; }
  ;

envs-describe-cmd
  : ENVS_DESCRIBE { PARSER->command->type = COMMAND_ENVS_DESCRIBE; }
  ;

rma-describe-cmd
  : RMA_DESCRIBE { PARSER->command->type = COMMAND_RMA_DESCRIBE; }
  ;

rules-describe-cmd
  : RULES_DESCRIBE { PARSER->command->type = COMMAND_RULES_DESCRIBE; }
  ;

fstree-describe-cmd
  : FSTREE_DESCRIBE { PARSER->command->type = COMMAND_FSTREE_DESCRIBE; }
  ;

filesystem-describe-cmd
  : FILESYSTEM_DESCRIBE { PARSER->command->type = COMMAND_FILESYSTEM_DESCRIBE; }
  ;

stats-cmd
  : stats-cmd-type
  | stats-cmd-type ':' '{' stats-args '}'
  ;

stats-cmd-type
  : STATS_READ { PARSER->command->type = COMMAND_STATS_READ; }
  | STATS_RESET { PARSER->command->type = COMMAND_STATS_RESET; }
  ;

stats-args
  : stats-args-list
  | %empty
  ;

stats-args-list
  : stats-args-list stats-arg
  | stats-arg
  ;

stats-arg
  : SPECIAL_ONLY
  {
    PARSER->command->stats.special_only = true;
  }
  | NO_SPECIAL
  {
    PARSER->command->stats.no_special = true;
  }
  ;

global-stats-cmd
  : GLOBAL_STATS_READ { PARSER->command->type = COMMAND_GLOBAL_STATS_READ; }
  | GLOBAL_STATS_RESET { PARSER->command->type = COMMAND_GLOBAL_STATS_RESET; }
  ;

metadata-cmd
  : METADATA { PARSER->command->type = COMMAND_METADATA; }
  ;

goals-cmd
  : GOALS ':' '{' goals-cmd-list '}'
  {
    PARSER->command->type = COMMAND_GOALS;
  }
  ;

goals-cmd-list
  : goals-cmd-list goals-subcmd
  | goals-subcmd
  ;

goals-subcmd
  : reconcile-cmd
  {
    PARSER->command->goals.reconcile = true;
  }
  | build-cmd
  | script-cmd
  | target-direct-cmd
  | target-transitive-cmd
  ;

build-cmd
  : BUILD
  {
    PARSER->command->goals.build = true;
  }
  ;

script-cmd
  : SCRIPT
  {
    PARSER->command->goals.script = true;
  }
  ;

target-direct-cmd
  : TARGET_DIRECT ':' SELECTOR
  {
    PARSER->command->goals.target_direct = $3;
  }
  ;

target-transitive-cmd
  : TARGET_TRANSITIVE ':' SELECTOR
  {
    PARSER->command->goals.target_transitive = $3;
  }
  ;

reconcile-cmd
  : RECONCILE { PARSER->command->type = COMMAND_RECONCILE; }
  ;

uint
  : UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  | INTMAX8   { $$ = $1; }
  | INTMAX16  { $$ = $1; }
  | INTMAX32  { $$ = $1; }
  | INTMAX64  { $$ = $1; }
  | HEX8
  | HEX16
  | HEX32
  | HEX64
  ;
