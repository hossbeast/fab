/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "descriptor.h"
#include "common/attrs.h"
#include "stats.h"

struct events_context;
struct moria_vertex;
struct bacon_writer;
struct channel;
struct request_handler;

#define SYSTEM_STATE_TABLE                                                    \
  DEF(SYSTEM_STATE_BOOTSTRAP                  , "bootstrap"             , 1)  \
  DEF(SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE    , "config-reconcile"      , 2)  \
  DEF(SYSTEM_STATE_WALKER_RECONCILE           , "walker-reconcile"      , 3)  \
  DEF(SYSTEM_STATE_NAMESPACE_RECONCILE        , "namespace-reconcile"   , 4)  \
  DEF(SYSTEM_STATE_VAR_SYSTEM_RECONCILE       , "var-reconcile"         , 5)  \
  DEF(SYSTEM_STATE_MODULE_SYSTEM_RECONCILE    , "module-reconcile"      , 6)  \
  DEF(SYSTEM_STATE_RULE_SYSTEM_RECONCILE      , "rule-reconcile"        , 7)  \
  DEF(SYSTEM_STATE_BPE_SYSTEM_RECONCILE       , "bpe-reconcile"         , 8)  \
  DEF(SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE   , "formula-reconcile"     , 9)  \
  DEF(SYSTEM_STATE_FSTREE_RECONCILE           , "fstree-reconcile"      , 10) \
  DEF(SYSTEM_STATE_FILESYSTEM_RECONCILE       , "filesystem-reconcile"  , 11) \
  DEF(SYSTEM_STATE_PLUGIN_SYSTEM_RECONCILE    , "plugin-reconcile"      , 12) \
  DEF(SYSTEM_STATE_FSENT_RECONCILE            , "fsent-reconcile"       , 13) \
  DEF(SYSTEM_STATE_READY                      , "ready"                 , 14) \
  DEF(SYSTEM_STATE_BUILDING                   , "building"              , 15) \
  DEF(SYSTEM_STATE_OK                         , "ok"                    , 16) /* build succeeded */ \
  DEF(SYSTEM_STATE_ERR                        , "err"                   , 17) /* build failed */    \

enum system_state {
#undef DEF
#define DEF(x, s, v) x = v,
SYSTEM_STATE_TABLE
};

/* object counts */
typedef union stats_system_objects {
  struct {
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
    uint32_t clients;
    uint32_t build_records;
    uint32_t formula_records;
    uint32_t details;

    /* edges */
    uint32_t fsedges;
    uint32_t dir_rma_edges;
    uint32_t module_edges;
    uint32_t meta_edges;
    uint32_t special_edges;
    uint32_t dependency_edges;
    uint32_t ref_edges;
    uint32_t ns_edges;
  };
  stats_record record;
} stats_system_objects;

/* instantaneous counts */
typedef union stats_system_states {
  struct {
    /* instantaneous counts */
    uint8_t system_error;
    uint8_t autorun;
  };
  stats_record record;
} stats_system_states;

/* event counters */
typedef union stats_system_events {
  struct {
    uint16_t system_state_bootstrap;              /* system-state set to bootstrap */
    uint16_t system_state_config_reconcile;       /* system-state set to config-reconcile */
    uint16_t system_state_walker_reconcile;       /* system-state set to walker-reconcile */
    uint16_t system_state_namespace_reconcile;    /* system-state set to namespace-reconcile */
    uint16_t system_state_var_reconcile;          /* system-state set to var-reconcile */
    uint16_t system_state_module_reconcile;       /* system-state set to module-reconcile */
    uint16_t system_state_rule_reconcile;         /* system-state set to rule-reconcile */
    uint16_t system_state_bpe_reconcile;          /* system-state set to bpe-reconcile */
    uint16_t system_state_formula_reconcile;      /* system-state set to formula-reconcile */
    uint16_t system_state_fstree_reconcile;       /* system-state set to fstree-reconcile */
    uint16_t system_state_filesystem_reconcile;   /* system-state set to filesystem-reconcile */
    uint16_t system_state_plugin_reconcile;       /* system-state set to plugin-reconcile */
    uint16_t system_state_fsent_reconcile;        /* system-state set to fsent-reconcile */
    uint16_t system_state_building;               /* system-state set to building */
    uint16_t system_state_ready;                  /* system-state set to ready */
    uint16_t system_state_ok;                     /* system-state set to ok */
    uint16_t system_state_err;                    /* system-state set to err */

    uint16_t ruleset_parse_ok;        /* ruleset file parsed successfully */
    uint16_t ruleset_parse_err;       /* ruleset file parse failed */
    uint16_t module_parse_ok;         /* module parsed successfully */
    uint16_t module_parse_err;        /* module parse failed */
    uint16_t module_parse_build_ok;   /* module parsed successfully */
    uint16_t module_parse_build_err;  /* module parse failed */
    uint16_t module_parse_anchor_ok;  /* module parsed successfully */
    uint16_t module_parse_anchor_err; /* module parse failed */
    uint16_t module_reloaded;         /* module is reloaded */
    uint16_t varset_parse_ok;         /* var.bam parsed successfully */
    uint16_t varset_parse_err;        /* var.bam parse failed */
    uint16_t configset_parse_ok;      /* config.bam parsed successfully */
    uint16_t configset_parse_err;     /* config.bam parse failed */
    uint16_t formula_parse_ok;        /* formula parsed successfully */
    uint16_t formula_parse_err;       /* formula parse failed */
    uint32_t vertex_stale;            /* vertex marked as stale */
    uint32_t vertex_fresh;            /* vertex marked as fresh */

    uint32_t rule_resolve_ok;         /* rule */
    uint32_t rule_resolve_err;        /* rule */
    uint32_t rule_run;                /* rule was run */
  };

  stats_record record;
} stats_system_events;

extern union stats_system_objects system_stats_objects;
extern descriptor_type descriptor_stats_system_objects;

extern union stats_system_states system_stats_states;
extern descriptor_type descriptor_stats_system_states;

extern union stats_system_events system_stats_events;
extern descriptor_type descriptor_stats_system_events;

extern attrs16 system_state_attrs;

extern enum system_state system_state;
extern bool system_has_error;

void system_state_change(enum system_state state);

struct bamipc_message *system_error_event(struct events_context * restrict ctx)
  __attribute__((nonnull));

void system_error(int code, void (*build)(struct bacon_writer * restrict builder_writer, void *ctx), const void *ctx)
  __attribute__((nonnull(2)));

void system_stats_show(void);
void system_stats_show_fd(int fd);

void system_sys_stats_build(const struct moria_vertex * restrict v, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void system_state_event(struct request_handler * restrict handler);

void system_setup(void);

void system_sys_mappings(const struct moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void system_sys_tell(const struct moria_vertex * restrict v, struct request_handler * restrict handler)
  __attribute__((nonnull));

void system_sys_describe(const struct moria_vertex * restrict, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void system_sys_id(const struct moria_vertex * restrict, const char ** restrict, uint16_t * restrict)
  __attribute__((nonnull));
