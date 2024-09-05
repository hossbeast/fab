/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <stdlib.h>

  #include "yyutil/parser.h"
  #include "yyutil/grammar.h"
  #include "common/attrs.h"
  #include "bam/events.h"

  #include "args.h"
  #include "args/parser.h"

  /* commands */
  #include "adhoc.h"
  #include "autobuild.h"
  #include "build.h"
  #include "buildplan.h"
  #include "describe.h"
  #include "fstrees.h"
  #include "events.h"
  #include "config.h"
  #include "configsets.h"
  #include "vars.h"
  #include "varsets.h"
  #include "envs.h"
  #include "goals.h"
  #include "rules.h"
  #include "rmas.h"
  #include "reconcile.h"
  #include "stats.h"
  #include "touch.h"
  #include "kill.h"
  #include "tree.h"
  #include "trace.h"
  #include "plugins.h"
  #include "formulas.h"
  #include "rulesets.h"
  #include "fsnames.h"
  #include "modules.h"
  #include "info.h"
  #include "zcat.h"
}

%code top {
  extern int args_yylex(void *lval, void *loc, void *scanner);
  #define ARGS (&g_args)
  #define CMD (g_cmd)
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
  bead bead;
}

%type <bead> bead
%type <yyu.umax> hex64
%type <yyu.umax> uint64
%type <yyu.umax> uint16
%type <yyu.b> bool

/* terminals */
%token             STR "string"
%token <yyu.umax>  HEX8
%token <yyu.umax>  HEX16
%token <yyu.umax>  HEX32
%token <yyu.umax>  HEX64
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
 SPECIAL                        "special"
 DIRECTORY                      "directory"
 MODULE_DIR                     "module-dir"
 FS                             "fs"
 FSTREE                         "fstree"
 FSTREES                        "fstrees"
 RELATION                       "relation"
 DIRTREE                        "dirtree"
 MODTREE                        "modtree"
 IMPORTS                        "imports"
 MODULES                        "modules"
 MODULE                         "module"
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
 INFO                           "info"
 CONFIG                         "config"
 CONFIGSET                      "configset"
 CONFIGSETS                     "configsets"
 VARS                           "vars"
 ENVS                           "envs"
 GOALS                          "goals"
 RULE                           "rule"
 RULES                          "rules"
 RMA                            "rma"
 RMAS                           "rmas"
 RECONCILE                      "reconcile"
 TRUE                           "true"
 FALSE                          "false"
 TRACE                          "trace"
 PLUGIN                         "plugin"
 PLUGINS                        "plugins"
 FORMULA                        "formula"
 FORMULAS                       "formulas"
 RULESET                        "ruleset"
 RULESETS                       "rulesets"
 VARSET                         "varset"
 VARSETS                        "varsets"
 FSNAME                         "fsname"
 FSNAMES                        "fsnames"
/* events */
 STALE                          "stale"
 FRESH                          "fresh"
 CREATE                         "create"
 DELETE                         "delete"
 GLOBAL_INVALIDATION            "global-invalidation"
 EXEC_FORKED                    "exec-fork"
 EXEC_WAITED                    "exec-wait"
 EXEC_STDOUT                    "exec-stdout"
 EXEC_STDERR                    "exec-stderr"
 EXEC_AUXOUT                    "exec-auxout"
 GOALS_SET                      "goals-set"
 AUTORUN_SET                    "autorun-set"
 SYSTEM_STATE_CHANGE            "system-state-change"

/* long options */
%token
 lno_launch                     "--no-launch"
 lhelp                          "--help"
 lversion                       "--version"
 lboth                          "--both"
 lup                            "--up"
 ldown                          "--down"
 lhidden                        "--hidden"
 lgraph                         "--graph"
 lvisit                         "--visit"
 lpaths                         "--paths"
 llabels                        "--labels"
 lspecial                       "--special"
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
 ss   "-s"
 sS   "-S"
 sL   "-L"

/* these are devel only */
%token
 lsystem_config_path              "--system-config-path"
 luser_config_path                "--user-config-path"
 ldefault_filesystem_invalidation "--default-filesystem-invalidation"
 lsweeper_period_nsec             "--sweeper-period-nsec"

%%
utterance
  : arguments
  ;

