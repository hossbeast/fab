/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <stdlib.h>

  #include "yyutil/parser.h"
  #include "yyutil/grammar.h"
  #include "common/attrs.h"
  #include "bam/graph.h"
  #include "bam/events.h"

  #include "args.h"
  #include "args/parser.h"

  /* commands */
  #include "adhoc.h"
  #include "autobuild.h"
  #include "build.h"
  #include "buildplan.h"
  #include "describe.h"
  #include "fstree.h"
  #include "filesystem.h"
  #include "events.h"
  #include "metadata.h"
  #include "config.h"
  #include "vars.h"
  #include "envs.h"
  #include "rules.h"
  #include "reconcile.h"
  #include "stats.h"
  #include "touch.h"
  #include "kill.h"
  #include "tree.h"
  #include "trace.h"
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
  bam_graph graph_type;
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
 BUILDPLAN                      "buildplan"
 DEPENDS                        "depends"
 FSTREE                         "fstree"
 FSTREE_HIDDEN                  "fstree-hidden"
 FILESYSTEM                     "filesystem"
 RELATION                       "relation"
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
 ENV                            "env"
 ENVS                           "envs"
 RULE                           "rule"
 RULES                          "rules"
 RECONCILE                      "reconcile"
 TRUE                           "true"
 FALSE                          "false"
 TRACE                          "trace"
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
 llabels                        "--labels"
 lspecial_only                  "--special-only"
 lno_special                    "--no-special"
 lfollow                        "--follow"
 lno_follow                     "--no-follow"
 lno_color                      "--no-color"
 lmodule                        "--module"
 lno_module                     "--no-module"
 lmax_distance                  "--max-distance"
 lmin_distance                  "--min-distance"

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
 sm   "-m"
 su   "-u"
 sv   "-v"
 svv  "-vv"
 svvv "-vvv"
 sV   "-V"
 st   "-t"
 sx   "-x"
 pt   "+t"
 px   "+x"
 sS   "-S"
 sL   "-L"

