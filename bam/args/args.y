/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <stdlib.h>
  #include <inttypes.h>

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
  #include "plan.h"
  #include "describe.h"
  #include "blame.h"
  #include "fstrees.h"
  #include "namespaces.h"
  #include "events.h"
  #include "config.h"
  #include "configsets.h"
  #include "vars.h"
  #include "varsets.h"
  #include "build_records.h"
  #include "envs.h"
  #include "goals.h"
  #include "rules.h"
  #include "rmas.h"
  #include "reconcile.h"
  #include "stats.h"
  #include "touch.h"
  #include "kill.h"
  #include "tree.h"
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
  #define G_ARGS (&g_args)
  #define PARSER ((yyu_parser*)parser)
  #define CMD (g_cmd)
  #define ARGS_YYLTYPE yyu_location
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {args_yy}
%parse-param { void *scanner }
%parse-param { void *parser }
%lex-param { void *scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;
}

%type <yyu.umax> uint64
%type <yyu.umax> uint16
%type <yyu.b> bool

/* terminals */
%token             STR "string"
%token             BECH32 "bech32"
%token <yyu.umax>  UINTMAX8
%token <yyu.umax>  UINTMAX16
%token <yyu.umax>  UINTMAX32
%token <yyu.umax>  UINTMAX64
%token <yyu.imax>  INTMAX8
%token <yyu.imax>  INTMAX16
%token <yyu.imax>  INTMAX32
%token <yyu.imax>  INTMAX64

/* words */
%token
 BUILD                          "build"
 PLAN                           "plan"
 DEPENDS                        "depends"
 SPECIAL                        "special"
 DIRECTORY                      "directory"
 FS                             "fs"
 FSTREE                         "fstree"
 FSTREES                        "fstrees"
 NAMESPACE                      "namespace"
 NAMESPACES                     "namespaces"
 MODULES                        "modules"
 MODULE                         "module"
 RECORDS                        "records"
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
 BLAME                          "blame"
 INFO                           "info"
 CONFIG                         "config"
 CONFIGSET                      "configset"
 CONFIGSETS                     "configsets"
 BUILD_RECORD                   "build-record"
 BUILD_RECORDS                  "build-records"
 VARS                           "vars"
 ENVS                           "envs"
 ARGS                           "args"
 D_FILE                         "file"
 CWD                            "cwd"
 GOALS                          "goals"
 RULE                           "rule"
 RULES                          "rules"
 RMA                            "rma"
 RMAS                           "rmas"
 RECONCILE                      "reconcile"
 TRUE                           "true"
 FALSE                          "false"
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
 STDIN                          "stdin"
 STDOUT                         "stdout"
 STDERR                         "stderr"
 AUXOUT                         "auxout"
 RUN                            "run"

/* events */
 STALE                          "stale"
 FRESH                          "fresh"
 CREATE                         "create"
 DELETE                         "delete"
 GOALS_SET                      "goals-set"
 AUTORUN_SET                    "autorun-set"
 SYSTEM_STATE_CHANGE            "system-state-change"
 FORMULA_FORK                   "formula-fork"
 FORMULA_REAP                   "formula-reap"
 BUILD_START                    "build-start"
 BUILD_END                      "build-end"
 RECONCILE_START                "reconcile-start"
 RECONCILE_END                  "reconcile-end"
 EXEC_FORK                      "exec-fork"
 EXEC_WAIT                      "exec-wait"
 EXEC_STDOUT                    "exec-stdout"
 EXEC_STDERR                    "exec-stderr"
 EXEC_AUXOUT                    "exec-auxout"
 RMA_START                      "rma-start"
 RMA_END                        "rma-end"
 RMA_SEARCH                     "rma-search"
 RMA_GENERATE                   "rma-generate"
 RMA_RESOLVE                    "rma-resolve"
 RMA_BIND                       "rma-bind"

