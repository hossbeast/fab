/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "descriptor.h"
#include "common/attrs.h"
#include "stats.h"

struct events_context;
struct event_bacon;
struct moria_vertex;
struct bacon_writer;
struct channel;

#define SYSTEM_STATE_TABLE                                                    \
  DEF(SYSTEM_STATE_BOOTSTRAP                  , "bootstrap"             , 1)  \
  DEF(SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE    , "config-reconcile"      , 2)  \
  DEF(SYSTEM_STATE_WALKER_RECONCILE           , "walker-reconcile"      , 3)  \
  DEF(SYSTEM_STATE_VAR_SYSTEM_RECONCILE       , "var-reconcile"         , 4)  \
  DEF(SYSTEM_STATE_MODULE_SYSTEM_RECONCILE    , "module-reconcile"      , 5)  \
  DEF(SYSTEM_STATE_RULE_SYSTEM_RECONCILE      , "rule-reconcile"        , 6)  \
  DEF(SYSTEM_STATE_BPE_SYSTEM_RECONCILE       , "bpe-reconcile"         , 7)  \
  DEF(SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE   , "formula-reconcile"     , 8)  \
  DEF(SYSTEM_STATE_FILESYSTEM_RECONCILE       , "filesystem-reconcile"  , 9)  \
  DEF(SYSTEM_STATE_PLUGIN_SYSTEM_RECONCILE    , "plugin-reconcile"      , 10) \
  DEF(SYSTEM_STATE_FSENT_RECONCILE            , "fsent-reconcile"       , 11) \
  DEF(SYSTEM_STATE_BUILDING                   , "building"              , 12) \
  DEF(SYSTEM_STATE_OK                         , "ok"                    , 13) \

enum system_state {
#undef DEF
#define DEF(x, s, v) x = v,
SYSTEM_STATE_TABLE
};

typedef union stats_system {
  struct {
      uint8_t system_error;
      uint8_t autorun;

      /* object counts */
      uint32_t modules;            /* modules */
      uint32_t rules;              /* rules */
      uint32_t fsents;             /* fsents */
      uint32_t rulesets;           /* rules.bam files */
      uint32_t varsets;            /* var.bam files */
      uint32_t configsets;         /* config.bam files */
      uint32_t formulas;           /* .bam formula files */
      uint32_t namespaces;
      uint32_t fsnames;
      uint32_t bpes;
      uint32_t rmas;
      uint32_t plugins;
      uint32_t fstrees;

      uint16_t system_state_bootstrap;              /* system-state set to bootstrap */
      uint16_t system_state_config_reconcile;       /* system-state set to config-reconcile */
      uint16_t system_state_walker_reconcile;       /* system-state set to walker-reconcile */
      uint16_t system_state_var_reconcile;          /* system-state set to var-reconcile */
      uint16_t system_state_module_reconcile;       /* system-state set to module-reconcile */
      uint16_t system_state_rule_reconcile;         /* system-state set to rule-reconcile */
      uint16_t system_state_bpe_reconcile;          /* system-state set to bpe-reconcile */
      uint16_t system_state_formula_reconcile;      /* system-state set to formula-reconcile */
      uint16_t system_state_filesystem_reconcile;   /* system-state set to filesystem-reconcile */
      uint16_t system_state_plugin_reconcile;       /* system-state set to plugin-reconcile */
      uint16_t system_state_fsent_reconcile;        /* system-state set to fsent-reconcile */
      uint16_t system_state_building;               /* system-state set to building */
      uint16_t system_state_ok;                     /* system-state set to ok */

      uint16_t ruleset_parse;           /* ruleset file parse attempt */
      uint16_t ruleset_parse_ok;        /* ruleset file parsed successfully */
      uint16_t module_parse;            /* module parse attempt */
      uint16_t module_parse_ok;         /* module parsed successfully */
      uint16_t module_reloaded;         /* module is reloaded */
      uint16_t varset_parse;            /* var.bam parse attempt */
      uint16_t varset_parse_ok;         /* var.bam parsed */
      uint16_t configset_parse;         /* config.bam parse attempt */
      uint16_t configset_parse_ok;      /* config.bam parsed */
      uint16_t formula_parse;           /* formula parse attempt */
      uint16_t formula_parse_ok;        /* formula parsed successfully */
      uint32_t vertex_stale;            /* vertex marked as stale */
      uint32_t vertex_fresh;            /* vertex marked as fresh */

      uint32_t rule_run;                /* rule was run */
      uint32_t rule_run_ok;             /* rule was run successfully */

      /* devel */
      uint32_t rcu_synchronize_nobody;
      uint32_t rcu_synchronize_wait;
      uint32_t rcu_quiesce_nograce;
      uint32_t rcu_quiesce_notlast;
      uint32_t rcu_quiesce_wake;
  };

  stats_record record;
} stats_system;

extern union stats_system system_stats;
extern struct moria_vertex *g_system;
extern descriptor_type descriptor_stats_system;

extern attrs16 system_state_attrs;

extern enum system_state system_state;
extern bool system_has_error;

void system_state_change(enum system_state state);

struct event_bacon *system_error_event(struct events_context * restrict ctx)
  __attribute__((nonnull));

void system_error(int code, void (*build)(struct bacon_writer * restrict builder_writer, void *ctx), const void *ctx)
  __attribute__((nonnull(2)));

void system_stats_build(bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void system_state_event(void);

void system_setup(void);

void system_mappings(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void system_tell(struct channel * restrict channel)
  __attribute__((nonnull));

void system_describe(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t system_id_cat(char * dst, size_t sz)
  __attribute__((nonnull));
