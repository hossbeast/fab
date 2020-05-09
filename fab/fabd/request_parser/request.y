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
  #include "node.h"

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

  node_property node_property;
  struct selector * selector;
  struct config * config;
  struct request * request;
}

/* tokens */
%token '{' '}'
%token '[' ']'
%token ':'
%token '='

%token <config> CONFIG
%token <selector> SELECTOR

%token
  AUTORUN            "autorun"
  BUILD              "build"
  CONSOLE            "console"
  DESCRIBE           "describe"
  INVALIDATE         "invalidate"
  LIST               "list"
  RECONFIGURE        "reconfigure"
  RESET_SELECTION    "reset-selection"
  RUN                "run"
  SCRIPT             "script"
  SELECT             "select"
  STAGE_CONFIG       "stage-config"
  GOALS              "goals"
  TARGET_DIRECT      "target-direct"
  TARGET_TRANSITIVE  "target-transitive"

/* node property names */
  NAME          "name"
  EXT           "ext"
  SUFFIX        "suffix"
  BASE          "base"
  ABSPATH       "abspath"
  ABSDIR        "absdir"
  RELDIR        "reldir"
  RELPATH       "relpath"
  FSROOT        "fsroot"
  VARIANT       "variant"

/* nonterminals */
%type <node_property> node-property

%destructor { selector_free($$); } <selector>

%%
utterance
  : '[' request ']'
  | request
  ;

allocate-request
  : %empty
  {
    YFATAL(request_create, &PARSER->request);
  }
  ;

request
  : allocate-request commands
  ;

commands
  : initial-commands final-command
  | initial-commands
  | final-command
  ;

initial-commands
  : initial-commands initial-command
  | initial-command
  ;

allocate-command
  : %empty
  {
    YFATAL(array_push, PARSER->request->commands, &PARSER->command);
  }
  ;

initial-command
  : allocate-command initial-command-branch
  ;

final-command
  : allocate-command final-command-branch
  ;

initial-command-branch
  : describe-cmd
  | invalidate-cmd
  | list-cmd
  | stage-config-cmd
  | reconfigure-cmd
  | select-cmd
  | reset-selection-cmd
  | goals-cmd
  ;

 /* only permitted as the last command */
final-command-branch
  : run-cmd
  | autorun-cmd
  ;

select-cmd
  : SELECT ':' SELECTOR
  {
    PARSER->command->type = COMMAND_SELECT;
    PARSER->command->selector = $3;
  }
  ;

reset-selection-cmd
  : RESET_SELECTION { PARSER->command->type = COMMAND_RESET_SELECTION; }
  ;

stage-config-cmd
  : STAGE_CONFIG ':' CONFIG
  {
    PARSER->command->type = COMMAND_STAGE_CONFIG;
    PARSER->command->config = $3;
  }
  ;

run-cmd
  : RUN
  {
    PARSER->command->type = COMMAND_RUN;
    PARSER->request->final_command = COMMAND_RUN;
  }
  ;

autorun-cmd
  : AUTORUN
  {
    PARSER->command->type = COMMAND_AUTORUN;
    PARSER->request->final_command = COMMAND_AUTORUN;
  }
  ;

describe-cmd
  : DESCRIBE { PARSER->command->type = COMMAND_DESCRIBE; }
  ;

invalidate-cmd
  : INVALIDATE { PARSER->command->type = COMMAND_INVALIDATE ; }
  ;

list-cmd
  : LIST
  {
    PARSER->command->type = COMMAND_LIST;
    PARSER->command->property = NODE_PROPERTY_RELPATH;
  }
  | LIST ':' node-property
  {
    PARSER->command->type = COMMAND_LIST;
    PARSER->command->property = $3;
  }
  ;

node-property
  : NAME         { $$ = NODE_PROPERTY_NAME; }
  | EXT          { $$ = NODE_PROPERTY_EXT; }
  | SUFFIX       { $$ = NODE_PROPERTY_SUFFIX; }
  | BASE         { $$ = NODE_PROPERTY_BASE; }
  | ABSPATH      { $$ = NODE_PROPERTY_ABSPATH; }
  | ABSDIR       { $$ = NODE_PROPERTY_ABSDIR; }
  | RELPATH      { $$ = NODE_PROPERTY_RELPATH; }
  | RELDIR       { $$ = NODE_PROPERTY_RELDIR; }
  | FSROOT       { $$ = NODE_PROPERTY_FSROOT; }
  | VARIANT      { $$ = NODE_PROPERTY_VARIANT; }
  ;

reconfigure-cmd
  : RECONFIGURE { PARSER->command->type = COMMAND_RECONFIGURE; }
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
  : build-cmd
  | script-cmd
  | target-direct-cmd
  | target-transitive-cmd
  ;

build-cmd
  : BUILD { PARSER->command->goals.build = true; }
  ;

script-cmd
  : SCRIPT { PARSER->command->goals.script = true; }
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