arguments
  : initial-arguments command-arguments
  | initial-arguments
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
  | lsystem_config_path bead              { ARGS->system_config_path = $2; }
  | luser_config_path bead                { ARGS->user_config_path = $2; }
  | ldefault_filesystem_invalidation bead { ARGS->default_filesystem_invalidation = $2; }
  | lsweeper_period_nsec uint64           { ARGS->sweeper_period_nsec = (bead) { @2.s, @2.l }; }
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
  : ADHOC        { command_set(&adhoc_command); }      adhoc-arguments
  | AUTOBUILD    { command_set(&autobuild_command); }  build-arguments
  | BUILD        { command_set(&build_command); }      build-arguments
  | BUILDPLAN    { command_set(&buildplan_command); }  buildplan-arguments
  | CONFIGSET    { command_set(&configset_command); }  configset-arguments
  | CONFIGSETS   { command_set(&configsets_command); } configsets-arguments
  | DESCRIBE     { command_set(&describe_command); }   describe-arguments
  | EVENTS       { command_set(&events_command); }     events-arguments
  | FORMULA      { command_set(&formula_command); }    formula-arguments
  | FORMULAS     { command_set(&formulas_command); }   formulas-arguments
  | FSTREE       { command_set(&fstree_command); }     fstree-arguments
  | FSTREES      { command_set(&fstrees_command); }    fstrees-arguments
  | GOALS        { command_set(&goals_command); }      goals-arguments
  | INFO         { command_set(&info_command); }       info-arguments
  | MODULES      { command_set(&modules_command); }    modules-arguments
  | PLUGIN       { command_set(&plugin_command); }     plugin-arguments
  | PLUGINS      { command_set(&plugins_command); }    plugins-arguments
  | RECONCILE    { command_set(&reconcile_command); }  reconcile-arguments
  | RULESET      { command_set(&ruleset_command); }    ruleset-arguments
  | RULESETS     { command_set(&rulesets_command); }   rulesets-arguments
  | STATS        { command_set(&stats_command); }      stats-arguments
  | TOUCH        { command_set(&touch_command); }      touch-arguments
  | TRACE        { command_set(&trace_command); }      trace-arguments
  | VARSET       { command_set(&varset_command); }     varset-arguments
  | VARSETS      { command_set(&varsets_command); }    varsets-arguments
  | FSNAME       { command_set(&fsname_command); }     fsname-arguments
  | FSNAMES      { command_set(&fsnames_command); }    fsnames-arguments
  | rules-command
  | rule-command
  | rmas-command
  | rma-command
  | config-command
  | tree-command
  | ls-command
  | kill-command
  | module-command
  ;

config-command
  : CONFIG { command_set(&config_command); } config-arguments
  ;

kill-command
  : KILL { command_set(&kill_command); } kill-arguments
  {
    kill_init();
  }
  ;

tree-command
  : TREE { command_set(&tree_command); tree_init(0); } tree-arguments
  ;

ls-command
  : LS { command_set(&tree_command); tree_init(TREE_LS); } tree-arguments
  ;

module-command
  : MODULE { command_set(&module_command); } module-arguments module-subcommand
  ;

module-arguments
  : module-argument-list
  | %empty
  ;

module-argument-list
  : module-argument-list module-argument
  | module-argument
  ;

module-argument
  : global-argument
  | hex64 { CMD->target = @1.s; CMD->target_len = @1.l; }
  | bead { CMD->target = @1.s; CMD->target_len = @1.l; }
  ;

module-subcommand
  : envs-command
  | vars-command
  | module-rules-command
  | module-rmas-command
  | %empty
  ;

envs-command
  : ENVS { command_sub_set(&envs_command); } envs-arguments
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

vars-command
  : VARS { command_sub_set(&vars_command); } vars-arguments
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

module-rules-command
  : RULES { command_sub_set(&module_rules_command); } module-rules-arguments
  ;

module-rules-arguments
  : module-rules-argument-list
  | %empty
  ;

module-rules-argument-list
  : module-rules-argument-list module-rules-argument
  | module-rules-argument
  ;

module-rules-argument
  : global-argument
  ;

module-rmas-command
  : RMAS { command_sub_set(&module_rmas_command); } module-rmas-arguments
  ;

module-rmas-arguments
  : module-rmas-argument-list
  | %empty
  ;

