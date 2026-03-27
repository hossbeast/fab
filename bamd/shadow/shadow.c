/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "types.h"
#include "narrator.h"
#include "common/attrs.h"
#include "moria/connect.h"
#include "moria/edge.h"
#include "bacon/builder.h"

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
#include "namespace.h"
#include "notify.h"
#include "filesystem.h"
#include "fsname.h"
#include "system.h"
#include "build.h"
#include "rcu.h"
#include "identity.h"
#include "edge.h"

#include "zcat.h"
#include "ref_edge.h"

static fsent *g_shadow_namespaces_by;
static fsent *g_shadow_plugins_by;
static fsent *g_shadow_rmas_by;
static fsent *g_shadow_rules_by;
static fsent *g_shadow_build_records_by;

fsent *g_shadow;
fsent *g_shadow_bm;
fsent *g_shadow_clients;
fsent *g_shadow_bpes;
fsent *g_shadow_configsets;
fsent *g_shadow_formulas;
fsent *g_shadow_fstrees;
fsent *g_shadow_modules;
fsent *g_shadow_namespaces;
fsent *g_shadow_namespaces_byname;
fsent *g_shadow_fsnames;
fsent *g_shadow_plugins;
fsent *g_shadow_plugins_byname;
fsent *g_shadow_rmas;
fsent *g_shadow_rmas_byname;
fsent *g_shadow_rules;
fsent *g_shadow_rules_byname;
fsent *g_shadow_rulesets;
fsent *g_shadow_targets;
fsent *g_shadow_varsets;
fsent *g_shadow_build_records;
fsent *g_shadow_build_records_byorder;
fsent *g_shadow_formula_records;

moria_vertex *shadow_link(
    moria_connect_context * restrict ctx
  , fsent * restrict parent
  , const char * restrict name
  , uint16_t namel
)
{
  moria_vertex key;
  moria_edge *e;
  int r;

  RUNTIME_ASSERT(vertex_filetype_get(&parent->vertex) == VERTEX_FILETYPE_DIR);
  RUNTIME_ASSERT(parent->vertex.attrs & VERTEX_SHADOW_BIT);

  /* fsent for preconnect check */
  memset(&key, 0, sizeof(key));
  key.label = name;
  key.label_len = namel;

  r = moria_preconnect(ctx, &g_graph, &key, &parent->vertex, EDGE_FS, &e);
  if(r == MORIA_EDGEDOWN)
  {
    /* there already exists an edge with this label */
    RUNTIME_ASSERT((e->B->attrs & VERTEX_TYPE_OPT) == VERTEX_SHADOW_LINK);
    return e->B;
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_NOEDGE);
    return 0;
  }
}

void shadow_link_noref(
    fsent * restrict parent
  , const char * restrict name
  , uint16_t namel
  , moria_vertex * restrict ref
  , fsent ** restrict linkp
)
{
  moria_vertex *link;
  fsent *fse;
  fsedge *fe;
  moria_connect_context ctx;

  if((link = shadow_link(&ctx, parent, name, namel)) == 0)
  {
    fsent_create(&fse, VERTEX_SHADOW_LINK, VERTEX_OK, name, namel);
    link = &fse->vertex;
    link->attrs |= MORIA_VERTEX_LINK;

    fsedge_alloc(&fe);
    moria_connect(&ctx, &g_graph, &fe->edge, link, &parent->vertex, EDGE_FS);

    vertex_alloc_probe(link);
  }

  link->ref = ref;
  *linkp = containerof(link, fsent, vertex);
}

void shadow_mkref_new(
    fsent * restrict parent
  , const char * restrict name
  , uint16_t namel
  , moria_vertex * restrict ref
)
{
  moria_vertex *link;
  fsent *fse;
  fsedge *fe;
  moria_connect_context ctx;

  if((link = shadow_link(&ctx, parent, name, namel))) {
    /* existing link is not expected ; it should be torn down
     * before this can happen */
    RTA;
  }

  /* create the link */
  fsent_create(&fse, VERTEX_SHADOW_LINK, VERTEX_OK, name, namel);
  link = &fse->vertex;
  link->attrs |= MORIA_VERTEX_LINK;

  fsedge_alloc(&fe);
  moria_connect(&ctx, &g_graph, &fe->edge, link, &parent->vertex, EDGE_FS);
  vertex_alloc_probe(link);

  /* set the referent */
  link->ref = ref;

  /* in addition, the target has an EDGE_SHADOW_REF edge back to the link, this ensures that links
   * to the target are cleaned up when the target itself is reaped */
  ref_connect(link, ref, EDGE_SHADOW_REF);
}