/* long options */
%token
 lno_launch                     "--no-launch"
 lhelp                          "--help"
 lversion                       "--version"
 lboth                          "--both"
 lup                            "--up"
 ldown                          "--down"
 lhidden                        "--hidden"
 lno_hidden                     "--no-hidden"
 lgraph                         "--graph"
 lvisit                         "--visit"
 lids                           "--ids"
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
 lmax_depth                     "--max-depth"
 lmin_depth                     "--min-depth"
 lprimary                       "--primary"
 lno_primary                    "--no-primary"
 lrecursive                     "--recursive"
 lno_recursive                  "--no-recursive"
 lcross_module                  "--cross-module"
 lno_cross_module               "--no-cross-module"
 lcross_fstree                  "--cross-fstree"
 lno_cross_fstree               "--no-cross-fstree"
 lreset                         "--reset"
 lno_reset                      "--no-reset"

 lon_success_show_path                "--on-success-show-path"
 lon_success_show_cwd                 "--on-success-show-cwd"
 lon_success_show_command             "--on-success-show-command"
 lon_success_show_arguments           "--on-success-show-arguments"
 lon_success_show_environment         "--on-success-show-environment"
 lon_success_show_nodes               "--on-success-show-nodes"
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
 lon_error_show_nodes                 "--on-error-show-nodes"
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
 sR   "-R"
 sr   "-r"

/* these are devel only */
%token
 lsystem_config_path              "--system-config-path"
 luser_config_path                "--user-config-path"
 ldefault_filesystem_invalidation "--default-filesystem-invalidation"
 lsweeper_period_nsec             "--sweeper-period-nsec"

%%
utterance
  : cmdline
  {
    yyu_loc_final(PARSER, &@1);
  }
  ;

cmdline
  /* build is the default command */
  : primary-build-argument build-arguments
  | global-arguments command
  | global-arguments
  ;

global-argument
  : lhelp           { G_ARGS->help = true; }
  | sh              { G_ARGS->help = true; }
  | lversion        { G_ARGS->version = true; }
  | sV              { G_ARGS->version = true; }
  | sv              { G_ARGS->verbose++; }
  | svv             { G_ARGS->verbose += 2; }
  | svvv            { G_ARGS->verbose += 3; }
  | sK              { G_ARGS->kill = true; }
  | sm STR          { args_set_module_path(@2.s, @2.l); }
  | lmodule STR     { args_set_module_path(@2.s, @2.l); }
  | lno_module      { args_set_no_module(); }
  | lno_launch      { G_ARGS->no_launch = true; }
  | lno_color       { G_ARGS->no_color = true; }
  | lon_success_show_path bool                  { build_args.success.show_path = $2; }
  | lon_success_show_cwd bool                   { build_args.success.show_cwd = $2; }
  | lon_success_show_command bool               { build_args.success.show_command = $2; }
  | lon_success_show_arguments bool             { build_args.success.show_arguments = $2; }
  | lon_success_show_environment bool           { build_args.success.show_environment = $2; }
  | lon_success_show_nodes bool                 { build_args.success.show_nodes = $2; }
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
  | lon_error_show_nodes bool                   { build_args.error.show_nodes = $2; }
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
  | lsystem_config_path STR               { G_ARGS->system_config_path = @2.s; G_ARGS->system_config_path_len = @2.l; }
  | luser_config_path STR                 { G_ARGS->user_config_path = @2.s; G_ARGS->user_config_path_len = @2.l; }
  | ldefault_filesystem_invalidation STR  { G_ARGS->default_filesystem_invalidation = @2.s; G_ARGS->default_filesystem_invalidation_len = @2.l; }
  | lsweeper_period_nsec uint64           { G_ARGS->sweeper_period_nsec = @2.s; G_ARGS->sweeper_period_nsec_len = @2.l; }
  ;

global-arguments
  : global-argument-list
  | %empty
  ;

global-argument-list
  : global-argument-list global-argument
  | global-argument
  ;

