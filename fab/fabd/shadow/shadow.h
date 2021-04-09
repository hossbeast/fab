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

#ifndef FABD_SHADOW_H
#define FABD_SHADOW_H

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
xapi shadow_setup(void);

/* module cleanup */
xapi shadow_cleanup(void);

/* attaach a module to the shadow fs */
xapi shadow_module_init(
    struct module * restrict mod
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

xapi shadow_graft_imports(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

xapi shadow_graft_requires(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

xapi shadow_graft_uses(
    struct module * restrict mod
  , struct fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , struct fsedge ** restrict ep
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3, 6)));

#endif
