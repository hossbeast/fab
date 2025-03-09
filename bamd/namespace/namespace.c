/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "moria/graph.h"
#include "moria/connect.h"
#include "moria/foreach.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "bacon/value.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "yyutil/grammar.h"

#include "namespace.h"
#include "fsname.h"
#include "ns_edge.h"
#include "dir_fsname_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "fstree.h"
#include "shadow.h"
#include "vertex/probes.h"
#include "events/events.h"
#include "system.h"
#include "edge.h"
#include "channel.h"
#include "special_edge.h"
#include "edges.h"
#include "vertices.h"
#include "pattern.h"
#include "segments.h"
#include "section.h"
#include "match.h"
#include "search.internal.h"
#include "lookup.h"
#include "identity.h"
#include "graph.h"
#include "disintegrate.h"
#include "fsedge.h"
#include "request_handler.h"
#include "reconcile.h"
#include "config.h"

#include "zcat.h"

llist namespace_list = LLIST_INITIALIZER(namespace_list);                 // active namespacees
static llist namespace_freelist = LLIST_INITIALIZER(namespace_freelist);  // freelist
static rbtree *namespaces_config;

namespace *namespace_modules;
namespace *namespace_formulas;
namespace *namespace_plugins;

static namespace *namespace_alloc(const char * restrict name, uint16_t name_len)
{
  identity_ctx id;
  namespace *ns;

  if((ns = llist_shift(&namespace_freelist, typeof(*ns), vertex.owner))) {
    ASAN_CLEAN(ns);
    memset(ns, 0, sizeof(*ns));
  } else {
    xmalloc(&ns, sizeof(*ns));
  }

  moria_vertex_init(&ns->vertex, &g_graph, VERTEX_NAMESPACE | VERTEX_OK);

  ns->vertex.label = ns->id;
  ns->vertex.label_len = ns->id_len;
  ns->name_len = zcatw(ns->name, sizeof(ns->name), name, name_len);

  identity_init(&id);
  namespace_compute_id(ns, &id);
  ns->id_len = identity_mk(&id, ns->id, sizeof(ns->id));

  llist_append(&namespace_list, &ns->vertex.owner);
  vertex_alloc_probe(&ns->vertex);

  return ns;
}