command
  : ADHOC          { command_set(&adhoc_command); }          adhoc-arguments
  | AUTOBUILD      { command_set(&autobuild_command); }      build-arguments
  | BUILD          { command_set(&build_command); }          build-arguments
  | BUILD_RECORD   { command_set(&build_record_command); }   build-record-arguments
  | BUILD_RECORDS  { command_set(&build_records_command); }  build-records-arguments
  | config-command
  | CONFIGSET      { command_set(&configset_command); }      configset-arguments
  | CONFIGSETS     { command_set(&configsets_command); }     configsets-arguments
  | DESCRIBE       { command_set(&describe_command); }       describe-arguments
  | EVENTS         { command_set(&events_command); }         events-arguments
  | FORMULA        { command_set(&formula_command); }        formula-arguments
  | FORMULAS       { command_set(&formulas_command); }       formulas-arguments
  | fsname-command
  | fsnames-command
  | FSTREE         { command_set(&fstree_command); }         fstree-arguments
  | FSTREES        { command_set(&fstrees_command); }        fstrees-arguments
  | GOALS          { command_set(&goals_command); }          goals-arguments
  | INFO           { command_set(&info_command); }           info-arguments
  | kill-command
  | ls-command
  | module-command
  | MODULES        { command_set(&modules_command); }        modules-arguments
  | namespace-command
  | NAMESPACES     { command_set(&namespaces_command); }     namespaces-arguments
  | PLAN           { command_set(&plan_command); }           plan-arguments
  | plugin-command
  | PLUGINS        { command_set(&plugins_command); }        plugins-arguments
  | RECONCILE      { command_set(&reconcile_command); }      reconcile-arguments
  | rma-command
  | rmas-command
  | rule-command
  | rules-command
  | RULESET        { command_set(&ruleset_command); }        ruleset-arguments
  | RULESETS       { command_set(&rulesets_command); }       rulesets-arguments
  | BLAME          { command_set(&blame_command); }          blame-arguments
  | STATS          { command_set(&stats_command); }          stats-arguments
  | TOUCH          { command_set(&touch_command); }          touch-arguments
  | tree-command
  | VARSET         { command_set(&varset_command); }         varset-arguments
  | VARSETS        { command_set(&varsets_command); }        varsets-arguments
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
  : MODULE { command_set(&module_command); } module-arguments
  ;

module-arguments
  : global-arguments module-target global-arguments module-subcommand
  | global-arguments module-target global-arguments
  | global-arguments module-subcommand
  | global-arguments
  ;

module-target
  : BECH32 { CMD->target = @1.s; CMD->target_len = @1.l; }
  | word { CMD->target = @1.s; CMD->target_len = @1.l; }
  ;

module-subcommand
  : module-envs-command
  | module-vars-command
  | module-rules-command
  | module-rule-command
  | module-rulesets-command
  | module-ruleset-command
  | module-varsets-command
  | module-varset-command
  | module-rmas-command
  | module-rma-command
  ;

module-envs-command
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

module-vars-command
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

module-rulesets-command
  : RULESETS { command_sub_set(&module_rulesets_command); } module-rulesets-arguments
  ;

module-rulesets-arguments
  : module-rulesets-argument-list
  | %empty
  ;

module-rulesets-argument-list
  : module-rulesets-argument-list module-rulesets-argument
  | module-rulesets-argument
  ;

module-rulesets-argument
  : global-argument
  ;

module-ruleset-command
  : RULESET { command_sub_set(&module_ruleset_command); } module-ruleset-arguments
  ;

module-ruleset-arguments
  : module-ruleset-argument-list
  | %empty
  ;

module-ruleset-argument-list
  : module-ruleset-argument-list module-ruleset-argument
  | module-ruleset-argument
  ;

module-ruleset-argument
  : global-argument
  | BECH32 { CMD->ruleset_id_len = zcatw(CMD->ruleset_id, sizeof(CMD->ruleset_id), @1.s, @1.l); }
  | word { CMD->ruleset_path = @1.s; CMD->ruleset_path_len = @1.l; }
  ;

module-varsets-command
  : VARSETS { command_sub_set(&module_varsets_command); } module-varsets-arguments
  ;

module-varsets-arguments
  : module-varsets-argument-list
  | %empty
  ;

module-varsets-argument-list
  : module-varsets-argument-list module-varsets-argument
  | module-varsets-argument
  ;

module-varsets-argument
  : global-argument
  ;

module-varset-command
  : VARSET { command_sub_set(&module_varset_command); } module-varset-arguments
  ;

module-varset-arguments
  : module-varset-argument-list
  | %empty
  ;

module-varset-argument-list
  : module-varset-argument-list module-varset-argument
  | module-varset-argument
  ;

module-varset-argument
  : global-argument
  | BECH32 { CMD->varset_id_len = zcatw(CMD->varset_id, sizeof(CMD->varset_id), @1.s, @1.l); }
  | word { CMD->varset_path = @1.s; CMD->varset_path_len = @1.l; }
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

