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
  #include <stdlib.h>

  #include "yyutil/parser.h"
  #include "yyutil/grammar.h"
  #include "common/attrs.h"
  #include "fab/graph.h"
  #include "fab/events.h"

  #include "args.h"
  #include "args/parser.h"

  /* commands */
  #include "adhoc.h"
  #include "autobuild.h"
  #include "build.h"
  #include "describe.h"
  #include "events.h"
  #include "metadata.h"
  #include "config.h"
  #include "vars.h"
  #include "reconcile.h"
  #include "stats.h"
  #include "touch.h"
  #include "kill.h"
  #include "tree.h"
  #include "ls.h"
}

%code top {
  extern int args_yylex(void *lval, void *loc, void *scanner);
  #define ARGS (&g_args)
  #define ARGS_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {args_yy}
%parse-param { void* scanner }
%parse-param { void* parser }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); }

%union {
  yyu_lval yyu;
  fab_graph graph_type;
  bead bead;
}

%type <graph_type> graph-type
%type <bead> bead
%type <yyu.umax> uint64
%type <yyu.umax> uint16
%type <yyu.b> bool

/* terminals */
%token             STR "string"
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

/* words */
%token
 BUILD                          "build"
 DEPENDS                        "depends"
 FSTREE                         "fstree"
 DIRTREE                        "dirtree"
 MODTREE                        "modtree"
 IMPORTS                        "imports"
 MODULES                        "modules"
 REQUIRES                       "requires"
 LS                             "ls"
 TREE                           "tree"
 USES                           "uses"
 AUTOBUILD                      "autobuild"
 ADHOC                          "adhoc"
 TOUCH                          "touch"
 KILL                           "kill"
 EVENTS                         "events"
 STATS                          "stats"
 DESCRIBE                       "describe"
 METADATA                       "metadata"
 CONFIG                         "config"
 VARS                           "vars"
 RECONCILE                      "reconcile"
 TRUE                           "true"
 FALSE                          "false"
/* events */
 STALE                          "stale"
 FRESH                          "fresh"
 CREATE                         "create"
 DELETE                         "delete"
 GLOBAL_INVALIDATE              "global-invalidate"
 EXEC_FORKED                    "exec-fork"
 EXEC_WAITED                    "exec-wait"
 EXEC_STDOUT                    "exec-stdout"
 EXEC_STDERR                    "exec-stderr"
 EXEC_AUXOUT                    "exec-auxout"
 GOALS                          "goals"
 SYSTEM_STATE                   "system-state"

/* long options */
%token
 lno_launch                     "--no-launch"
 lhelp                          "--help"
 lversion                       "--version"
 lboth                          "--both"
 lup                            "--up"
 ldown                          "--down"
 lgraph                         "--graph"
 lpaths                         "--paths"

 lon_success_show_path                "--on-success-show-path"
 lon_success_show_cwd                 "--on-success-show-cwd"
 lon_success_show_command             "--on-success-show-command"
 lon_success_show_arguments           "--on-success-show-arguments"
 lon_success_show_environment         "--on-success-show-environment"
 lon_success_show_sources             "--on-success-show-sources"
 lon_success_show_targets             "--on-success-show-targets"
 lon_success_show_status              "--on-success-show-status"
 lon_success_show_stdout              "--on-success-show-stdout"
 lon_success_show_stdout_limit_lines  "--on-success-show-stdout-limit-lines"
 lon_success_show_stdout_limit_bytes  "--on-success-show-stdout-limit-bytes"
 lon_success_show_stderr              "--on-success-show-stderr"
 lon_success_show_stderr_limit_lines  "--on-success-show-stderr-limit-lines"
 lon_success_show_stderr_limit_bytes  "--on-success-show-stderr-limit-bytes"
 lon_success_show_auxout              "--on-success-show-auxout"
 lon_success_show_auxout_limit_lines  "--on-success-show-auxout-limit-lines"
 lon_success_show_auxout_limit_bytes  "--on-success-show-auxout-limit-bytes"

 lon_error_show_path                  "--on-error-show-path"
 lon_error_show_cwd                   "--on-error-show-cwd"
 lon_error_show_command               "--on-error-show-command"
 lon_error_show_arguments             "--on-error-show-arguments"
 lon_error_show_environment           "--on-error-show-environment"
 lon_error_show_sources               "--on-error-show-sources"
 lon_error_show_targets               "--on-error-show-targets"
 lon_error_show_status                "--on-error-show-status"
 lon_error_show_stdout                "--on-error-show-stdout"
 lon_error_show_stdout_limit_lines    "--on-error-show-stdout-limit-lines"
 lon_error_show_stdout_limit_bytes    "--on-error-show-stdout-limit-bytes"
 lon_error_show_stderr                "--on-error-show-stderr"
 lon_error_show_stderr_limit_lines    "--on-error-show-stderr-limit-lines"
 lon_error_show_stderr_limit_bytes    "--on-error-show-stderr-limit-bytes"
 lon_error_show_auxout                "--on-error-show-auxout"
 lon_error_show_auxout_limit_lines    "--on-error-show-auxout-limit-lines"
 lon_error_show_auxout_limit_bytes    "--on-error-show-auxout-limit-bytes"

