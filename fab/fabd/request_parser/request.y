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
  #include "types.h"

  #include "xlinux/xstdlib.h"
  #include "valyria/array.h"

  #include "request_parser.internal.h"
  #include "request.internal.h"
  #include "selector.h"
  #include "fsent.h"

  #include "macros.h"
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

  struct selector * selector;
  struct request * request;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'
%token '='

%token <selector> SELECTOR

%token
  AUTORUN            "autorun"
  BUILD              "build"
  CONSOLE            "console"
  CONFIG_READ        "config-read"
  VARS_READ          "vars-read"
  DESCRIBE           "describe"
  INVALIDATE         "invalidate"
  GLOBAL_INVALIDATE  "global-invalidate"
  LIST               "list"
  RESET_SELECTION    "reset-selection"
  RUN                "run"
  SCRIPT             "script"
  SELECT             "select"
  GLOBAL_STATS_READ  "global-stats-read"
  GLOBAL_STATS_RESET "global-stats-reset"
  STATS_READ         "stats-read"
  STATS_RESET        "stats-reset"
  METADATA           "metadata"
  RECONCILE          "reconcile"
  GOALS              "goals"
  TARGET_DIRECT      "target-direct"
  TARGET_TRANSITIVE  "target-transitive"

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
  | select-cmd
  | reset-selection-cmd
  | goals-cmd
  | metadata-cmd
  | config-read-cmd
  | vars-read-cmd
  | run-cmd
  | autorun-cmd
  | reconcile-cmd
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

config-read-cmd
  : CONFIG_READ { PARSER->command->type = COMMAND_CONFIG_READ; }
  ;

vars-read-cmd
  : VARS_READ { PARSER->command->type = COMMAND_VARS_READ; }
  ;

stats-cmd
  : STATS_READ { PARSER->command->type = COMMAND_STATS_READ; }
  | STATS_RESET { PARSER->command->type = COMMAND_STATS_RESET; }
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