void shadow_mkref(
    fsent * restrict parent
  , const char * restrict name
  , uint16_t namel
  , moria_vertex * restrict ref
)
{
  moria_vertex key, *link;
  moria_connect_context ctx;
  fsent *fse;
  fsedge *fe;
  moria_edge *e, *re;
  struct ref_edge *ref_edge;
  int r;

  RUNTIME_ASSERT(vertex_filetype_get(&parent->vertex) == VERTEX_FILETYPE_DIR);
  RUNTIME_ASSERT(parent->vertex.attrs & VERTEX_SHADOW_BIT);

  /* fsent for preconnect check */
  memset(&key, 0, sizeof(key));
  key.label = name;
  key.label_len = namel;

  r = moria_preconnect(&ctx, &g_graph, &key, &parent->vertex, EDGE_FS, &e);
  if(r == MORIA_EDGEDOWN)
  {
    /* there already exists an edge with this label */
    RUNTIME_ASSERT((e->B->attrs & VERTEX_TYPE_OPT) == VERTEX_SHADOW_LINK);
    link = e->B;

    /* teardown the existing ref edge */
    re = moria_edge_between(link, link->ref);
    ref_edge = containerof(re, typeof(*ref_edge), edge);
    moria_edge_disconnect(&g_graph, &ref_edge->edge);
    ref_edge_release(ref_edge);
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_NOEDGE);

    fsent_create(&fse, VERTEX_SHADOW_LINK, VERTEX_OK, name, namel);
    link = &fse->vertex;
    link->attrs |= MORIA_VERTEX_LINK;

    fsedge_alloc(&fe);
    moria_connect(&ctx, &g_graph, &fe->edge, link, &parent->vertex, EDGE_FS);

    vertex_alloc_probe(link);
  }

  /* set the referent */
  link->ref = ref;

  /* in addition, the target has an EDGE_SHADOW_REF edge back to the link, this ensures that links
   * to the target are cleaned up when the target itself is reaped */
  ref_connect(link, ref, EDGE_SHADOW_REF);
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
  fsent *n;

  fsent_create(&n, kind, VERTEX_OK, name, namel);

  fsedge_connect(n, parent, invalidation);
  vertex_alloc_probe(&n->vertex);

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

  fsent_create(&n, type, VERTEX_OK, name, name_len);
  moria_preconnect(&ctx, &g_graph, &n->vertex, &parent->vertex, EDGE_FS, 0);
  fsedge_alloc(&fse);
  moria_connect(&ctx, &g_graph, &fse->edge, &n->vertex, &parent->vertex, EDGE_FS);
  fsent_compute_identifiers(n);
  vertex_alloc_probe(&n->vertex);

  *dst = n;
}

void shadow_root_setup()
{
  fsent *n = 0;

  /* shadow root */
  fsent_create(&n, VERTEX_SHADOW_DIR, VERTEX_OK, MMS("(shadow)"));
  fsent_compute_identifiers(n);
  vertex_alloc_probe(&n->vertex);

  g_shadow = n;
}