/* short options */
%token
 sB   "-B"
 sK   "-K"
 sa   "-a"
 sb   "-b"
 sd   "-d"
 sg   "-g"
 sh   "-h"
 su   "-u"
 sv   "-v"
 svv  "-vv"
 svvv "-vvv"
 sV   "-V"
 st   "-t"
 sx   "-x"
 pt   "+t"
 px   "+x"

/* these are devel only */
%token
 lsystem_config_path            "--system-config-path"
 luser_config_path              "--user-config-path"
 lproject_config_path           "--project-config-path"
 ldefault_filesystem_invalidate "--default-filesystem-invalidate"
 lsweeper_period_nsec           "--sweeper-period-nsec"

%%
utterance
  : arguments
  ;

arguments
  : initial-arguments command-arguments
  ;

initial-arguments
  : initial-arguments-list
  | %empty
  ;

initial-arguments-list
  : initial-arguments-list global-argument
  | global-argument
  ;

global-argument
  : help            { ARGS->help = true; }
  | version         { ARGS->version = true; }
  | sv              { ARGS->verbose++; }
  | svv             { ARGS->verbose += 2; }
  | svvv            { ARGS->verbose += 3; }
  | sK              { ARGS->kill = true; }
  | sB              { ARGS->invalidate = true; }
  | lno_launch      { ARGS->no_launch = true; }

  | lon_success_show_path bool                  { ARGS->build->success.show_path = $2; }
  | lon_success_show_cwd bool                   { ARGS->build->success.show_cwd = $2; }
  | lon_success_show_command bool               { ARGS->build->success.show_command = $2; }
  | lon_success_show_arguments bool             { ARGS->build->success.show_arguments = $2; }
  | lon_success_show_environment bool           { ARGS->build->success.show_environment = $2; }
  | lon_success_show_sources bool               { ARGS->build->success.show_sources = $2; }
  | lon_success_show_targets bool               { ARGS->build->success.show_targets = $2; }
  | lon_success_show_status bool                { ARGS->build->success.show_status = $2; }
  | lon_success_show_stdout bool                { ARGS->build->success.show_stdout = $2; }
  | lon_success_show_stdout_limit_lines uint16  { ARGS->build->success.show_stdout_limit_lines = $2; }
  | lon_success_show_stdout_limit_bytes uint16  { ARGS->build->success.show_stdout_limit_bytes = $2; }
  | lon_success_show_stderr bool                { ARGS->build->success.show_stderr = $2; }
  | lon_success_show_stderr_limit_lines uint16  { ARGS->build->success.show_stderr_limit_lines = $2; }
  | lon_success_show_stderr_limit_bytes uint16  { ARGS->build->success.show_stderr_limit_bytes = $2; }
  | lon_success_show_auxout bool                { ARGS->build->success.show_auxout = $2; }
  | lon_success_show_auxout_limit_lines uint16  { ARGS->build->success.show_auxout_limit_lines = $2; }
  | lon_success_show_auxout_limit_bytes uint16  { ARGS->build->success.show_auxout_limit_bytes = $2; }
  | lon_error_show_path bool                    { ARGS->build->error.show_path = $2; }
  | lon_error_show_cwd bool                     { ARGS->build->error.show_cwd = $2; }
  | lon_error_show_command bool                 { ARGS->build->error.show_command = $2; }
  | lon_error_show_arguments bool               { ARGS->build->error.show_arguments = $2; }
  | lon_error_show_environment bool             { ARGS->build->error.show_environment = $2; }
  | lon_error_show_sources bool                 { ARGS->build->error.show_sources = $2; }
  | lon_error_show_targets bool                 { ARGS->build->error.show_targets = $2; }
  | lon_error_show_status bool                  { ARGS->build->error.show_status = $2; }
  | lon_error_show_stdout bool                  { ARGS->build->error.show_stdout = $2; }
  | lon_error_show_stdout_limit_lines uint16    { ARGS->build->error.show_stdout_limit_lines = $2; }
  | lon_error_show_stdout_limit_bytes uint16    { ARGS->build->error.show_stdout_limit_bytes = $2; }
  | lon_error_show_stderr bool                  { ARGS->build->error.show_stderr = $2; }
  | lon_error_show_stderr_limit_lines uint16    { ARGS->build->error.show_stderr_limit_lines = $2; }
  | lon_error_show_stderr_limit_bytes uint16    { ARGS->build->error.show_stderr_limit_bytes = $2; }
  | lon_error_show_auxout bool                  { ARGS->build->error.show_auxout = $2; }
  | lon_error_show_auxout_limit_lines uint16    { ARGS->build->error.show_auxout_limit_lines = $2; }
  | lon_error_show_auxout_limit_bytes uint16    { ARGS->build->error.show_auxout_limit_bytes = $2; }

 /* devel only */
  | lsystem_config_path bead            { ARGS->system_config_path = $2; }
  | luser_config_path bead              { ARGS->user_config_path = $2; }
  | lproject_config_path bead           { ARGS->project_config_path = $2; }
  | ldefault_filesystem_invalidate bead { ARGS->default_filesystem_invalidate = $2; }
  | lsweeper_period_nsec uint64         { ARGS->sweeper_period_nsec = (bead) { @2.s, @2.l }; }
  ;