static namespace *namespace_create(
    const char * restrict name
  , uint16_t name_len
  , graph_invalidation_context * restrict invalidation
)
{
  namespace *ns;

  ns = namespace_alloc(name, name_len);

  /* build out the namespace shadow dir */
  shadow_create(g_shadow_namespaces, &ns->shadow, ns->id, ns->id_len, VERTEX_SHADOW_DIR, invalidation);
  shadow_create(ns->shadow, &ns->shadow_fsnames, MMS("fsnames"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(ns->shadow, &ns->shadow_fsnames_by, MMS("fsnames-by"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(ns->shadow_fsnames_by, &ns->shadow_fsnames_byname, MMS("name"), VERTEX_SHADOW_DIR, invalidation);

  /* setup links */
  shadow_mkref(g_shadow_namespaces_byname, ns->name, ns->name_len, &ns->vertex);

  /* special edges to simplify listing by clients */
  special_connect(&ns->shadow->vertex, &ns->vertex, EDGE_SPECIAL, invalidation);

  return ns;
}

static fsname *lookup_name(namespace * restrict fsi, const char * restrict label, uint16_t label_len)
{
  fsname *entry;
  moria_vertex *v;

  if((v = moria_vertex_downw(&fsi->vertex, label, label_len))) {
    entry = containerof(v, typeof(*entry), vertex);
    return entry;
  }

  return 0;
}

/* misses create an fsname upon which a dependency can be taken
 * they are cleaned up once no such dependency exists */
static fsname *namespace_miss(namespace * restrict ns, const char * restrict name, uint16_t name_len)
{
  return fsname_create(ns, name, name_len);
}

//
// public
//

void namespace_release(namespace * restrict ns)
{
  moria_vertex *v;

  /* should be fully excised from the graph previously */
  v = &ns->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&ns->vertex.owner);
  llist_push(&namespace_freelist, &ns->vertex.owner);
  ASAN_TAINT(ns);
}

void namespace_put(namespace * restrict ns, fsent * restrict n, graph_invalidation_context * restrict invalidation)
{
  const moria_vertex *ref;
  fsname *entry;
  dir_fsname_edge *nse;
  moria_connect_context ctx;
  moria_edge *e;
  int r;

  ref = &n->vertex;
  if(vertex_type_get(&n->vertex) == VERTEX_SYMLINK) {
    ref = fsent_symlink_follow(n);
    if(ref == 0) {
      return;
    }
  }

  /* only directory nodes are indexed */
  switch(vertex_type_get(ref)) {
    case VERTEX_DIR:
      break;
    default:
      return;
  }

  /* create this name if it doesnt already exist in this namespace */
  if(!(entry = lookup_name(ns, n->vertex.label, n->vertex.label_len))) {
    ns->namespace_stats.count++;
    entry = fsname_create(ns, n->vertex.label, n->vertex.label_len);
  }

  r = moria_preconnect(&ctx, &g_graph, &n->vertex, &entry->vertex, EDGE_DIR_FSNAME, &e);
  if(r == MORIA_HASEDGE) {
    nse = containerof(e, typeof(*nse), edge);
    nse->epoch = reconciliation_epoch;
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  nse = dir_fsname_edge_alloc();
  nse->epoch = reconciliation_epoch;
  moria_connect(&ctx, &g_graph, &nse->edge, &n->vertex, &entry->vertex, EDGE_DIR_FSNAME);

  ns->namespace_stats.put++;
  entry->fsname_stats.put++;
  entry->fsname_stats.count++;

  vertex_invalidate(&entry->vertex, invalidation);
}

namespace *namespace_get(const char * restrict name, uint16_t name_len, graph_invalidation_context * restrict invalidation)
{
  namespace *ns;

  if((ns = namespace_lookup(name, name_len)) == 0) {
    ns = namespace_create(name, name_len, invalidation);
  }

  ns->epoch = reconciliation_epoch;
  return ns;
}

void namespace_setup(void)
{
}

void namespace_cleanup(void)
{
  namespace *ns;

  while((ns = llist_shift(&namespace_list, typeof(*ns), vertex.owner))) {
    graph_demolish(&ns->vertex);
  }

  while((ns = llist_shift(&namespace_freelist, typeof(*ns), vertex.owner))) {
    ASAN_CLEAN(ns);
    free(ns);
  }
}

namespace *namespace_lookup(const char * restrict name, uint16_t name_len)
{
  moria_vertex *v;
  namespace *fsi;

  if((v = moria_vertex_downw(&g_shadow_namespaces_byname->vertex, name, name_len)) == NULL) {
    return NULL;
  }
  v = moria_vertex_follow(v);
  fsi = containerof(v, typeof(*fsi), vertex);

  return fsi;
}

void namespace_show(const namespace * restrict fsi)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  namespace_describe(fsi, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void namespace_stats_build(const namespace * restrict fsi, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&fsi->namespace_stats.record, &descriptor_stats_namespace, builder_writer);
}

void namespace_describe(const namespace * restrict fsi, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fsi->vertex, builder_writer);
    { SET
      vertex_mappings(&fsi->vertex, builder_writer);
      { MAPPING STRING("name")
        BYTES(fsi->name, fsi->name_len)
      }
      { MAPPING
        STRING("stats")
        { SET
          namespace_stats_build(fsi, false, builder_writer);
        }
      }
      vertex_relations_describe(&fsi->vertex, builder_writer);
    }
  }
}

size_t namespace_path_cat(void * restrict dst, size_t sz, const namespace * restrict ns)
{
  return zcatf(dst, sz, "//bm/namespaces/%.*s", ns->id_len, ns->id);
}

void namespace_identity_describe(const struct namespace * restrict ns, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(ns->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(ns->id, ns->id_len);
  }
  { MAPPING
    STRING("name")
    BYTES(ns->name, ns->name_len);
  }
}

int namespace_resolve(struct lookup_context * restrict ctx, lookup_pattern * restrict pattern)
{
  pattern_section *section;
  pattern_section *init_section;
  pattern_segments *init_segments;
  pattern_section *last_section;
  pattern_segments *last_segments;
  moria_vertex *fsname;
  moria_foreach_state st;
  moria_edge *e;
  moria_vertex *v;
  fsent *fse;
  int r;
  char label[256];
  uint16_t label_len;

  ctx->rctx.ref = &pattern->pattern;

  /* sanity check */
  section = llist_first(&ctx->rctx.ref->sections, typeof(*section), lln);
  if(section->nodeset || section->axis != PATTERN_AXIS_DOWN) {
    RTA;
  }

  /* start with the last section, but if that section has file-type qualifiers, then
   * the initial section is preceding one */
  section = llist_last(&ctx->rctx.ref->sections, typeof(*section), lln);
  if(section->type_qualifiers_mask) {
    last_section = section;
    init_section = llist_prev(&ctx->rctx.ref->sections, section, lln);
  } else {
    last_section = 0;
    init_section = section;
  }

  /* the section from which the namespace lookup is performed must have a single qualifiers-list
   * and it may not contain any type qualifiers. this is enforced in the parser */
  RUNTIME_ASSERT(init_section->type_qualifiers_mask == 0);
  RUNTIME_ASSERT(llist_count(&init_section->qualifiers) == 1);

  init_segments = llist_first(&init_section->qualifiers, typeof(*init_segments), lln);
  RUNTIME_ASSERT(init_segments->qualifier_type == PATTERN_QUALIFIER_TYPE_AND);
  RUNTIME_ASSERT(init_segments->type_qualifiers_mask == 0);

  label_len = pattern_segments_cat(label, sizeof(label), ctx->rctx.vars, init_segments);

  /* get the set of starting vertices having the initial label */
  if((fsname = moria_vertex_downw(&ctx->ns->vertex, label, label_len)) == 0) {
    /* miss */
    ctx->miss = namespace_miss(ctx->ns, label, label_len);
    return ctx->rctx.r = 0;
  } else {
    ctx->hit = containerof(fsname, struct fsname, vertex);
  }

  /* the last section can have qualifiers */
  if(last_section) {
    last_segments = llist_first(&last_section->qualifiers, typeof(*last_segments), lln);
    label_len = pattern_segments_cat(label, sizeof(label), ctx->rctx.vars, last_segments);
  }

  r = 0;
  moria_foreach_edge_down(&st, fsname, e) {
    if(edge_type_get(e) != EDGE_DIR_FSNAME) {
      continue;
    }

    fse = containerof(e->B, typeof(*fse), vertex);
    /* leftwards match */
    if(!pattern_match_sections(&ctx->rctx.ref->sections, init_section, ctx->rctx.vars, fse)) {
      continue;
    }

    if(!last_section) {
      ctx->rctx.vertices[r++] = &fse->vertex;
      if(r == 2) {
        break;
      }
      continue;
    }

    if((v = moria_vertex_downw(&fse->vertex, label, label_len)) == 0) {
      continue;
    }

    /* evaluate type qualifiers if any */
    if(!pattern_section_evaluate_type_qualifiers(last_section, v)) {
      continue;
    }

    ctx->rctx.vertices[r++] = v;
    if(r == 2) {
      break;
    }
  }

  return ctx->rctx.r = r;
}

void namespace_reconfigure(struct config *cfg, bool dryrun, graph_invalidation_context * restrict invalidation)
{
  config_namespace_entry *nse;

  if(dryrun) {
    return;
  }

  namespaces_config = &cfg->namespaces.set;

  /* touch configured namespaces */
  rbtree_foreach(namespaces_config, nse, rbn) {
    namespace_get(nse->name->s, nse->name->len, invalidation);
  }

  /* touch the default namespace(s) */
  namespace_modules = namespace_get(MMS("modules"), invalidation);
  namespace_formulas = namespace_get(MMS("formulas"), invalidation);
  namespace_plugins = namespace_get(MMS("plugins"), invalidation);
}

void namespace_reconcile(struct graph_invalidation_context * restrict invalidation)
{
  config_namespace_entry *nse;
  namespace *ns;
  llist *T;

  /* touch configured namespaces */
  rbtree_foreach(namespaces_config, nse, rbn) {
    namespace_get(nse->name->s, nse->name->len, invalidation);
  }

  /* touch the default namespace(s) */
  namespace_get(MMS("modules"), invalidation);
  namespace_get(MMS("formulas"), invalidation);
  namespace_get(MMS("plugins"), invalidation);

  /* cleanup un-referenced namespaces */
  llist_foreach_safe(&namespace_list, ns, vertex.owner, T) {
    if(ns->epoch == reconciliation_epoch) {
      continue;
    }

    graph_demolish(&ns->vertex);
  }

  /* refresh and establish directory <-> namespace fsname EDGE_DIR_FSNAME edges */
  fsent_namespace_refresh(invalidation);

  /* cleanup expired EDGE_DIR_FSNAME edges */
  dir_fsname_edge_reconcile(invalidation);
}

void namespace_id(const struct namespace * restrict ns, const char ** restrict id, uint16_t * restrict idl)
{
  *id = ns->id;
  *idl = ns->id_len;
}

void namespace_compute_id(const struct namespace * restrict ns, struct identity_ctx * restrict id)
{
  identity_name(id, VERTEX_NAMESPACE, ns->name, ns->name_len);
}