void shadow_setup()
{
  g_shadow->fst = fstree_shadow;

  /* shadow root */
  shadow_mk(g_shadow, &g_shadow_targets, VERTEX_SHADOW_FILE, MMS("targets"));
  shadow_mk(g_shadow, &g_shadow_bm, VERTEX_SHADOW_DIR, MMS("bm"));

  /* shadow bm */
  shadow_mk(g_shadow_bm, &g_shadow_bpes, VERTEX_SHADOW_DIR, MMS("bpes"));
  shadow_mk(g_shadow_bm, &g_shadow_build_records, VERTEX_SHADOW_DIR, MMS("build-records"));
  shadow_mk(g_shadow_bm, &g_shadow_build_records_by, VERTEX_SHADOW_DIR, MMS("build-records-by"));
  shadow_mk(g_shadow_build_records_by, &g_shadow_build_records_byorder, VERTEX_SHADOW_DIR, MMS("order"));
  shadow_mk(g_shadow_bm, &g_shadow_clients, VERTEX_SHADOW_DIR, MMS("clients"));
  shadow_mk(g_shadow_bm, &g_shadow_configsets, VERTEX_SHADOW_DIR, MMS("configsets"));
  shadow_mk(g_shadow_bm, &g_shadow_formula_records, VERTEX_SHADOW_DIR, MMS("formula-records"));
  shadow_mk(g_shadow_bm, &g_shadow_formulas, VERTEX_SHADOW_DIR, MMS("formulas"));
  shadow_mk(g_shadow_bm, &g_shadow_fstrees, VERTEX_SHADOW_DIR, MMS("fstrees"));
  shadow_mk(g_shadow_bm, &g_shadow_modules, VERTEX_SHADOW_DIR, MMS("modules"));
  shadow_mk(g_shadow_bm, &g_shadow_namespaces, VERTEX_SHADOW_DIR, MMS("namespaces"));
  shadow_mk(g_shadow_bm, &g_shadow_namespaces_by, VERTEX_SHADOW_DIR, MMS("namespaces-by"));
  shadow_mk(g_shadow_namespaces_by, &g_shadow_namespaces_byname, VERTEX_SHADOW_DIR, MMS("name"));
  shadow_mk(g_shadow_bm, &g_shadow_fsnames, VERTEX_SHADOW_DIR, MMS("fsnames"));
  shadow_mk(g_shadow_bm, &g_shadow_plugins, VERTEX_SHADOW_DIR, MMS("plugins"));
  shadow_mk(g_shadow_bm, &g_shadow_plugins_by, VERTEX_SHADOW_DIR, MMS("plugins-by"));
  shadow_mk(g_shadow_plugins_by, &g_shadow_plugins_byname, VERTEX_SHADOW_DIR, MMS("name"));
  shadow_mk(g_shadow_bm, &g_shadow_rmas, VERTEX_SHADOW_DIR, MMS("rmas"));
  shadow_mk(g_shadow_bm, &g_shadow_rmas_by, VERTEX_SHADOW_DIR, MMS("rmas-by"));
  shadow_mk(g_shadow_rmas_by, &g_shadow_rmas_byname, VERTEX_SHADOW_DIR, MMS("name"));
  shadow_mk(g_shadow_bm, &g_shadow_rules, VERTEX_SHADOW_DIR, MMS("rules"));
  shadow_mk(g_shadow_bm, &g_shadow_rules_by, VERTEX_SHADOW_DIR, MMS("rules-by"));
  shadow_mk(g_shadow_rules_by, &g_shadow_rules_byname, VERTEX_SHADOW_DIR, MMS("name"));
  shadow_mk(g_shadow_bm, &g_shadow_rulesets, VERTEX_SHADOW_DIR, MMS("rulesets"));
  shadow_mk(g_shadow_bm, &g_shadow_varsets, VERTEX_SHADOW_DIR, MMS("varsets"));
}

void shadow_cleanup()
{
}

void shadow_graft_bind(
    module * restrict mod
  , fsent * restrict ref
  , fsent * restrict attachment
  , fsent ** restrict target
  , graph_invalidation_context * restrict invalidation
)
{
  const char *id;
  uint16_t id_len;
  moria_vertex *link;
  fsent *fse;
  moria_connect_context ctx;

  vertex_id(&ref->vertex, &id, &id_len);
  if((link = shadow_link(&ctx, attachment, id, id_len)) == 0)
  {
    fsent_create(&fse, VERTEX_SHADOW_LINK, VERTEX_OK, id, id_len);
    link = &fse->vertex;
    link->attrs |= MORIA_VERTEX_LINK;

    fsedge_connect_ctx(fse, attachment, &ctx, invalidation);

    vertex_alloc_probe(link);
  }

  link->ref = &ref->vertex;
  *target = containerof(link, fsent, vertex);
}

void fsent_shadow_link_describe(fsent * restrict fse, bacon_writer * restrict builder_writer)
{
  moria_vertex *v;

  v = &fse->vertex;
  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      vertex_mappings(&fse->vertex, builder_writer);
      { MAPPING
        STRING("ref")
        bacon_writer_push_string(builder_writer);
        if(v->ref) {
          vertex_path_say(v->ref, builder_writer->N);
        } else {
          narrator_says(builder_writer->N, "-none-");
        }
        bacon_writer_pop_string(builder_writer);
      }
      vertex_relations_describe(&fse->vertex, builder_writer);
    }
  }
}