help
  : lhelp
  | sh
  ;

version
  : lversion
  | sV
  ;

command-arguments
  : TREE      tree-arguments      { g_cmd = &tree_command; }
  | LS        ls-arguments        { g_cmd = &ls_command; }
  | BUILD     build-arguments     { g_cmd = &build_command; }
  | AUTOBUILD build-arguments     { g_cmd = &autobuild_command; }
  | TOUCH     touch-arguments     { g_cmd = &touch_command; }
  | STATS     stats-arguments     { g_cmd = &stats_command; }
  | ADHOC     adhoc-arguments     { g_cmd = &adhoc_command; }
  | EVENTS    events-arguments    { g_cmd = &events_command; }
  | DESCRIBE  describe-arguments  { g_cmd = &describe_command; }
  | METADATA  metadata-arguments  { g_cmd = &metadata_command; }
  | CONFIG    config-arguments    { g_cmd = &config_command; }
  | VARS      vars-arguments      { g_cmd = &vars_command; }
  | KILL      kill-arguments      { g_cmd = &kill_command; ARGS->kill = true; }
  | RECONCILE reconcile-arguments { g_cmd = &reconcile_command; }
  | %empty                        { g_cmd = &build_command; }
  ;

tree-arguments
  : tree-argument-list
  | %empty
  ;

