/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
shadow filesystem layout

//module           the context module

//modules/<hash>   virtual directory with an entry for each loaded module

 module/imports/  virtual directory with an entry for each imported directory
 module/targets   virtual file for module targets
 module/scope/    generate patterns start here
 module/fs/       -> module directory node - match patterns start here

*/

struct fsedge;
struct fsent;
struct module;
enum vertex_kind;
struct graph_invalidation_context;

// globally scoped fsents
extern struct fsent *g_shadow;                 /* shadow fs root */

/* lists of specials */
extern struct fsent *g_shadow_modules;         /* //modules */
extern struct fsent *g_shadow_rulesets;        /* //rulesets */
extern struct fsent *g_shadow_formulas;        /* //formulas */
extern struct fsent *g_shadow_varsets;         /* //varsets */
extern struct fsent *g_shadow_configsets;      /* //configsets */
extern struct fsent *g_shadow_plugins;         /* //plugins */
extern struct fsent *g_shadow_fstrees;         /* //fstrees */
extern struct fsent *g_shadow_fsnames;         /* //fsnames */
extern struct fsent *g_shadow_fsnames_by;      /* //fsnames-by */
extern struct fsent *g_shadow_fsnames_byname;  /* //fsnames-by/name */

/* rules */
extern struct fsent *g_shadow_rules;           /* //rules */
extern struct fsent *g_shadow_rules_by;        /* //rules-by/ */
extern struct fsent *g_shadow_rules_byname;    /* //rules-by/name/ */
extern struct fsent *g_shadow_rmas;            /* //rmas */
extern struct fsent *g_shadow_rmas_by;         /* //rmas-by */
extern struct fsent *g_shadow_rmas_byname;     /* //rmas-by/name/ */

/* other node lists */
extern struct fsent *g_shadow_bpes;            /* //bpes */

/* magic files */
extern struct fsent *g_shadow_targets;         /* //targets */

/* module initialization */
void shadow_setup(void);

/* module cleanup */
void shadow_cleanup(void);

void shadow_graft_bind(
    struct module * restrict mod
  , struct fsent * restrict ref
  , struct fsent * restrict attachment
  , struct fsent ** restrict target
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

void shadow_link(
    /* 1 */ struct fsent * restrict parent
  , /* 2 */ const char * restrict name
  , /* 3 */ uint16_t namel
  , /* 4 */ struct moria_vertex * restrict ref
  , /* 5 */ struct fsedge ** restrict ep
)
  __attribute__((nonnull(1, 2, 4)));