/* these are devel only */
%token
 lsystem_config_path            "--system-config-path"
 luser_config_path              "--user-config-path"
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
  | sm bead         { args_set_module_path($2.s, $2.len); }
  | lmodule bead    { args_set_module_path($2.s, $2.len); }
  | lno_module      { args_set_no_module(); }
  | lno_launch      { ARGS->no_launch = true; }
  | lno_color       { ARGS->no_color = true; }
  | lon_success_show_path bool                  { build_args.success.show_path = $2; }
  | lon_success_show_cwd bool                   { build_args.success.show_cwd = $2; }
  | lon_success_show_command bool               { build_args.success.show_command = $2; }
  | lon_success_show_arguments bool             { build_args.success.show_arguments = $2; }
  | lon_success_show_environment bool           { build_args.success.show_environment = $2; }
  | lon_success_show_sources bool               { build_args.success.show_sources = $2; }
  | lon_success_show_targets bool               { build_args.success.show_targets = $2; }
  | lon_success_show_status bool                { build_args.success.show_status = $2; }
  | lon_success_show_stdout bool                { build_args.success.show_stdout = $2; }
  | lon_success_show_stdout_limit_lines uint16  { build_args.success.show_stdout_limit_lines = $2; }
  | lon_success_show_stdout_limit_bytes uint16  { build_args.success.show_stdout_limit_bytes = $2; }
  | lon_success_show_stderr bool                { build_args.success.show_stderr = $2; }
  | lon_success_show_stderr_limit_lines uint16  { build_args.success.show_stderr_limit_lines = $2; }
  | lon_success_show_stderr_limit_bytes uint16  { build_args.success.show_stderr_limit_bytes = $2; }
  | lon_success_show_auxout bool                { build_args.success.show_auxout = $2; }
  | lon_success_show_auxout_limit_lines uint16  { build_args.success.show_auxout_limit_lines = $2; }
  | lon_success_show_auxout_limit_bytes uint16  { build_args.success.show_auxout_limit_bytes = $2; }
  | lon_error_show_path bool                    { build_args.error.show_path = $2; }
  | lon_error_show_cwd bool                     { build_args.error.show_cwd = $2; }
  | lon_error_show_command bool                 { build_args.error.show_command = $2; }
  | lon_error_show_arguments bool               { build_args.error.show_arguments = $2; }
  | lon_error_show_environment bool             { build_args.error.show_environment = $2; }
  | lon_error_show_sources bool                 { build_args.error.show_sources = $2; }
  | lon_error_show_targets bool                 { build_args.error.show_targets = $2; }
  | lon_error_show_status bool                  { build_args.error.show_status = $2; }
  | lon_error_show_stdout bool                  { build_args.error.show_stdout = $2; }
  | lon_error_show_stdout_limit_lines uint16    { build_args.error.show_stdout_limit_lines = $2; }
  | lon_error_show_stdout_limit_bytes uint16    { build_args.error.show_stdout_limit_bytes = $2; }
  | lon_error_show_stderr bool                  { build_args.error.show_stderr = $2; }
  | lon_error_show_stderr_limit_lines uint16    { build_args.error.show_stderr_limit_lines = $2; }
  | lon_error_show_stderr_limit_bytes uint16    { build_args.error.show_stderr_limit_bytes = $2; }
  | lon_error_show_auxout bool                  { build_args.error.show_auxout = $2; }
  | lon_error_show_auxout_limit_lines uint16    { build_args.error.show_auxout_limit_lines = $2; }
  | lon_error_show_auxout_limit_bytes uint16    { build_args.error.show_auxout_limit_bytes = $2; }

 /* devel only */
  | lsystem_config_path bead            { ARGS->system_config_path = $2; }
  | luser_config_path bead              { ARGS->user_config_path = $2; }
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
  : TREE         { args_command_start(&tree_command, 0); }        tree-arguments         { args_command_finish(); }
  | LS           { args_command_start(&tree_command, TREE_LS); }  tree-arguments         { args_command_finish(); }
  | BUILD        { args_command_start(&build_command, 0); }       build-arguments        { args_command_finish(); }
  | AUTOBUILD    { args_command_start(&autobuild_command, 0); }   build-arguments        { args_command_finish(); }
  | BUILDPLAN    { args_command_start(&buildplan_command, 0); }   buildplan-arguments    { args_command_finish(); }
  | TOUCH        { args_command_start(&touch_command, 0); }       touch-arguments        { args_command_finish(); }
  | STATS        { args_command_start(&stats_command, 0); }       stats-arguments        { args_command_finish(); }
  | ADHOC        { args_command_start(&adhoc_command, 0); }       adhoc-arguments        { args_command_finish(); }
  | EVENTS       { args_command_start(&events_command, 0); }      events-arguments       { args_command_finish(); }
  | DESCRIBE     { args_command_start(&describe_command, 0); }    describe-arguments     { args_command_finish(); }
  | FSTREE       { args_command_start(&fstree_command, 0); }      fstree-arguments       { args_command_finish(); }
  | FILESYSTEM   { args_command_start(&filesystem_command, 0); }  filesystem-arguments   { args_command_finish(); }
  | METADATA     { args_command_start(&metadata_command, 0); }    metadata-arguments     { args_command_finish(); }
  | CONFIG       { args_command_start(&config_command, 0); }      config-arguments       { args_command_finish(); }
  | VARS         { args_command_start(&vars_command, 0); }        vars-arguments         { args_command_finish(); }
  | ENV          { args_command_start(&envs_command, 0); }        envs-arguments         { args_command_finish(); }
  | ENVS         { args_command_start(&envs_command, 0); }        envs-arguments         { args_command_finish(); }
  | RULE         { args_command_start(&rule_command, 0); }        rule-arguments         { args_command_finish(); }
  | RULES        { args_command_start(&rules_command, 0); }       rules-arguments        { args_command_finish(); }
  | KILL         { args_command_start(&kill_command, 0); }        kill-arguments         { args_command_finish(); }
  | RECONCILE    { args_command_start(&reconcile_command, 0); }   reconcile-arguments    { args_command_finish(); }
  | TRACE        { args_command_start(&trace_command, 0); }       trace-arguments        { args_command_finish(); }
  | %empty                                                                               { args_command_finish(); }
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
    tree_args.graph = $2;
  }
  | up
  {
    tree_args.direction = "up";
    tree_args.both = false;
  }
  | down
  {
    tree_args.direction = "down";
    tree_args.both = false;
  }
  | both
  {
    tree_args.both = true;
  }
  | lpaths
  {
    tree_args.show_paths = true;
    tree_args.show_labels = false;
  }
  | llabels
  {
    tree_args.show_paths = false;
    tree_args.show_labels = true;
  }
  | bead
  {
    tree_args.target = $1;
  }
  | lmin_distance uint16
  {
    tree_args.min_distance = $2;
  }
  | lmax_distance uint16
  {
    tree_args.max_distance = $2;
  }
  | lno_follow
  {
    tree_args.nofollow = true;
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
    build_args.targets[build_args.targets_len].mode = 'x';
  }
  | st build-target
  {
    build_args.targets[build_args.targets_len].mode = 't';
  }
  | px
  {
    build_args.mode = 'x';
  }
  | pt
  {
    build_args.mode = 't';
  }
  ;

