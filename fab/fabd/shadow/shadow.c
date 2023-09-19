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

#include <stdio.h>

#include "types.h"

#include "shadow.h"
#include "filesystem.internal.h"
#include "fsedge.h"
#include "fsent.h"
#include "module.h"

fsent * g_shadow;
fsent * g_shadow_module;
fsent * g_shadow_modules;

static void shadow_root_init()
{
  fsent * n = 0;

  fsent_create(&n, VERTEX_SHADOW_DIR, 0, MMS("(shadow)"));

  n->fst = &fstree_shadow;
  n->fs = n->fst->fs;

  g_shadow = n;
}

static void shadow_link(
    fsent * restrict parent
  , module * restrict mod
  , const char * restrict name
  , uint16_t namel
  , fsent * restrict ref
  , fsedge ** restrict nep
  , fsent ** restrict displaced
  , graph_invalidation_context * restrict invalidation
)
{
  fsent *n;
  moria_connect_context ctx;
  fsedge *fse;
  moria_edge *e;
  int r;

  fsent_create(&n, VERTEX_SHADOW_LINK, 0, name, namel);
  n->vertex.ref = &ref->vertex;

  r = moria_preconnect(&ctx, &g_graph, &parent->vertex, &n->vertex, EDGE_FSTREE, &e);

  if(r == MORIA_EDGEDOWN)
  {
    *nep = containerof(e, fsedge, edge);

    /* existing edge with the same referent */
    if(e->B->ref == &ref->vertex)
    {
      *displaced = 0;
      fsent_release(n);
    }
    else
    {
      /* the edge is still in use, but no longer connected to this vertex */
      *displaced = containerof(e->B, fsent, vertex);
      e->B->up_identity = 0;

      /* this is valid because the old/new B vertices have the same label */
      e->B = &n->vertex;
    }
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_NOEDGE);
    fsedge_alloc(&fse, &g_graph);
    graph_connect(&ctx, &parent->vertex, &n->vertex, &fse->edge, EDGE_FSTREE);
    *nep = fse;
  }
}

static void shadow_create(
    fsent * restrict parent
  , fsent ** restrict np
  , module * restrict mod
  , const char * restrict name
  , uint16_t namel
  , vertex_kind kind
  , graph_invalidation_context * restrict invalidation
)
{
  fsent * n;

  fsent_create(&n, kind, 0, name, namel);

  n->fst = &fstree_shadow;
  n->fs = n->fst->fs;
  n->mod = mod;

  fsedge_connect(parent, n, invalidation);

  if(np) {
    *np = n;
  }
}

//
// public
//

void shadow_setup()
{
  graph_invalidation_context invalidation = { 0 };

  shadow_root_init();

  graph_invalidation_begin(&invalidation);
  shadow_create(g_shadow, &g_shadow_module, 0, MMS("module"), VERTEX_SHADOW_MODULE, &invalidation);
  shadow_create(g_shadow, &g_shadow_modules, 0, MMS("modules"), VERTEX_SHADOW_MODULES, &invalidation);

  graph_invalidation_end(&invalidation);
}

void shadow_cleanup()
{
}

void shadow_module_init(struct module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  shadow_create(g_shadow_modules, &mod->shadow, mod, mod->id, 16, VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_imports, mod, MMS("imports"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_requires, mod, MMS("requires"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_uses, mod, MMS("uses"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_targets, mod, MMS("targets"), VERTEX_SHADOW_FILE, invalidation);

  /* //module/this/fs -> module directory fsent */
  shadow_link(mod->shadow, mod, MMS("fs"), mod->dir_node, &mod->shadow_fs, 0, invalidation);
}

void shadow_graft_requires(struct module * restrict mod, fsent * restrict ref, const char * restrict as, uint16_t asl, fsedge ** restrict nep, graph_invalidation_context * restrict invalidation)
{
  fsent *displaced = 0;

  shadow_link(mod->shadow_requires, mod, as, asl, ref, nep, &displaced, invalidation);

  if(displaced)
  {
    printf("require '%.*s' displaces earlier name", (int)asl, as);

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}

void shadow_graft_imports(struct module * restrict mod, fsent * restrict ref, const char * restrict as, uint16_t asl, fsedge ** restrict nep, graph_invalidation_context * restrict invalidation)
{
  fsent *n, *displaced = 0;

  shadow_link(mod->shadow_imports, mod, as, asl, ref, nep, &displaced, invalidation);

  if(displaced)
  {
    printf("import '%.*s' displaces earlier name", (int)asl, as);

    /* erase the referent */
    n = containerof(displaced->vertex.ref, fsent, vertex);
    RUNTIME_ASSERT(n->import_scope_edge);
    n->import_scope_edge = 0;

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}

void shadow_graft_uses(struct module * restrict mod, fsent * restrict ref, const char * restrict as, uint16_t asl, fsedge ** restrict nep, graph_invalidation_context * restrict invalidation)
{
  fsent *n, *displaced = 0;

  shadow_link(mod->shadow_uses, mod, as, asl, ref, nep, &displaced, invalidation);

  if(displaced)
  {
    printf("uses '%.*s' displaces earlier name", (int)asl, as);

    /* erase the referent */
    n = containerof(displaced->vertex.ref, fsent, vertex);
    RUNTIME_ASSERT(n->use_scope_edge);
    n->use_scope_edge = 0;

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}
