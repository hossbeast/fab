/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <inttypes.h>
  #include "types.h"

  #include "xlinux/xstdlib.h"
  #include "bam/events.h"

  #include "request_parser.internal.h"
  #include "request.h"
  #include "command.h"
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
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;

  struct command *command;
  struct selector *selector;
  struct request *request;
  bamipc_event_type eventtype;
}

%token            STR        "string"
%token <yyu.u8>   CREF       "character-escape"
%token <yyu.u8>   HREF       "hex-escape"
%token <yyu.b>    BOOL       "boolean"
%token <yyu.umax> UINTMAX8
%token <yyu.umax> UINTMAX16
%token <yyu.umax> UINTMAX32
%token <yyu.umax> UINTMAX64
%token <yyu.imax> INTMIN8
%token <yyu.imax> INTMIN16
%token <yyu.imax> INTMIN32
%token <yyu.imax> INTMIN64
%token <yyu.imax> INTMAX8
%token <yyu.imax> INTMAX16
%token <yyu.imax> INTMAX32
%token <yyu.imax> INTMAX64
%token <yyu.umax> HEX8
%token <yyu.umax> HEX16
%token <yyu.umax> HEX32
%token <yyu.umax> HEX64

%token <selector> SELECTOR

%type <yyu.umax> uint hex64
%type <eventtype> eventname
%type <yyu.t> string quoted-string unquoted-string

%destructor { selector_free($$); } <selector>

%token
  AUTORUN                 "autorun"
  BUILD                   "build"
  CONSOLE                 "console"
  DESCRIBE                "describe"
  IDENTIFY                "identify"
  LIST                    "list"
  TELL                    "tell"
  INVALIDATE              "invalidate"
  INVALIDATE_PRIMARY      "invalidate-primary"
  TREE_LIST               "tree-list"
  RESET_SELECTION         "reset-selection"
  RUN                     "run"
  SELECT                  "select"
  GLOBAL_STATS_READ       "global-stats-read"
  GLOBAL_STATS_RESET      "global-stats-reset"
  STATS_READ              "stats-read"
  STATS_RESET             "stats-reset"
  RECONCILE               "reconcile"
  GOALS                   "goals"
  TARGET_DIRECT           "target-direct"
  TARGET_TRANSITIVE       "target-transitive"
  SPECIAL_ONLY            "special-only"
  NO_SPECIAL              "no-special"
  NO_STATS                "no-stats"
  CONTEXT                 "context"
  MODULE                  "module"
  TOKEN                   "token"
  ID                      "id"
  PID                     "pid"
  LABEL                   "label"
  PLUGIN_READY            "plugin-ready"
  EVENTSUB                "eventsub"
  NODE_STALE              "node-stale"
  NODE_FRESH              "node-fresh"
  NODE_CREATE             "node-create"
  NODE_DELETE             "node-delete"
  GLOBAL_INVALIDATION     "global-invalidation"
  GOALS_SET               "goals-set"
  AUTORUN_SET             "autorun-set"
  SYSTEM_STATE_CHANGE     "system-state-change"
  FORMULA_FORK            "formula-fork"
  FORMULA_WAIT            "formula-wait"
  BUILD_START             "build-start"
  BUILD_END               "build-end"

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

commands
  : commands command
  | command
  ;

allocate-command
  : %empty
  {
    PARSER->command = command_alloc();
  }
  ;

command
  : allocate-command command-branch
  {
    llist_push(&PARSER->commands, &PARSER->command->lln);
    PARSER->command = 0;
  }
  ;