tree-argument-list
  : tree-argument-list tree-argument
  | tree-argument
  ;

tree-argument
  : global-argument
  | graph graph-type
  {
    ARGS->tree->graph = $2;
  }
  | up
  {
    ARGS->tree->direction = "up";
    ARGS->tree->both = false;
  }
  | down
  {
    ARGS->tree->direction = "down";
    ARGS->tree->both = false;
  }
  | both
  {
    ARGS->tree->both = true;
  }
  | lpaths
  {
    ARGS->tree->show_paths = true;
  }
  | bead
  {
    ARGS->tree->target = $1;
  }
  ;

ls-arguments
  : ls-argument-list
  | %empty
  ;

ls-argument-list
  : ls-argument-list ls-argument
  | ls-argument
  ;

ls-argument
  : global-argument
  | graph graph-type
  {
    ARGS->ls->graph = $2;
  }
  | up
  {
    ARGS->ls->direction = "up";
  }
  | down
  {
    ARGS->ls->direction = "down";
  }
  | lpaths
  {
    ARGS->ls->show_paths = true;
  }
  | bead
  {
    ARGS->ls->target = $1;
  }
  ;

build-arguments
  : build-argument-list
  | %empty
  ;

build-argument-list
  : build-argument-list build-argument
  | build-argument
  ;

build-argument
  : global-argument
  | build-target
  | sx build-target
  {
    ARGS->build->targets[ARGS->build->targets_len].mode = 'x';
  }
  | st build-target
  {
    ARGS->build->targets[ARGS->build->targets_len].mode = 't';
  }
  | px
  {
    ARGS->build->mode = 'x';
  }
  | pt
  {
    ARGS->build->mode = 't';
  }
  ;

build-target
  : bead
  {
    ARGS->build->targets[ARGS->build->targets_len].s = @1.s;
    ARGS->build->targets[ARGS->build->targets_len].sl = @1.l;
    ARGS->build->targets[ARGS->build->targets_len].mode = ARGS->build->mode;
    ARGS->build->targets_len++;
  }
  ;

graph
  : lgraph
  | sg
  ;

up
  : lup
  | su
  ;

down
  : ldown
  | sd
  ;

both
  : lboth
  | sb
  ;

graph-type
  : FSTREE     { $$ = FAB_GRAPH_FSTREE; }
  | DIRTREE    { $$ = FAB_GRAPH_DIRTREE; }
  | MODTREE    { $$ = FAB_GRAPH_MODTREE; }
  | DEPENDS    { $$ = FAB_GRAPH_DEPENDS; }
  | IMPORTS    { $$ = FAB_GRAPH_MODULE_IMPORTS; }
  | USES       { $$ = FAB_GRAPH_MODULE_USES; }
  | REQUIRES   { $$ = FAB_GRAPH_MODULE_REQUIRES; }
  ;

touch-arguments
  : touch-arguments touch-argument
  | touch-argument
  ;

touch-argument
  : global-argument
  | bead { ARGS->touch->targets[ARGS->touch->targets_len++] = $1; }
  ;

kill-arguments
  : kill-argument-list
  | %empty
  ;

kill-argument-list
  : kill-argument-list kill-argument
  | kill-argument
  ;

kill-argument
  : global-argument
  ;

reconcile-arguments
  : reconcile-arguments-list
  | %empty;

reconcile-arguments-list
  : reconcile-arguments-list reconcile-argument
  | reconcile-argument
  ;

reconcile-argument
  : global-argument
  ;

stats-arguments
  : stats-argument-list
  | %empty
  ;

stats-argument-list
  : stats-argument-list stats-argument
  | stats-argument
  ;

stats-argument
  : global-argument
  | bead { ARGS->stats->targets[ARGS->stats->targets_len++] = $1; }
  ;

adhoc-arguments
  : bead
  {
    ARGS->adhoc->request = $1;
  }
  ;

describe-arguments
  : describe-arguments describe-argument
  | describe-argument
  ;

