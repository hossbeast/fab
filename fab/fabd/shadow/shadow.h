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
struct graph_invalidation_context;
struct module;

// globally scoped fsents
extern struct fsent * g_shadow;                 /* root of the shadow fs */
extern struct fsent * g_shadow_module;          /* the node at //module */
extern struct fsent * g_shadow_modules;         /* the node at //modules */

/* module initialization */
void shadow_setup(void);

/* module cleanup */
void shadow_cleanup(void);

/* attaach a module to the shadow fs */
void shadow_module_init(
    struct module * restrict mod
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void shadow_graft_imports(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

void shadow_graft_requires(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

void shadow_graft_uses(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));
