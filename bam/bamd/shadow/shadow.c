/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "types.h"
#include "narrator.h"
#include "common/attrs.h"
#include "moria/connect.h"

#include "shadow.h"
#include "filesystem.internal.h"
#include "fsedge.h"
#include "fsent.h"
#include "module.h"
#include "vertex.h"
#include "vertex/probes.h"

#include "zcat.h"

fsent *g_shadow;
fsent *g_shadow_module;
fsent *g_shadow_modules;
fsent *g_shadow_targets;

void shadow_link(
    fsent * restrict parent
  , const char * restrict name
  , uint16_t namel
  , fsent * restrict ref
  , fsedge ** restrict ep
  , fsent ** restrict pref
  , graph_invalidation_context * restrict invalidation
)
{
  fsent *n;
  moria_connect_context ctx;
  fsedge *fse;
  moria_edge *e;
  int r;

  fsent_create(&n, VERTEX_SHADOW_LINK, VERTEX_OK, name, namel);
  n->vertex.ref = &ref->vertex;
  n->vertex.attrs |= MORIA_VERTEX_LINK;

  if(pref) {
    *pref = 0;
  }

  r = moria_preconnect(&ctx, &g_graph, &n->vertex, &parent->vertex, EDGE_FSTREE, &e);
  if(r == MORIA_EDGEDOWN)
  {
    RUNTIME_ASSERT((e->B->attrs & VERTEX_TYPE_OPT) == VERTEX_SHADOW_LINK);

    if(ep) {
      *ep = containerof(e, typeof(**ep), edge);
    }

    /* existing edge with this label */
    fsent_release(n);

    /* overwrite the referent */
    if(e->B->ref != &ref->vertex) {
      if(pref) {
        *pref = containerof(e->B->ref, fsent, vertex);
      }
    }
    e->B->ref = &ref->vertex;
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_NOEDGE);
    fsedge_alloc(&fse);
    moria_connect(&ctx, &g_graph, &fse->edge, &n->vertex, &parent->vertex, EDGE_FSTREE);
    if(ep) {
      *ep = fse;
    }

    vertex_alloc_probe(&n->vertex);
  }
}

void shadow_create(
    fsent * restrict parent
  , fsent ** restrict np
  , const char * restrict name
  , uint16_t namel
  , vertex_kind kind
  , graph_invalidation_context * restrict invalidation
)
{
  fsent * n;

  fsent_create(&n, kind, VERTEX_OK, name, namel);

  n->fst = &fstree_shadow;
  n->fs = n->fst->fs;

  fsedge_connect(n, parent, invalidation);

  if(np) {
    *np = n;
  }
}

//
// public
//

void shadow_setup()
{
  moria_connect_context ctx;
  fsedge *fse;

  /* shadow root */
  fsent_create(&g_shadow, VERTEX_SHADOW_DIR, VERTEX_OK | VERTEX_PROTECT_BIT, MMS("(shadow)"));
  g_shadow->fst = &fstree_shadow;
  g_shadow->fs = fstree_shadow.fs;
  g_shadow->path_hash = 0xbeef;
  fsent_compute_identifiers(g_shadow);
  vertex_alloc_probe(&g_shadow->vertex);

  /* //module */
  fsent_create(&g_shadow_module, VERTEX_CONTEXT_MODULE, VERTEX_OK | VERTEX_PROTECT_BIT, MMS("module"));
  moria_preconnect(&ctx, &g_graph, &g_shadow_module->vertex, &g_shadow->vertex, EDGE_FSTREE, 0);
  fsedge_alloc(&fse);
  moria_connect(&ctx, &g_graph, &fse->edge, &g_shadow_module->vertex, &g_shadow->vertex, EDGE_FSTREE);
  fsent_compute_identifiers(g_shadow_module);
  vertex_alloc_probe(&g_shadow_module->vertex);

  /* //modules */
  fsent_create(&g_shadow_modules, VERTEX_SHADOW_DIR, VERTEX_OK | VERTEX_PROTECT_BIT, MMS("modules"));
  moria_preconnect(&ctx, &g_graph, &g_shadow_modules->vertex, &g_shadow->vertex, EDGE_FSTREE, 0);
  fsedge_alloc(&fse);
  moria_connect(&ctx, &g_graph, &fse->edge, &g_shadow_modules->vertex, &g_shadow->vertex, EDGE_FSTREE);
  fsent_compute_identifiers(g_shadow_modules);
  vertex_alloc_probe(&g_shadow_modules->vertex);

  /* //targets */
  fsent_create(&g_shadow_targets, VERTEX_SHADOW_FILE, VERTEX_OK | VERTEX_PROTECT_BIT, MMS("targets"));
  moria_preconnect(&ctx, &g_graph, &g_shadow_targets->vertex, &g_shadow->vertex, EDGE_FSTREE, 0);
  fsedge_alloc(&fse);
  moria_connect(&ctx, &g_graph, &fse->edge, &g_shadow_targets->vertex, &g_shadow->vertex, EDGE_FSTREE);
  fsent_compute_identifiers(g_shadow_targets);
  vertex_alloc_probe(&g_shadow_targets->vertex);

  /* the //module link is created in module_system_bootstrap */
}