module-rule-command
  : RULE { command_sub_set(&module_rule_command); } module-rule-arguments
  ;

module-rule-arguments
  : module-rule-argument-list
  | %empty
  ;

module-rule-argument-list
  : module-rule-argument-list module-rule-argument
  | module-rule-argument
  ;

module-rule-argument
  : global-argument
  | BECH32 { CMD->rule_id_len = zcatw(CMD->rule_id, sizeof(CMD->rule_id), @1.s, @1.l); }
  | word { CMD->rule_name = @1.s; CMD->rule_name_len = @1.l; }
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

module-rma-command
  : RMA { command_sub_set(&module_rma_command); } module-rma-arguments module-rma-subcommand
  ;

module-rma-arguments
  : module-rma-argument-list
  | %empty
  ;

module-rma-argument-list
  : module-rma-argument-list module-rma-argument
  | module-rma-argument
  ;

module-rma-argument
  : global-argument
  | BECH32 { CMD->rma_id_len = zcatw(CMD->rma_id, sizeof(CMD->rma_id), @1.s, @1.l); }
  | word { CMD->rma_name = @1.s; CMD->rma_name_len = @1.l; }
  ;

module-rma-subcommand
  : module-rma-run-command
  | %empty
  ;

module-rma-run-command
  : RUN { command_sub_set(&module_rma_run_command); } module-rma-run-arguments
  ;

module-rma-run-arguments
  : module-rma-run-argument-list
  | %empty
  ;

module-rma-run-argument-list
  : module-rma-run-argument-list module-rma-run-argument
  | module-rma-run-argument
  ;