describe-argument
  : global-argument
  | bead { ARGS->describe->targets[ARGS->describe->targets_len++] = $1; }
  ;

config-arguments
  : config-argument-list
  | %empty
  ;

config-argument-list
  : config-argument-list config-argument
  | config-argument
  ;

config-argument
  : global-argument
  ;

vars-arguments
  : vars-argument-list
  | %empty
  ;

vars-argument-list
  : vars-argument-list vars-argument
  | vars-argument
  ;

vars-argument
  : global-argument
  ;

metadata-arguments
  : metadata-argument-list
  | %empty
  ;

metadata-argument-list
  : metadata-argument-list metadata-argument
  | metadata-argument
  ;

metadata-argument
  : global-argument
  ;

events-arguments
  : events-argument-list
  | %empty;

events-argument-list
  : events-argument-list events-argument
  | events-argument
  ;

events-argument
  : global-argument
  | sa                { ARGS->events->event_mask = ~0; }
  | STALE             { ARGS->events->event_mask |= FABIPC_EVENT_NODE_STALE; }
  | FRESH             { ARGS->events->event_mask |= FABIPC_EVENT_NODE_FRESH; }
  | CREATE            { ARGS->events->event_mask |= FABIPC_EVENT_NODE_CREATE; }
  | DELETE            { ARGS->events->event_mask |= FABIPC_EVENT_NODE_DELETE; }
  | GLOBAL_INVALIDATE { ARGS->events->event_mask |= FABIPC_EVENT_GLOBAL_INVALIDATE; }
  | EXEC_FORKED       { ARGS->events->event_mask |= FABIPC_EVENT_FORMULA_EXEC_FORKED; }
  | EXEC_WAITED       { ARGS->events->event_mask |= FABIPC_EVENT_FORMULA_EXEC_WAITED; }
  | EXEC_STDOUT       { ARGS->events->event_mask |= FABIPC_EVENT_FORMULA_EXEC_STDOUT; }
  | EXEC_STDERR       { ARGS->events->event_mask |= FABIPC_EVENT_FORMULA_EXEC_STDERR; }
  | EXEC_AUXOUT       { ARGS->events->event_mask |= FABIPC_EVENT_FORMULA_EXEC_AUXOUT; }
  | GOALS             { ARGS->events->event_mask |= FABIPC_EVENT_GOALS; }
  | SYSTEM_STATE      { ARGS->events->event_mask |= FABIPC_EVENT_SYSTEM_STATE; }
  ;

bead
  : strtoken
  {
    $$ = (bead){ s : @1.s, len : @1.l };
  }

strtoken
  : STR
  | BUILD
  | DEPENDS
  | FSTREE
  | DIRTREE
  | MODTREE
  | IMPORTS
  | MODULES
  | REQUIRES
  | LS
  | TREE
  | USES
  | AUTOBUILD
  | ADHOC
  | TOUCH
  | KILL
  | EVENTS
  | STATS
  | DESCRIBE
  | METADATA
  | CONFIG
  | VARS
  | RECONCILE
  | TRUE
  | FALSE
  | STALE
  | FRESH
  | CREATE
  | DELETE
  | GLOBAL_INVALIDATE
  | EXEC_FORKED
  | EXEC_WAITED
  | EXEC_STDOUT
  | EXEC_STDERR
  | EXEC_AUXOUT
  | GOALS
  | SYSTEM_STATE
  | '+'
  ;

bool
  : TRUE  { $$ = true; }
  | FALSE { $$ = false; }
  ;

uint16
  : INTMAX8   { $$ = $1; }
  | INTMAX16  { $$ = $1; }
  | UINTMAX8
  | UINTMAX16
  ;

uint64
  : INTMAX8   { $$ = $1; }
  | INTMAX16  { $$ = $1; }
  | INTMAX32  { $$ = $1; }
  | INTMAX64  { $$ = $1; }
  | UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  ;