command-branch
  : describe-cmd
  | identify-cmd
  | list-cmd
  | tell-cmd
  | tree-list-cmd
  | invalidate-cmd
  | invalidate-primary-cmd
  | stats-cmd
  | global-stats-cmd
  | context-cmd
  | select-cmd
  | reset-selection-cmd
  | goals-cmd
  | run-cmd
  | autorun-cmd
  | reconcile-cmd
  | eventsub-cmd
  | plugin-ready-cmd
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
  : MODULE ':' string
  {
    PARSER->command->context.module_id_len = zcatw(
        PARSER->command->context.module_id
      , sizeof(PARSER->command->context.module_id)
      , $3.s
      , $3.l
    );
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
  : AUTORUN ':' BOOL
  {
    PARSER->command->type = COMMAND_AUTORUN;
    PARSER->command->autorun.enabled = $3;
  }
  ;

reset-selection-cmd
  : RESET_SELECTION { PARSER->command->type = COMMAND_RESET_SELECTION; }
  ;

describe-cmd
  : DESCRIBE { PARSER->command->type = COMMAND_DESCRIBE; } describe-args-set-epsilon
  ;

describe-args-set-epsilon
  : describe-args-set
  | %empty
  ;

describe-args-set
  : ':' '{' describe-args '}'
  ;

describe-args
  : describe-arg-list
  | %empty
  ;

describe-arg-list
  : describe-arg-list describe-arg
  | describe-arg
  ;

describe-arg
  : SPECIAL_ONLY
  {
    PARSER->command->describe.special_only = true;
  }
  | NO_SPECIAL
  {
    PARSER->command->describe.no_special = true;
  }
  | NO_STATS
  {
    PARSER->command->describe.no_stats = true;
  }
  ;

identify-cmd
  : IDENTIFY { PARSER->command->type = COMMAND_IDENTIFY; }
  ;

list-cmd
  : LIST { PARSER->command->type = COMMAND_LIST; }
  ;

tell-cmd
  : TELL { PARSER->command->type = COMMAND_TELL; }
  ;

invalidate-cmd
  : INVALIDATE { PARSER->command->type = COMMAND_INVALIDATE; }
  ;

invalidate-primary-cmd
  : INVALIDATE_PRIMARY { PARSER->command->type = COMMAND_INVALIDATE_PRIMARY; }
  ;

tree-list-cmd
  : TREE_LIST { PARSER->command->type = COMMAND_TREE_LIST; }
  ;

stats-cmd
  : stats-cmd-type stats-args-set-epsilon
  ;

stats-cmd-type
  : STATS_READ { PARSER->command->type = COMMAND_STATS_READ; }
  | STATS_RESET { PARSER->command->type = COMMAND_STATS_RESET; }
  ;

stats-args-set-epsilon
  : stats-args-set
  | %empty
  ;

stats-args-set
  : ':' '{' stats-args '}'
  ;

stats-args
  : stats-arg-list
  | %empty
  ;

stats-arg-list
  : stats-arg-list stats-arg
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
  : RECONCILE             { PARSER->command->goals.reconcile = true; }
  | BUILD                 { PARSER->command->goals.build = true; }
  | PID ':' uint
  | ID ':' uint
  | goals-token-cmd
  | goals-label-cmd
  | target-direct-cmd
  | target-transitive-cmd
  ;

goals-label-cmd
  : LABEL ':' STR
  {
    memcpy(PARSER->command->goals.label, @3.s, @3.l);
    PARSER->command->goals.label_len = @3.l;
  }
  ;

goals-token-cmd
  : TOKEN ':' hex64
  {
    PARSER->command->goals.token = $3;
  }
  ;

plugin-ready-cmd
  : PLUGIN_READY
  {
    PARSER->command->type = COMMAND_PLUGIN_READY;
  }
  ;

eventsub-cmd
  : EVENTSUB ':' '[' eventsubs ']'
  {
    PARSER->command->type = COMMAND_EVENTSUB;
  }
  ;

eventsubs
  : eventsub-list
  | %empty
  ;

eventsub-list
  : eventsub-list eventsub
  | eventsub
  ;

eventsub
  : eventname
  {
    PARSER->command->mask |= $1;
  }
  ;

eventname
  /* events which have a small bacon payload */
  : NODE_STALE            { $$ = BAMIPC_EVENT_NODE_STALE; }
  | NODE_FRESH            { $$ = BAMIPC_EVENT_NODE_FRESH; }
  | NODE_CREATE           { $$ = BAMIPC_EVENT_NODE_CREATE; }
  | NODE_DELETE           { $$ = BAMIPC_EVENT_NODE_DELETE; }
  | GLOBAL_INVALIDATION   { $$ = BAMIPC_EVENT_GLOBAL_INVALIDATION; }
  | GOALS_SET             { $$ = BAMIPC_EVENT_GOALS_SET; }
  | AUTORUN_SET           { $$ = BAMIPC_EVENT_AUTORUN_SET; }
  | SYSTEM_STATE_CHANGE   { $$ = BAMIPC_EVENT_SYSTEM_STATE_CHANGE; }
  | FORMULA_FORK          { $$ = BAMIPC_EVENT_FORMULA_FORK; }
  | FORMULA_WAIT          { $$ = BAMIPC_EVENT_FORMULA_WAIT; }
  | BUILD_START           { $$ = BAMIPC_EVENT_BUILD_START; }
  | BUILD_END             { $$ = BAMIPC_EVENT_BUILD_END; }
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
  | hex64
  ;

hex64
  : HEX8
  | HEX16
  | HEX32
  | HEX64
  ;

string
  : unquoted-string
  | quoted-string
  ;

unquoted-string
  : strpart
  {
    $$.s = @1.s;
    $$.l = @1.l;
  }
  ;

quoted-string
  : '"' strpart '"'
  {
    $$.s = @2.s;
    $$.l = @2.l;
  }
  ;

strpart
  : STR
  | uint
  ;