build-target
  : bead
  {
    build_args.targets[build_args.targets_len].s = @1.s;
    build_args.targets[build_args.targets_len].sl = @1.l;
    build_args.targets[build_args.targets_len].mode = build_args.mode;
    build_args.targets_len++;
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
  : FSTREE        { $$ = BAM_GRAPH_FSTREE; }
  | FSTREE_HIDDEN { $$ = BAM_GRAPH_FSTREE_HIDDEN; }
  | DIRTREE       { $$ = BAM_GRAPH_DIRTREE; }
  | MODTREE       { $$ = BAM_GRAPH_MODTREE; }
  | DEPENDS       { $$ = BAM_GRAPH_DEPENDS; }
  | IMPORTS       { $$ = BAM_GRAPH_MODULE_IMPORTS; }
  | USES          { $$ = BAM_GRAPH_MODULE_USES; }
  | REQUIRES      { $$ = BAM_GRAPH_MODULE_REQUIRES; }
  | RELATION      { $$ = BAM_GRAPH_RELATION; }
  ;

touch-arguments
  : touch-arguments touch-argument
  | touch-argument
  ;

touch-argument
  : global-argument
  | bead { touch_args.targets[touch_args.targets_len++] = $1; }
  ;

buildplan-arguments
  : buildplan-argument-list
  | %empty
  ;

buildplan-argument-list
  : buildplan-argument-list buildplan-argument
  | buildplan-argument
  ;

buildplan-argument
  : global-argument
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
  | lspecial_only
  {
    stats_args.special_only = true;
  }
  | sS
  {
    stats_args.special_only = true;
  }
  | lno_special
  {
    stats_args.no_special = true;
  }
  | bead { stats_args.targets[stats_args.targets_len++] = $1; }
  ;

adhoc-arguments
  : bead
  {
    adhoc_args.request = $1;
  }
  ;

describe-arguments
  : describe-arguments describe-argument
  | describe-argument
  ;

describe-argument
  : global-argument
  | bead { describe_args.targets[describe_args.targets_len++] = $1; }
  ;

fstree-arguments
  : fstree-argument-list
  | %empty
  ;

fstree-argument-list
  : fstree-argument-list fstree-argument
  | fstree-argument
  ;

fstree-argument
  : global-argument
  ;

filesystem-arguments
  : filesystem-argument-list
  | %empty
  ;

filesystem-argument-list
  : filesystem-argument-list filesystem-argument
  | filesystem-argument
  ;

filesystem-argument
  : global-argument
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

envs-arguments
  : envs-argument-list
  | %empty
  ;

envs-argument-list
  : envs-argument-list envs-argument
  | envs-argument
  ;

envs-argument
  : global-argument
  ;

rule-arguments
  : rule-argument-list
  | %empty
  ;

rule-argument-list
  : rule-argument-list rule-argument
  | rule-argument
  ;

rule-argument
  : global-argument
  | bead { rule_args.target = $1; }
  ;

rules-arguments
  : rules-argument-list
  | %empty
  ;

rules-argument-list
  : rules-argument-list rules-argument
  | rules-argument
  ;

rules-argument
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

trace-arguments
  : trace-script-path trace-argument-list
  | trace-script-path
  ;

trace-argument-list
  : trace-argument-list trace-argument
  | trace-argument
  ;

trace-argument
  : global-argument
  | trace-script-path
  ;

trace-script-path
  : bead { trace_args.script_path = $1; }
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
  | sa                { events_args.event_mask = ~0; }
  | STALE             { events_args.event_mask |= BAMIPC_EVENT_NODE_STALE; }
  | FRESH             { events_args.event_mask |= BAMIPC_EVENT_NODE_FRESH; }
  | CREATE            { events_args.event_mask |= BAMIPC_EVENT_NODE_CREATE; }
  | DELETE            { events_args.event_mask |= BAMIPC_EVENT_NODE_DELETE; }
  | GLOBAL_INVALIDATE { events_args.event_mask |= BAMIPC_EVENT_GLOBAL_INVALIDATE; }
  | EXEC_FORKED       { events_args.event_mask |= BAMIPC_EVENT_FORMULA_EXEC_FORKED; }
  | EXEC_WAITED       { events_args.event_mask |= BAMIPC_EVENT_FORMULA_EXEC_WAITED; }
  | EXEC_STDOUT       { events_args.event_mask |= BAMIPC_EVENT_FORMULA_EXEC_STDOUT; }
  | EXEC_STDERR       { events_args.event_mask |= BAMIPC_EVENT_FORMULA_EXEC_STDERR; }
  | EXEC_AUXOUT       { events_args.event_mask |= BAMIPC_EVENT_FORMULA_EXEC_AUXOUT; }
  | GOALS             { events_args.event_mask |= BAMIPC_EVENT_GOALS; }
  | SYSTEM_STATE      { events_args.event_mask |= BAMIPC_EVENT_SYSTEM_STATE; }
  ;

bead
  : strtoken
  {
    $$ = (bead){ s : @1.s, len : @1.l };
  }
  ;

strtoken
  : STR
  | BUILD
  | DEPENDS
  | FSTREE
  | RELATION
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
  | TRACE
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
  | uint16
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