void shadow_cleanup()
{
}

void shadow_graft_requires(
    module * restrict mod
  , fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , fsedge ** restrict nep
  , graph_invalidation_context * restrict invalidation
)
{
  fsent *displaced = 0;

  shadow_link(mod->shadow_requires, as, asl, ref, nep, &displaced, invalidation);

  if(displaced)
  {
    printf("require '%.*s' displaces earlier referent\n", (int)asl, as);

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}

void shadow_graft_rules(
    module * restrict mod
  , fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , fsedge ** restrict nep
  , graph_invalidation_context * restrict invalidation
)
{
  fsent *n, *displaced = 0;

  shadow_link(mod->shadow_rules, as, asl, ref, nep, &displaced, invalidation);

  if(displaced)
  {
    n = containerof(displaced->vertex.ref, fsent, vertex);

    printf("rules '%.*s' displaces earlier referent\n", (int)asl, as);
    fsent_absolute_path_say(n, g_narrator_stdout);
    printf("\n");
    fsent_absolute_path_say(ref, g_narrator_stdout);
    printf("\n");

    /* erase the referent */
    RUNTIME_ASSERT(n->shadow_rules_edge);
    n->shadow_rules_edge = 0;

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}

void shadow_graft_var(
    module * restrict mod
  , fsent * restrict ref
  , const char * restrict as
  , uint16_t asl
  , fsedge ** restrict nep
  , graph_invalidation_context * restrict invalidation
)
{
  fsent *n, *displaced = 0;

  shadow_link(mod->shadow_var, as, asl, ref, nep, &displaced, invalidation);

  if(displaced)
  {
    n = containerof(displaced->vertex.ref, fsent, vertex);

    printf("var '%.*s' displaces earlier referent\n", (int)asl, as);
    fsent_absolute_path_say(n, g_narrator_stdout);
    printf("\n");
    fsent_absolute_path_say(ref, g_narrator_stdout);
    printf("\n");

    /* erase the referent */
    RUNTIME_ASSERT(n->shadow_var_edge);
    n->shadow_var_edge = 0;

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}

void shadow_graft_bind(
    module * restrict mod
  , fsent * restrict ref
  , fsent * restrict attachment
  , fsent ** restrict target
  , graph_invalidation_context * restrict invalidation
)
{
  fsent *n, *displaced = 0;
  uint64_t u64;
  char id[64];
  uint16_t id_len;
  fsedge *e;

  u64 = fsent_abspath_hash(ref);
  id_len = zcatf(id, sizeof(id), "%016"PRIx64, u64);

  shadow_link(attachment, id, id_len, ref, &e, &displaced, invalidation);
  *target = containerof(e->edge.B, fsent, vertex);

  if(displaced)
  {
    /* erase the referent */
    n = containerof(displaced->vertex.ref, fsent, vertex);

    printf("import '%.*s' displaces earlier referent\n", id_len, id);
    fsent_absolute_path_say(n, g_narrator_stdout);
    printf("\n");
    fsent_absolute_path_say(ref, g_narrator_stdout);
    printf("\n");

    RUNTIME_ASSERT(n->shadow_imports_edge);
    n->shadow_imports_edge = 0;

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    fsent_release(displaced);
  }
}