module-rmas-argument-list
  : module-rmas-argument-list module-rmas-argument
  | module-rmas-argument
  ;

module-rmas-argument
  : global-argument
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
    tree_args.graph = @2.s;
    tree_args.graph_len = @2.l;
  }
  | visit visit-type
  {
    tree_args.visit = @2.s;
    tree_args.visit_len = @2.l;
  }
  | up
  {
    tree_args.direction = @1.s;
    tree_args.direction_len = @1.l;
    tree_args.both = false;
  }
  | down
  {
    tree_args.direction = @1.s;
    tree_args.direction_len = @1.l;
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
  | lhidden
  {
    tree_args.hidden = true;
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

visit
  : lvisit
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
  : FS
  | DEPENDS
  | SPECIAL
  | USES
  | REQUIRES
  ;

visit-type
  : MODULE
  | RULESET
  | FORMULA
  | VARSET
  | CONFIGSET
  | PLUGIN
  | DIRECTORY
  | MODULE_DIR
  | RULE
  | RMA
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

plugins-arguments
  : plugins-argument-list
  | %empty
  ;

plugins-argument-list
  : plugins-argument-list plugins-argument
  | plugins-argument
  ;

plugins-argument
  : global-argument
  ;

plugin-arguments
  : plugin-argument-list
  ;

plugin-argument-list
  : plugin-argument-list plugin-argument
  | plugin-argument
  ;

plugin-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

varsets-arguments
  : varsets-argument-list
  | %empty
  ;

varsets-argument-list
  : varsets-argument-list varsets-argument
  | varsets-argument
  ;

varsets-argument
  : global-argument
  ;

varset-arguments
  : varset-argument-list
  ;

varset-argument-list
  : varset-argument-list varset-argument
  | varset-argument
  ;

varset-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

fsnames-arguments
  : fsnames-argument-list
  | %empty
  ;

fsnames-argument-list
  : fsnames-argument-list fsnames-argument
  | fsnames-argument
  ;

fsnames-argument
  : global-argument
  ;

fsname-arguments
  : fsname-argument-list
  ;

fsname-argument-list
  : fsname-argument-list fsname-argument
  | fsname-argument
  ;

fsname-argument
  : global-argument
  | hex64 { CMD->target = @1.s; CMD->target_len = @1.l; }
  | bead { CMD->target = @1.s; CMD->target_len = @1.l; }
  ;

rulesets-arguments
  : rulesets-argument-list
  | %empty
  ;

rulesets-argument-list
  : rulesets-argument-list rulesets-argument
  | rulesets-argument
  ;

rulesets-argument
  : global-argument
  ;

ruleset-arguments
  : ruleset-argument-list
  ;

ruleset-argument-list
  : ruleset-argument-list ruleset-argument
  | ruleset-argument
  ;

ruleset-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

formulas-arguments
  : formulas-argument-list
  | %empty
  ;

formulas-argument-list
  : formulas-argument-list formulas-argument
  | formulas-argument
  ;

formulas-argument
  : global-argument
  ;

formula-arguments
  : formula-argument-list
  ;

formula-argument-list
  : formula-argument-list formula-argument
  | formula-argument
  ;

formula-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

modules-arguments
  : modules-argument-list
  | %empty
  ;

modules-argument-list
  : modules-argument-list modules-argument
  | modules-argument
  ;

modules-argument
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
  | ss
  {
    CMD->special += 1;
  }
  | sS
  {
    CMD->special += 2;
  }
  | lspecial
  {
    CMD->special++;
  }
  | lno_special
  {
    CMD->special = 0;
  }
  | bead
  {
    CMD->targets[CMD->targets_len++] = $1;
  }
  ;

adhoc-arguments
  : bead { adhoc_args.request = $1; }
  ;

describe-arguments
  : describe-arguments describe-argument
  | describe-argument
  ;

describe-argument
  : global-argument
  | ss
  {
    CMD->special += 1;
  }
  | sS
  {
    CMD->special += 2;
  }
  | lspecial
  {
    CMD->special++;
  }
  | lno_special
  {
    CMD->special = 0;
  }
  | bead { CMD->targets[CMD->targets_len++] = $1; }
  ;

fstrees-arguments
  : fstrees-argument-list
  | %empty
  ;

fstrees-argument-list
  : fstrees-argument-list fstrees-argument
  | fstrees-argument
  ;

fstrees-argument
  : global-argument
  ;

fstree-arguments
  : fstree-argument-list
  ;

fstree-argument-list
  : fstree-argument-list fstree-argument
  | fstree-argument
  ;

fstree-argument
  : global-argument
  | hex64 { CMD->fstree_id_len = zcatw(CMD->fstree_id, sizeof(CMD->fstree_id), @1.s, @1.l); }
  | bead { CMD->target = @1.s; CMD->target_len = @1.l; }
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

configsets-arguments
  : configsets-argument-list
  | %empty
  ;

configsets-argument-list
  : configsets-argument-list configsets-argument
  | configsets-argument
  ;

configsets-argument
  : global-argument
  ;

configset-arguments
  : configset-argument-list
  ;

configset-argument-list
  : configset-argument-list configset-argument
  | configset-argument
  ;

configset-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

rules-command
  : RULES { command_set(&rules_command); } rules-arguments
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

rule-command
  : RULE { command_set(&rule_command); } rule-arguments
  ;

rule-arguments
  : rule-argument-list
  ;

rule-argument-list
  : rule-argument-list rule-argument
  | rule-argument
  ;

rule-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

rmas-command
  : RMAS { command_set(&rmas_command); } rmas-arguments
  ;

rmas-arguments
  : rmas-argument-list
  | %empty
  ;

rmas-argument-list
  : rmas-argument-list rmas-argument
  | rmas-argument
  ;

rmas-argument
  : global-argument
  ;

rma-command
  : RMA { command_set(&rma_command); } rma-arguments
  ;

rma-arguments
  : rma-argument-list
  ;

rma-argument-list
  : rma-argument-list rma-argument
  | rma-argument
  ;

rma-argument
  : global-argument
  | hex64 { CMD->id = $1; }
  | bead { CMD->special_target = $1; }
  ;

goals-arguments
  : goals-argument-list
  | %empty
  ;

goals-argument-list
  : goals-argument-list goals-argument
  | goals-argument
  ;

goals-argument
  : global-argument
  ;

info-arguments
  : info-argument-list
  | %empty
  ;

info-argument-list
  : info-argument-list info-argument
  | info-argument
  ;

info-argument
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
  | sa                  { events_args.event_mask = ~0; }
  | STALE               { events_args.event_mask |= BAMIPC_EVENT_NODE_STALE; }
  | FRESH               { events_args.event_mask |= BAMIPC_EVENT_NODE_FRESH; }
  | CREATE              { events_args.event_mask |= BAMIPC_EVENT_NODE_CREATE; }
  | DELETE              { events_args.event_mask |= BAMIPC_EVENT_NODE_DELETE; }
  | GLOBAL_INVALIDATION { events_args.event_mask |= BAMIPC_EVENT_GLOBAL_INVALIDATION; }
  | EXEC_FORKED         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_FORK_BIN; }
  | EXEC_WAITED         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_WAIT_BIN; }
  | EXEC_STDOUT         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_STDOUT_BIN; }
  | EXEC_STDERR         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_STDERR_BIN; }
  | EXEC_AUXOUT         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_AUXOUT_BIN; }
  | GOALS_SET           { events_args.event_mask |= BAMIPC_EVENT_GOALS_SET; }
  | AUTORUN_SET         { events_args.event_mask |= BAMIPC_EVENT_AUTORUN_SET; }
  | SYSTEM_STATE_CHANGE { events_args.event_mask |= BAMIPC_EVENT_SYSTEM_STATE_CHANGE; }
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
  | FS
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
  | INFO
  | TRACE
  | CONFIG
  | CONFIGSET
  | CONFIGSETS
  | FSNAME
  | FSNAMES
  | VARSET
  | VARSETS
  | RECONCILE
  | TRUE
  | FALSE
  | STALE
  | FRESH
  | CREATE
  | DELETE
  | GLOBAL_INVALIDATION
  | EXEC_FORKED
  | EXEC_WAITED
  | EXEC_STDOUT
  | EXEC_STDERR
  | EXEC_AUXOUT
  | GOALS_SET
  | AUTORUN_SET
  | SYSTEM_STATE_CHANGE
  | PLUGIN
  | PLUGINS
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

hex64
  : HEX8
  | HEX16
  | HEX32
  | HEX64
  ;