module-rma-run-argument
  : global-argument
  | lpaths
  {
    rma_run_args.show_paths = true;
  }
  | lids
  {
    rma_run_args.show_ids = true;
  }
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
  | word { tree_args.targets[tree_args.targets_len] = @1.s; tree_args.targets_lens[tree_args.targets_len++] = @1.l; }
  | BECH32 { tree_args.targets[tree_args.targets_len] = @1.s; tree_args.targets_lens[tree_args.targets_len++] = @1.l; }

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
  | lmindist uint16
  {
    tree_args.min_distance = $2;
  }
  | lmaxdist uint16
  {
    tree_args.max_distance = $2;
  }
  | lfollow
  {
    tree_args.nofollow = false;
  }
  | lno_follow
  {
    tree_args.nofollow = true;
  }
  | lhidden
  {
    tree_args.hidden = true;
  }
  | lno_hidden
  {
    tree_args.hidden = false;
  }
  | ss
  {
    tree_args.special += 1;
  }
  | sS
  {
    tree_args.special += 2;
  }
  | lspecial
  {
    tree_args.special++;
  }
  | lno_special
  {
    tree_args.special = 0;
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
  | primary-build-argument
  ;

primary-build-argument
  : build-target
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
  : build-target-name
  {
    build_args.targets[build_args.targets_len].s = @1.s;
    build_args.targets[build_args.targets_len].sl = @1.l;
    if(build_args.targets[build_args.targets_len].mode == 0) {
      build_args.targets[build_args.targets_len].mode = build_args.mode;
    }
    build_args.targets_len++;
  }
  ;

build-target-name
  : word
  | BECH32
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
  | RECORDS
  ;

visit-type
  : MODULE
  | RULESET
  | FORMULA
  | VARSET
  | CONFIGSET
  | PLUGIN
  | DIRECTORY
  | RULE
  | RMA
  ;

touch-arguments
  : touch-argument-list
  | %empty
  ;

touch-argument-list
  : touch-argument-list touch-argument
  | touch-argument
  ;

touch-argument
  : global-argument
  | word { touch_args.targets[touch_args.targets_len] = @1.s; touch_args.targets_lens[touch_args.targets_len++] = @1.l; }
  | BECH32 { touch_args.targets[touch_args.targets_len] = @1.s; touch_args.targets_lens[touch_args.targets_len++] = @1.l; }
  | lfollow
  {
    touch_args.nofollow = false;
  }
  | lno_follow
  {
    touch_args.nofollow = true;
  }
  | sR
  {
    touch_args.recursive = true;
  }
  | lrecursive
  {
    touch_args.recursive = true;
  }
  | lno_recursive
  {
    touch_args.recursive = false;
  }
  | lprimary
  {
    touch_args.primary = true;
  }
  | lno_primary
  {
    touch_args.primary = false;
  }
  | lcross_module
  {
    touch_args.cross_module = true;
  }
  | lno_cross_module
  {
    touch_args.cross_module = false;
  }
  | lcross_fstree
  {
    touch_args.cross_fstree = true;
  }
  | lno_cross_fstree
  {
    touch_args.cross_fstree = false;
  }
  | lmindist uint16
  {
    touch_args.min_distance = $2;
  }
  | lmaxdist uint16
  {
    touch_args.max_distance = $2;
  }
  | lhidden
  {
    touch_args.hidden = true;
  }
  | lno_hidden
  {
    touch_args.hidden = false;
  }
  ;

plan-arguments
  : plan-argument-list
  | %empty
  ;

plan-argument-list
  : plan-argument-list plan-argument
  | plan-argument
  ;

plan-argument
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

plugin-command
  : PLUGIN { command_set(&plugin_command); } plugin-arguments
  ;

plugin-arguments
  : global-arguments
  | plugin-target global-arguments plugin-subcommand
  ;

plugin-subcommand
  : plugin-stdin-command
  | plugin-stdout-command
  | plugin-stderr-command
  | plugin-auxout-command
  | %empty
  ;

plugin-target
  : BECH32 { CMD->id_len = zcatw(CMD->id, sizeof(CMD->id), @1.s, @1.l); }
  | word { CMD->target = @1.s; CMD->target_len = @1.l; }
  ;

plugin-stdin-command
  : STDIN { command_sub_set(&plugin_stdin_command); } plugin-stdin-arguments
  ;

plugin-stdin-arguments
  : plugin-stdin-argument-list
  | %empty
  ;

plugin-stdin-argument-list
  : plugin-stdin-argument-list plugin-stdin-argument
  | plugin-stdin-argument
  ;

plugin-stdin-argument
  : global-argument
  ;

plugin-stdout-command
  : STDOUT { command_sub_set(&plugin_stdout_command); } plugin-stdout-arguments
  ;

plugin-stdout-arguments
  : plugin-stdout-argument-list
  | %empty
  ;

plugin-stdout-argument-list
  : plugin-stdout-argument-list plugin-stdout-argument
  | plugin-stdout-argument
  ;

plugin-stdout-argument
  : global-argument
  ;

plugin-stderr-command
  : STDERR { command_sub_set(&plugin_stderr_command); } plugin-stderr-arguments
  ;

plugin-stderr-arguments
  : plugin-stderr-argument-list
  | %empty
  ;

plugin-stderr-argument-list
  : plugin-stderr-argument-list plugin-stderr-argument
  | plugin-stderr-argument
  ;

plugin-stderr-argument
  : global-argument
  ;

plugin-auxout-command
  : AUXOUT { command_sub_set(&plugin_auxout_command); } plugin-auxout-arguments
  ;

plugin-auxout-arguments
  : plugin-auxout-argument-list
  | %empty
  ;

plugin-auxout-argument-list
  : plugin-auxout-argument-list plugin-auxout-argument
  | plugin-auxout-argument
  ;

plugin-auxout-argument
  : global-argument
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
  | BECH32 { CMD->id_len = zcatw(CMD->id, sizeof(CMD->id), @1.s, @1.l); }
  | word { CMD->special_target = @1.s; CMD->special_target_len = @1.l; }
  ;

build-records-arguments
  : build-records-argument-list
  | %empty
  ;

build-records-argument-list
  : build-records-argument-list build-records-argument
  | build-records-argument
  ;

build-records-argument
  : global-argument
  ;

build-record-arguments
  : build-record-argument-list
  ;

build-record-argument-list
  : build-record-argument-list build-record-argument
  | build-record-argument
  ;

build-record-argument
  : global-argument
  | BECH32 { CMD->build_record_id_len = zcatw(CMD->build_record_id, sizeof(CMD->build_record_id), @1.s, @1.l); }
  | uint16 { CMD->build_record_order = $1; }
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
  | BECH32 { CMD->ruleset_id_len = zcatw(CMD->ruleset_id, sizeof(CMD->ruleset_id), @1.s, @1.l); }
  | word { CMD->ruleset_path = @1.s; CMD->ruleset_path_len = @1.l; }
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
  | BECH32 { CMD->id_len = zcatw(CMD->id, sizeof(CMD->id), @1.s, @1.l); }
  | word { CMD->special_target = @1.s; CMD->special_target_len = @1.l; }
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
  | word  { CMD->targets[CMD->targets_len] = @1.s; CMD->targets_lens[CMD->targets_len++] = @1.l; }
  | BECH32 { CMD->targets[CMD->targets_len] = @1.s; CMD->targets_lens[CMD->targets_len++] = @1.l; }
  | sr
  {
    CMD->reset = true;
  }
  | lreset
  {
    CMD->reset = true;
  }
  | lno_reset
  {
    CMD->reset = false;
  }
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
  | lhidden
  {
    CMD->hidden = true;
  }
  | lno_hidden
  {
    CMD->hidden = false;
  }
  | lfollow
  {
    CMD->nofollow = false;
  }
  | lno_follow
  {
    CMD->nofollow = true;
  }
  ;

adhoc-arguments
  : adhoc-arguments adhoc-argument
  | adhoc-argument
  ;

adhoc-argument
  : global-argument
  | STR { adhoc_args.args[adhoc_args.args_len] = @1.s; adhoc_args.args_lens[adhoc_args.args_len++] = @1.l; }
  ;

blame-arguments
  : global-arguments word global-arguments blame-detail global-arguments
  {
    CMD->target = @2.s; CMD->target_len = @2.l;
    CMD->blame_detail = @4.s;
    CMD->blame_detail_len = @4.l;
  }
  | global-arguments word global-arguments
  {
    CMD->target = @2.s; CMD->target_len = @2.l;
  }
  ;

blame-detail
  : STDIN
  | STDOUT
  | STDERR
  | AUXOUT
  | ARGS
  | ENVS
  | D_FILE
  | CWD
  ;

describe-arguments
  : describe-arguments describe-argument
  | describe-argument
  ;

describe-argument
  : global-argument
  | word { CMD->targets[CMD->targets_len] = @1.s; CMD->targets_lens[CMD->targets_len++] = @1.l; }
  | BECH32 { CMD->targets[CMD->targets_len] = @1.s; CMD->targets_lens[CMD->targets_len++] = @1.l; }
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
  | lfollow
  {
    CMD->nofollow = false;
  }
  | lno_follow
  {
    CMD->nofollow = true;
  }
  | lhidden
  {
    CMD->hidden = true;
  }
  | lno_hidden
  {
    CMD->hidden = false;
  }
  ;

namespaces-arguments
  : global-arguments
  ;

namespace-command
  : NAMESPACE { command_set(&namespace_command); } global-arguments namespace-arguments
  ;

namespace-arguments
  : namespace-target global-arguments namespace-subcommand
  ;

namespace-target
  : BECH32 { CMD->namespace_id_len = zcatw(CMD->namespace_id, sizeof(CMD->namespace_id), @1.s, @1.l); }
  | word { CMD->namespace_name = @1.s; CMD->namespace_name_len = @1.l; }
  ;

namespace-subcommand
  : namespace-fsnames-command
  | namespace-fsname-command
  | %empty
  ;

namespace-fsnames-command
  : FSNAMES { command_sub_set(&namespace_fsnames_command); } fsnames-arguments
  ;

fsname-command
  : FSNAME { command_set(&fsname_command); } fsname-arguments
  ;

fsname-arguments
  : fsname-arguments fsname-argument
  | fsname-argument
  ;

fsnames-command
  : FSNAMES { command_set(&fsnames_command); } fsnames-arguments
  ;

fsnames-arguments
  : global-arguments
  ;

namespace-fsname-command
  : FSNAME { command_sub_set(&namespace_fsname_command); } namespace-fsname-arguments
  ;

namespace-fsname-arguments
  : namespace-fsname-arguments namespace-fsname-argument
  | namespace-fsname-argument
  ;

namespace-fsname-argument
  : global-argument
  | BECH32 { CMD->fsname_id_len = zcatw(CMD->fsname_id, sizeof(CMD->fsname_id), @1.s, @1.l); }
  | word { CMD->fsname_name = @1.s; CMD->fsname_name_len = @1.l; }
  ;

fsname-argument
  : global-argument
  | BECH32 { CMD->fsname_id_len = zcatw(CMD->fsname_id, sizeof(CMD->fsname_id), @1.s, @1.l); }
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
  | BECH32 { CMD->fstree_id_len = zcatw(CMD->fstree_id, sizeof(CMD->fstree_id), @1.s, @1.l); }
  | word { CMD->target = @1.s; CMD->target_len = @1.l; }
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
  | BECH32 { CMD->id_len = zcatw(CMD->id, sizeof(CMD->id), @1.s, @1.l); }
  | word { CMD->special_target = @1.s; CMD->special_target_len = @1.l; }
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
  | BECH32 { CMD->rule_id_len = zcatw(CMD->rule_id, sizeof(CMD->rule_id), @1.s, @1.l); }
  | word { CMD->rule_name = @1.s; CMD->rule_name_len = @1.l; }
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
  : RMA { command_set(&rma_command); } rma-arguments rma-subcommand
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
  | BECH32 { CMD->rma_id_len = zcatw(CMD->rma_id, sizeof(CMD->rma_id), @1.s, @1.l); }
  | word { CMD->rma_name = @1.s; CMD->rma_name_len = @1.l; }
  ;

rma-subcommand
  : rma-run-command
  | %empty
  ;

rma-run-command
  : RUN { command_sub_set(&rma_run_command); } rma-run-arguments
  ;

rma-run-arguments
  : rma-run-argument-list
  | %empty
  ;

rma-run-argument-list
  : rma-run-argument-list rma-run-argument
  | rma-run-argument
  ;

rma-run-argument
  : global-argument
  | lpaths
  {
    rma_run_args.show_paths = true;
  }
  | lids
  {
    rma_run_args.show_ids = true;
  }
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
  | GOALS_SET           { events_args.event_mask |= BAMIPC_EVENT_GOALS_SET; }
  | AUTORUN_SET         { events_args.event_mask |= BAMIPC_EVENT_AUTORUN_SET; }
  | SYSTEM_STATE_CHANGE { events_args.event_mask |= BAMIPC_EVENT_SYSTEM_STATE_CHANGE; }
  | FORMULA_FORK        { events_args.event_mask |= BAMIPC_EVENT_FORMULA_FORK; }
  | FORMULA_REAP        { events_args.event_mask |= BAMIPC_EVENT_FORMULA_REAP; }
  | BUILD_START         { events_args.event_mask |= BAMIPC_EVENT_BUILD_START; }
  | BUILD_END           { events_args.event_mask |= BAMIPC_EVENT_BUILD_END; }
  | RECONCILE_START     { events_args.event_mask |= BAMIPC_EVENT_RECONCILE_START; }
  | RECONCILE_END       { events_args.event_mask |= BAMIPC_EVENT_RECONCILE_END; }
  | EXEC_FORK           { events_args.event_mask |= BAMIPC_EVENT_FORMULA_FORK_BIN; }
  | EXEC_WAIT           { events_args.event_mask |= BAMIPC_EVENT_FORMULA_REAP_BIN; }
  | EXEC_STDOUT         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_STDOUT_BIN; }
  | EXEC_STDERR         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_STDERR_BIN; }
  | EXEC_AUXOUT         { events_args.event_mask |= BAMIPC_EVENT_FORMULA_AUXOUT_BIN; }
  | RMA_START           { events_args.event_mask |= BAMIPC_EVENT_RMA_START; }
  | RMA_END             { events_args.event_mask |= BAMIPC_EVENT_RMA_END; }
  | RMA_SEARCH          { events_args.event_mask |= BAMIPC_EVENT_RMA_SEARCH; }
  | RMA_GENERATE        { events_args.event_mask |= BAMIPC_EVENT_RMA_GENERATE; }
  | RMA_RESOLVE         { events_args.event_mask |= BAMIPC_EVENT_RMA_RESOLVE; }
  | RMA_BIND            { events_args.event_mask |= BAMIPC_EVENT_RMA_BIND; }
  ;

word
  : STR
  | uint64
  | bool
 /* why are there no rollup tokens here ? see comments in args.l */
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

lmindist
  : lmin_distance
  | lmin_depth
  ;

lmaxdist
  : lmax_distance
  | lmax_depth
  ;
