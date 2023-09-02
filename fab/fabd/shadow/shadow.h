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
extern struct fsent * g_shadow;                 /* root of the shadow fs */
extern struct fsent * g_shadow_module;          /* the node at //module */
extern struct fsent * g_shadow_modules;         /* the node at //modules */
extern struct fsent * g_shadow_targets;         /* the node at //targets */

/* module initialization */
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
  __attribute__((nonnull(1, 2, 3, 5)));

void shadow_graft_requires(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

void shadow_graft_rules(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

void shadow_graft_var(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

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
  , /* 4 */ struct fsent * restrict ref
  , /* 5 */ struct fsedge ** restrict ep
  , /* 6 */ struct fsent ** restrict pref
  , /* 7 */ struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 4, 7)));
