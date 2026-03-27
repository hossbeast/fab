/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#if 0
shadow filesystem layout

//module           the context module
//modules/<id>     virtual directory with an entry for each loaded module
//targets          default build targets
//bm/sys-*         subsystem nodes
//bm/*             lists of specials
#endif

struct fsedge;
struct fsent;
struct module;
enum vertex_kind;
struct graph_invalidation_context;
struct moria_connect_context;

// globally scoped fsents
extern struct fsent *g_shadow;                        /* shadow fs root */
extern struct fsent *g_shadow_bm;                     /* shadow bm dir */

/* lists of specials */
extern struct fsent *g_shadow_modules;                /* //bm/modules */
extern struct fsent *g_shadow_rulesets;               /* //bm/rulesets */
extern struct fsent *g_shadow_formulas;               /* //bm/formulas */
extern struct fsent *g_shadow_varsets;                /* //bm/varsets */
extern struct fsent *g_shadow_configsets;             /* //bm/configsets */
extern struct fsent *g_shadow_plugins;                /* //bm/plugins */
extern struct fsent *g_shadow_plugins_byname;         /* //bm/plugins-by/name */
extern struct fsent *g_shadow_fstrees;                /* //bm/fstrees */
extern struct fsent *g_shadow_clients;                /* //bm/clients */
extern struct fsent *g_shadow_namespaces;             /* //bm/namespaces */
extern struct fsent *g_shadow_namespaces_byname;      /* //bm/namespaces-by/name */
extern struct fsent *g_shadow_fsnames;                /* //bm/fsnames */
extern struct fsent *g_shadow_build_records;          /* //bm/build-records */
extern struct fsent *g_shadow_build_records_byorder;  /* //bm/build-records-by/order */
extern struct fsent *g_shadow_formula_records;        /* //bm/formula-records */

/* rules */
extern struct fsent *g_shadow_rules;                  /* //bm/rules */
extern struct fsent *g_shadow_rules_byname;           /* //bm/rules-by/name/ */
extern struct fsent *g_shadow_rmas;                   /* //bm/rmas */
extern struct fsent *g_shadow_rmas_byname;            /* //bm/rmas-by/name/ */

/* other node lists */
extern struct fsent *g_shadow_bpes;                   /* //bm/bpes */

/* magic files */
extern struct fsent *g_shadow_targets;                /* //targets */

/* module initialization */
void shadow_root_setup(void);
void shadow_setup(void);

/* module cleanup */
void shadow_cleanup(void);

void shadow_graft_bind(
    struct module * restrict mod
  , struct fsent * restrict ref
  , struct fsent * restrict attachment
  , struct fsent ** restrict target
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3)));

void shadow_create(
    struct fsent * restrict parent
  , struct fsent ** restrict np
  , const char * restrict name
  , uint16_t namel
  , enum vertex_kind kind
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 3, 6)));

struct moria_vertex *shadow_link(
    /* 1 */ struct moria_connect_context * restrict ctx
  , /* 2 */ struct fsent * restrict parent
  , /* 3 */ const char * restrict name
  , /* 4 */ uint16_t namel
)
  __attribute__((nonnull(2, 3)));

void shadow_mkref(
    /* 1 */ struct fsent * restrict parent
  , /* 2 */ const char * restrict name
  , /* 3 */ uint16_t namel
  , /* 4 */ struct moria_vertex * restrict ref
)
  __attribute__((nonnull(1, 2, 4)));

void shadow_mkref_new(
    /* 1 */ struct fsent * restrict parent
  , /* 2 */ const char * restrict name
  , /* 3 */ uint16_t namel
  , /* 4 */ struct moria_vertex * restrict ref
)
  __attribute__((nonnull(1, 2, 4)));

void shadow_link_noref(
    /* 1 */ struct fsent * restrict parent
  , /* 2 */ const char * restrict name
  , /* 3 */ uint16_t namel
  , /* 4 */ struct moria_vertex * restrict ref
  , /* 5 */ struct fsent ** restrict linkp
)
  __attribute__((nonnull));

void fsent_shadow_link_describe(struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
