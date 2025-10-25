/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "types.h"
#include "narrator.h"
#include "common/attrs.h"
#include "moria/connect.h"

#include "shadow.h"
#include "fstree.h"
#include "fsedge.h"
#include "fsent.h"
#include "module.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "config.h"
#include "goals.h"
#include "buildplan.h"
#include "module.h"
#include "params.h"
#include "path_cache.h"
#include "path_cache.h"
#include "fsindex.h"
#include "notify.h"
#include "filesystem.h"
#include "fsname.h"
#include "system.h"
#include "build.h"

#include "zcat.h"

fsent *g_shadow;
fsent *g_shadow_modules;
fsent *g_shadow_rulesets;
fsent *g_shadow_formulas;
fsent *g_shadow_varsets;
fsent *g_shadow_configsets;
fsent *g_shadow_plugins;
fsent *g_shadow_fsnames;
fsent *g_shadow_fsnames_by;
fsent *g_shadow_fsnames_byname;
fsent *g_shadow_targets;
fsent *g_shadow_rules;
fsent *g_shadow_rules_by;
fsent *g_shadow_rules_byname;
fsent *g_shadow_bpes;
fsent *g_shadow_rmas;
fsent *g_shadow_rmas_by;
fsent *g_shadow_rmas_byname;
fsent *g_shadow_fstrees;

static uint32_t link_refmark;

void shadow_link(
    fsent * restrict parent
  , const char * restrict name
  , uint16_t namel
  , moria_vertex * restrict ref
  , fsedge ** restrict ep
)
{
  fsent *link;
  moria_connect_context ctx;
  fsedge *fse;
  moria_edge *e;
  int r;

  RUNTIME_ASSERT(vertex_type_get(&parent->vertex) == VERTEX_SHADOW_DIR);

  /* allocate a new fsent to point to the referent */
  fsent_create(&link, VERTEX_SHADOW_LINK, VERTEX_OK, name, namel);
  link->vertex.ref = ref;
  link->vertex.attrs |= MORIA_VERTEX_LINK;

  r = moria_preconnect(&ctx, &g_graph, &link->vertex, &parent->vertex, EDGE_FSTREE, &e);
  if(r == MORIA_EDGEDOWN)
  {
    RUNTIME_ASSERT((e->B->attrs & VERTEX_TYPE_OPT) == VERTEX_SHADOW_LINK);
    fse = containerof(e, typeof(*fse), edge);

    /* there already exists an edge with this label */
    fsent_release(link);

    /* overwrite the referent */
    e->B->ref = ref;
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_NOEDGE);
    fsedge_alloc(&fse);
    moria_connect(&ctx, &g_graph, &fse->edge, &link->vertex, &parent->vertex, EDGE_FSTREE);
    fse->refmark = ++link_refmark;

    vertex_alloc_probe(&link->vertex);
  }

  fse->refmark = ++link_refmark;

  if(ep) {
    *ep = fse;
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

  fsedge_connect(n, parent, invalidation);

  if(np) {
    *np = n;
  }
}

//
// public
//

static void shadow_mk(fsent * restrict parent, fsent ** restrict dst, enum vertex_type type, const char *name, uint16_t name_len)
{
  moria_connect_context ctx;
  fsedge *fse;
  fsent *n;

  fsent_create(&n, type, VERTEX_OK | VERTEX_PROTECT_BIT, name, name_len);
  n->fst = &fstree_shadow;
  moria_preconnect(&ctx, &g_graph, &n->vertex, &parent->vertex, EDGE_FSTREE, 0);
  fsedge_alloc(&fse);
  moria_connect(&ctx, &g_graph, &fse->edge, &n->vertex, &parent->vertex, EDGE_FSTREE);
  fsent_compute_identifiers(n);
  vertex_alloc_probe(&n->vertex);

  *dst = n;
}

void shadow_setup()
{
  /* shadow root */
  fsent_create(&g_shadow, VERTEX_SHADOW_DIR, VERTEX_OK | VERTEX_PROTECT_BIT, MMS("(shadow)"));
  g_shadow->fst = &fstree_shadow;
  g_shadow->path_hash = 0xbeef;
  fsent_compute_identifiers(g_shadow);
  vertex_alloc_probe(&g_shadow->vertex);

  /* specials */
  shadow_mk(g_shadow, &g_shadow_modules, VERTEX_SHADOW_DIR, MMS("modules"));
  shadow_mk(g_shadow, &g_shadow_rulesets, VERTEX_SHADOW_DIR, MMS("rulesets"));
  shadow_mk(g_shadow, &g_shadow_formulas, VERTEX_SHADOW_DIR, MMS("formulas"));
  shadow_mk(g_shadow, &g_shadow_varsets, VERTEX_SHADOW_DIR, MMS("varsets"));
  shadow_mk(g_shadow, &g_shadow_configsets, VERTEX_SHADOW_DIR, MMS("configsets"));
  shadow_mk(g_shadow, &g_shadow_plugins, VERTEX_SHADOW_DIR, MMS("plugins"));
  shadow_mk(g_shadow, &g_shadow_fstrees, VERTEX_SHADOW_DIR, MMS("fstrees"));
  shadow_mk(g_shadow, &g_shadow_fsnames, VERTEX_SHADOW_DIR, MMS("fsnames"));
  shadow_mk(g_shadow, &g_shadow_fsnames_by, VERTEX_SHADOW_DIR, MMS("fsnames-by"));
  shadow_mk(g_shadow_fsnames_by, &g_shadow_fsnames_byname, VERTEX_SHADOW_DIR, MMS("name"));

  /* entities */
  shadow_mk(g_shadow, &g_shadow_rules, VERTEX_SHADOW_DIR, MMS("rules"));
  shadow_mk(g_shadow, &g_shadow_rules_by, VERTEX_SHADOW_DIR, MMS("rules-by"));
  shadow_mk(g_shadow_rules_by, &g_shadow_rules_byname, VERTEX_SHADOW_DIR, MMS("name"));
  shadow_mk(g_shadow, &g_shadow_rmas, VERTEX_SHADOW_DIR, MMS("rmas"));
  shadow_mk(g_shadow, &g_shadow_rmas_by, VERTEX_SHADOW_DIR, MMS("rmas-by"));
  shadow_mk(g_shadow_rmas_by, &g_shadow_rmas_byname, VERTEX_SHADOW_DIR, MMS("name"));
  shadow_mk(g_shadow, &g_shadow_bpes, VERTEX_SHADOW_DIR, MMS("bpes"));

  /* magic files */
  shadow_mk(g_shadow, &g_shadow_targets, VERTEX_SHADOW_FILE, MMS("targets"));

  /* magick links */
  shadow_link(g_shadow, MMS("build"), g_build, 0);
  shadow_link(g_shadow, MMS("buildplan"), g_buildplan, 0);
  shadow_link(g_shadow, MMS("config"), g_config, 0);
  shadow_link(g_shadow, MMS("filesystem"), g_filesystem, 0);
  shadow_link(g_shadow, MMS("goals"), g_goals, 0);
  shadow_link(g_shadow, MMS("fsindex"), g_fsindex, 0);
  shadow_link(g_shadow, MMS("notify"), g_notify, 0);
  shadow_link(g_shadow, MMS("module"), g_module, 0);
  shadow_link(g_shadow, MMS("path-cache"), g_path_cache, 0);
  shadow_link(g_shadow, MMS("system"), g_system, 0);
}

void shadow_cleanup()
{
}

void shadow_graft_bind(
    module * restrict mod
  , fsent * restrict ref
  , fsent * restrict attachment
  , fsent ** restrict target
)
{
  uint64_t u64;
  char id[64];
  uint16_t id_len;
  fsedge *e;

  u64 = fsent_abspath_hash(ref);
  id_len = zcatf(id, sizeof(id), "%016"PRIx64, u64);

  shadow_link(attachment, id, id_len, &ref->vertex, &e);
  *target = containerof(e->edge.B, fsent, vertex);
}
