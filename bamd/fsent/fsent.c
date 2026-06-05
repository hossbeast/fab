/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "bam/ipc.h"
#include "moria/graph.h"
#include "moria/traverse.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "moria/connect.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/fd.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"

#include "fsent.h"
#include "vertex.h"
#include "config.h"
#include "configset.h"
#include "dependency.h"
#include "events/events.h"
#include "fstree.h"
#include "formula.h"
#include "fsedge.h"
#include "channel.h"
#include "module.h"
#include "params.h"
#include "rma.h"
#include "rule_system.h"
#include "dir_rma_edge.h"
#include "rule.h"
#include "shadow.h"
#include "stats.h"
#include "varset.h"
#include "channel.h"
#include "vertex/probes.h"
#include "ruleset.h"
#include "selection.h"
#include "special_edge.h"
#include "edge.h"
#include "meta_edge.h"
#include "edges.h"
#include "vertices.h"
#include "walker.h"
#include "namespace.h"
#include "plugin.h"
#include "bpe.h"
#include "config_system.h"
#include "ns_edge.h"
#include "notify.h"
#include "stats_fsent_dir.desc.h"
#include "identity.h"
#include "system.h"
#include "symlink.h"
#include "disintegrate.h"

#include "common/attrs.h"
#include "zcat.h"
#include "marshal.h"

uint16_t fsent_fstree_epoch;
uint16_t fsent_account_epoch;

llist fsent_list = LLIST_INITIALIZER(fsent_list);                       // active fsents
static llist fsent_freelist = LLIST_INITIALIZER(fsent_freelist);        // free fsents

/* directory nodes which had previously been mounted for fstree */
static llist fsent_unmountlist = LLIST_INITIALIZER(fsent_unmountlist);

/* invalidated fsents - primary nodes to be marked fresh at end of reconcile */
static llist fsent_freshenlist = LLIST_INITIALIZER(fsent_freshenlist);  // fsent freshen candidates

/* invalidated fsents - primary nodes in managed build dirs to be deleted at end of reconcile */
static llist fsent_unlinklist = LLIST_INITIALIZER(fsent_unlinklist);    // fresnt unlink candidates

fsent *g_root;
fsent *g_base;
char g_basedir[PATH_MAX] = "/base";
uint16_t g_basedir_len;
int g_basedirfd;

attrs32 *fsent_property_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FSENT_PROPERTY_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FSENT_PROPERTY_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(fsent_property_attrs);
}

static void __attribute__((nonnull)) fsent_dispose(fsent * restrict n)
{
  free(n->symlink_target);

  llist_delete(&n->vertex.owner);
  llist_append(&fsent_freelist, &n->vertex.owner);
  ASAN_TAINT(n);
}

static void fsent_alloc(
    fsent ** restrict np
  , vertex_kind kind
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
{
  fsent *n;

  RUNTIME_ASSERT(label_len);
  RUNTIME_ASSERT((attrs & VERTEX_TYPE_OPT) == 0);

  if((n = llist_shift(&fsent_freelist, typeof(*n), vertex.owner))) {
    ASAN_CLEAN(n);
    memset(n, 0, sizeof(*n));
  } else {
    xmalloc(&n, sizeof(*n));
  }

  fsent_name_init(&n->name, label, label_len);
  moria_vertex_initw(&n->vertex, &g_graph, attrs | kind | VERTEX_KIND_FSENT, n->name.name, n->name.namel);

  n->in_epoch = -1;
  n->in_wd = -1;

  n->fst = fstree_root;

  llist_init(&n->pending.lln);
  n->pending.kind = SWEEPER_EVENT_SELF;
  rbtree_init(&n->pending_child);
  llist_init(&n->fs_mkdir_lln);
  llist_init(&n->fs_readlink_lln);
  llist_init(&n->unmount_lln);
  llist_init(&n->unlink_lln);
  llist_init(&n->freshen_lln);
  llist_init(&n->notify_watched_files_lln);
  llist_init(&n->bpe_reconcile_lln);

  if(vertex_type_get(&n->vertex) == VERTEX_SYMLINK) {
    xmalloc(&n->symlink_target, PATH_MAX);
    n->symlink_target_sz = PATH_MAX;
    RUNTIME_ASSERT(n->symlink_target_sz >= PATH_MAX);
  }

  llist_append(&fsent_list, &n->vertex.owner);

  *np = n;
}

static size_t relative_path_cat(void * restrict dst, size_t sz, const fsent * subject, const fsent * base, size_t (*zcatw_fn)(void * restrict, size_t, const void * restrict, size_t))
{
  const fsent *n;
  const fsent *bs[64];
  size_t bsl = 0;
  const fsent *ns[64];
  size_t nsl = 0;
  size_t z = 0;
  int x;

  /* subject and base are the same fsent */
  if(subject == base)
  {
    z += zcatw_fn(dst + z, sz - z, ".", 1);
    goto end;
  }

  /* path from base up -> subject */
  n = base;
  while(n)
  {
    if(n == subject)
      break;

    if(n == g_root)
      break;

    RUNTIME_ASSERT(bsl < (sizeof(bs) / sizeof(bs[0])));
    bs[bsl++] = n;
    n = fsent_parent(n);
  }

  /* base is directly up from subject */
  if(n == subject) {
    goto end;
  }

  /* path from subject up -> base */
  n = subject;
  while(n)
  {
    if(n == base)
      break;

    if(n == g_root)
      break;

    RUNTIME_ASSERT(nsl < (sizeof(ns) / sizeof(ns[0])));
    ns[nsl++] = n;
    n = fsent_parent(n);
  }

  /* subject is directly up from base */
  if(n == base)
  {
    bsl = 0;
  }

  /* find the nearest common ancestor */
  while(nsl && bsl && ns[nsl - 1] == bs[bsl - 1])
  {
    nsl--;
    bsl--;
  }

end:
  /* path from base to nearest common ancestor */
  for(x = 0; x < bsl; x++)
  {
    if(z) {
      z += zcatw_fn(dst + z, sz - z, "/", 1);
    }
    z += zcatw_fn(dst + z, sz - z, "..", 2);
  }

  /* path from nearest common ancestor to the subject */
  for(x = nsl - 1; x >= 0; x--)
  {
    n = ns[x];

    if(z) {
      z += zcatw_fn(dst + z, sz - z, "/", 1);
    }
    z += zcatw_fn(dst + z, sz - z, n->name.name, n->name.namel);
  }

  ((char*)dst)[z] = 0;

  return z;
}

//
// internal
//

size_t fsent_relative_path_cat(void * restrict dst, size_t sz, const fsent * subject, const fsent * base)
{
  return relative_path_cat(dst, sz, subject, base, zcatw);
}

size_t fsent_relative_path_cat_bacon(void * restrict dst, size_t sz, const fsent * subject, const fsent * base)
{
  return relative_path_cat(dst, sz, subject, base, bacon_value_string_zcatw);
}

void fsent_relative_path_say(const fsent * n, const fsent * base, narrator * restrict N)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = fsent_relative_path_cat(path, sizeof(path), n, base);

  xsayw(path, pathl);
}

void fsent_create(
    fsent ** restrict np
  , vertex_kind kind
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
{
  fsent *n;

  fsent_alloc(&n, kind, attrs, label, label_len);

  *np = n;
}

void fsent_id_path_compute(const fsent * restrict fse, identity_ctx * restrict id)
{
  const fsent *n;
  const fsent *bs[64];
  uint16_t bsl = 0;
  int x;

  for(n = fse; n; n = fsent_parent(n)) {
    RUNTIME_ASSERT(bsl < ARRAY_LEN(bs));
    bs[bsl++] = n;
  }

  if(bs[bsl - 1] == g_root) {
    identity_add(id, "/", 1);
  } else {
    identity_add(id, "//", 2);
  }
  for(x = bsl - 2; x >= 0; x--) {
    if(x != bsl - 2) {
      identity_add(id, "/", 1);
    }
    identity_add(id, bs[x]->vertex.label, bs[x]->vertex.label_len);
  }
}

void fsent_compute_id(const fsent * restrict fse, identity_ctx * restrict id)
{
  vertex_filetype filetype;
  const char *type;
  const moria_vertex *v;

  /* dir, file, symlink */
  v = &fse->vertex;
  filetype = vertex_filetype_get(v);
  type = attrs32_name_byvalue(graph_filetype_attrs, filetype);
  identity_add(id, type, strlen(type));

  identity_add(id, " ", 1);

  fsent_id_path_compute(fse, id);
}

void fsent_compute_identifiers(fsent * restrict fse)
{
  identity_ctx id;

  identity_init(&id);
  fsent_compute_id(fse, &id);
  fse->id_len = identity_mk(&id, fse->id, sizeof(fse->id));
}

void fsent_release(fsent * restrict n)
{
  moria_vertex *v;

  /* should be fully excised from the graph previously */
  v = &n->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  notify_rm_watch(n);
  llist_delete(&n->fs_mkdir_lln);
  llist_delete(&n->fs_readlink_lln);
  llist_delete(&n->unmount_lln);
  llist_delete(&n->unlink_lln);
  llist_delete(&n->freshen_lln);
  llist_delete(&n->notify_watched_files_lln);
  llist_delete(&n->bpe_reconcile_lln);

  fsent_dispose(n);
}

//
// public
//

/* create the root fsent (g_root) */
void fsent_root_setup()
{
  fsent *n = 0;

  fsent_alloc(&n, VERTEX_DIR, VERTEX_EXISTS_BIT, MMS("(root)"));

  fsent_compute_identifiers(n);
  vertex_alloc_probe(&n->vertex);

  g_root = n;
}

void fsent_base_setup()
{
  fsent_graft(g_basedir, &g_base);
}

void fsent_root_cleanup()
{
  graph_demolish(&g_root->vertex);
  graph_demolish(&g_shadow->vertex);
}

void fsent_cleanup()
{
  fsent *n;

  while((n = llist_shift(&fsent_freelist, typeof(*n), vertex.owner))) {
    ASAN_CLEAN(n);
    free(n);
  }
}

void fsent_graft(const char * restrict base, fsent ** restrict rn)
{
  fsent * parent;
  fsent * child = 0;
  fsedge *fse;
  const char *seg;
  const char *end;
  moria_vertex *v;
  moria_connect_context ctx;
  int r;

  parent = g_root;
  seg = base + 1;
  while(*seg)
  {
    end = seg + 1;
    while(*end && *end != '/') {
      end++;
    }

    v = moria_vertex_downw(&parent->vertex, seg, end - seg);
    if(v)
    {
      child = containerof(v, fsent, vertex);
    }
    else
    {
      fsent_create(&child, VERTEX_DIR, VERTEX_OK, seg, end - seg);

      r = moria_preconnect(&ctx, &g_graph, &child->vertex, &parent->vertex, EDGE_FS, 0);
      RUNTIME_ASSERT(r == MORIA_NOEDGE);
      fsedge_alloc(&fse);
      moria_connect(&ctx, &g_graph, &fse->edge, &child->vertex, &parent->vertex, EDGE_FS);

      /* compute final identifiers */
      fsent_compute_identifiers(child);
      vertex_alloc_probe(&child->vertex);
    }
    parent = child;

    seg = end;
    if(*seg) {
      seg++;
    }
  }

  *rn = child;
}

size_t fsent_absolute_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
{
  const fsent *base;
  size_t z = 0;

  RUNTIME_ASSERT(vertex_kind_get(&n->vertex) == VERTEX_KIND_FSENT);

  if(n == g_root)
  {
    z += zcats(dst, sz, "/");
    return z;
  }
  else if(n == g_shadow)
  {
    z += zcats(dst, sz, "//");
    return z;
  }
  else if(vertex_shadow_get(&n->vertex))
  {
    z += zcats(dst, sz, "//");
    base = g_shadow;
  }
  else
  {
    z += zcats(dst, sz, "/");
    base = g_root;
  }

  z += fsent_relative_path_cat(dst + z, sz - z, n, base);
  return z;
}

size_t fsent_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
{
  if(vertex_shadow_get(&n->vertex))
  {
    return fsent_absolute_path_cat(dst, sz, n);
  }
  else
  {
    return fsent_base_relative_path_cat(dst, sz, n);
  }
}

size_t fsent_base_relative_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
{
  if(g_base == 0)
  {
    /* project not yet initialized */
    return fsent_absolute_path_cat(dst, sz, n);
  }

  return fsent_relative_path_cat(dst, sz, n, g_base);
}

size_t fsent_property_cat(void * dst, size_t sz, const fsent * restrict n, fsent_property property, const fsent_property_context * restrict ctx)
{
  uint32_t attrs;
  size_t z;

  z = 0;
  if(property == FSENT_PROPERTY_ID) {
    z += vertex_id_cat(dst + z, sz - z, &n->vertex);
  } else if(property == FSENT_PROPERTY_NAME) {
    z += zcatw(dst + z, sz - z, n->name.name, n->name.namel);
  } else if(property == FSENT_PROPERTY_EXT) {
    z += zcatw(dst + z, sz - z, n->name.ext, n->name.extl);
  } else if(property == FSENT_PROPERTY_SUFFIX) {
    z += zcatw(dst + z, sz - z, n->name.suffix, n->name.suffixl);
  } else if(property == FSENT_PROPERTY_BASE) {
    z += zcatw(dst + z, sz - z, n->name.base, n->name.basel);
  } else if(property == FSENT_PROPERTY_ABSDIR || property == FSENT_PROPERTY_RELDIR) {
    if(vertex_filetype_get(&n->vertex) != VERTEX_FILETYPE_DIR) {
      n = fsent_parent(n);
    }

    if(property == FSENT_PROPERTY_ABSDIR || vertex_shadow_get(&n->vertex))
    {
      z += fsent_absolute_path_cat(dst + z, sz - z, n);
    }
    else
    {
      z += fsent_relative_path_cat(dst + z, sz - z, n, ctx->cwd);//mod->dir_node);
    }
  } else if(property == FSENT_PROPERTY_ABSPATH || property == FSENT_PROPERTY_RELPATH) {
    if(property == FSENT_PROPERTY_ABSPATH || vertex_shadow_get(&n->vertex))
    {
      z += fsent_absolute_path_cat(dst + z, sz - z, n);
    }
    else
    {
      z += fsent_relative_path_cat(dst + z, sz - z, n, ctx->cwd);
    }
  } else if(property == FSENT_PROPERTY_STATE) {
    attrs = n->vertex.attrs & VERTEX_STATE_OPT;
    if(attrs)
    {
      z += attrs32_zcat(dst + z, sz - z, graph_vertex_attrs, attrs);
    }
  } else if(property == FSENT_PROPERTY_TYPE) {
    z += attrs32_zcat(dst + z, sz - z, graph_vertex_attrs, n->vertex.attrs & VERTEX_TYPE_OPT);
  }

  return z;
}

void fsent_base_relative_path_say(const fsent * restrict n, narrator * restrict N)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = fsent_base_relative_path_cat(path, sizeof(path), n);
  xsayw(path, pathl);
}

void fsent_base_relative_path_describe(const fsent * restrict n, bacon_writer * restrict writer)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = fsent_base_relative_path_cat(path, sizeof(path), n);
  bacon_writer_bytes(writer, path, pathl);
}

void fsent_absolute_path_say(const fsent * restrict n, narrator * restrict N)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = fsent_absolute_path_cat(path, sizeof(path), n);
  xsayw(path, pathl);
}

void fsent_absolute_path_describe(const fsent * restrict n, bacon_writer * restrict writer)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = fsent_absolute_path_cat(path, sizeof(path), n);
  bacon_writer_bytes(writer, path, pathl);
}

void fsent_path_say(fsent * restrict n, struct narrator * restrict N)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = fsent_path_cat(path, sizeof(path), n);
  xsayw(path, pathl);
}

void fsent_property_say(const fsent * restrict n, fsent_property property, const fsent_property_context * restrict ctx, narrator * restrict N)
{
  char space[PATH_MAX];
  size_t sz;

  sz = fsent_property_cat(space, sizeof(space), n, property, ctx);
  xsayw(space, sz);
}

formula *fsent_formula_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  identity_ctx id;
  formula *fml;

  if((fml = fsent_is_formula_file(fse))) {
    return fml;
  }

  formula_create(&fml, &g_graph);
  fsent_absolute_path_cat(fml->abspath, sizeof(fml->abspath), fse);
  fml->abspath_len = strlen(fml->abspath);

  fml->self_node = fse;

  identity_init(&id);
  vertex_compute_id(&fml->vertex, &id);
  fml->id_len = identity_mk(&id, fml->id, sizeof(fml->id));

  fml->vertex.label = fml->id;
  fml->vertex.label_len = fml->id_len;

  special_connect(&fse->vertex, &fml->vertex, EDGE_FSENT_FML, invalidation);

  /* shadow link to the special node itself */
  shadow_mkref(g_shadow_formulas, fml->id, fml->id_len, &fml->vertex);

  return fml;
}

varset *fsent_varset_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  identity_ctx id;
  struct varset *var;

  if((var = fsent_is_varset_file(fse))) {
    return var;
  }

  varset_alloc(&var, &g_graph);
  fsent_absolute_path_cat(var->self_node_abspath, sizeof(var->self_node_abspath), fse);
  var->self_node_abspath_len = strlen(var->self_node_abspath);

  var->self_node = fse;

  identity_init(&id);
  varset_compute_id(var, &id);
  var->id_len = identity_mk(&id, var->id, sizeof(var->id));

  var->vertex.label = var->id;
  var->vertex.label_len = var->id_len;

  special_connect(&fse->vertex, &var->vertex, EDGE_FSENT_VARSET, invalidation);

  /* shadow link to the special node itself */
  shadow_mkref(g_shadow_varsets, var->id, var->id_len, &var->vertex);

  return var;
}

void fsent_configset_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  configset *cfg;
  identity_ctx id;

  if(fsent_is_configset_file(fse)) {
    return;
  }

  configset_alloc(&cfg);
  fsent_absolute_path_cat(cfg->self_node_abspath, sizeof(cfg->self_node_abspath), fse);
  cfg->self_node_abspath_len = strlen(cfg->self_node_abspath);

  cfg->self_node = fse;

  identity_init(&id);
  configset_compute_id(cfg, &id);
  cfg->id_len = identity_mk(&id, cfg->id, sizeof(cfg->id));

  cfg->vertex.label = cfg->id;
  cfg->vertex.label_len = cfg->id_len;

  special_connect(&fse->vertex, &cfg->vertex, EDGE_FSENT_CONFIGSET, invalidation);

  /* shadow link to the special node itself */
  shadow_mkref(g_shadow_configsets, cfg->id, cfg->id_len, &cfg->vertex);
}

void fsent_unlink(fsent * restrict n, struct graph_disintegrate_context * restrict ctx)
{
  fsent *p;
  moria_foreach_state st;
  moria_edge *edge;

  /* invalidate the dirnode */
  if((p = fsent_parent(n))) {
    vertex_invalidate(&p->vertex, ctx->invalidation);
  }

  /* if the directory is queued up for unlinking, cancel that */
  llist_delete(&n->unlink_lln);
  llist_delete(&n->fs_mkdir_lln);
  llist_delete(&n->fs_readlink_lln);

  /* if this node has a pending event in the sweeper thread, cancel it */
  sweeper_thread_cancel(n);

  /* no need to readlink */
  symlink_readlink_cancel(n);

  moria_foreach_edge(&st, &n->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_FSENT_FML:
      case EDGE_FSENT_VARSET:
      case EDGE_FSENT_CONFIGSET:
      case EDGE_FSENT_RULESET:
        vertices_push(ctx->unlinking, edge->A);
      case EDGE_SHADOW_REF:
      case EDGE_DIR_RMA:
      case EDGE_DIR_FSNAME:

      /* comment */
      case EDGE_REF_SYMLINK:
      case EDGE_BUILD_DIR_MOD:

      /* ?? */
      case EDGE_LOOKUP:

/* re
 *    case EDGE_SRC_BPE:
 *    case EDGE_BPE_TGT:
 *
 * its necessary to OMIT these here, to retain nodes in the unlinked state
 * which are still the source or target of some dependency
 *
 * when the rule that created them is run again, the bpe is not refreshed
 * because now the fsent is unlinked and rules dont match it. when that bpe
 * is destroyed, then these edges are removed, and with them this node
 */
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

bool fsent_dir_reap(struct fsent * restrict dir, struct graph_disintegrate_context * restrict ctx)
{
  return false; // retain empty directory nodes
}

fsent *fsent_translate(fsent * fse, const fsent *origin, const fsent *target, graph_invalidation_context * restrict invalidation)
{
  const moria_vertex *v, *sv, *next_v;
  fsent *p, *next_n;
  vertices list, *eln;

  vertices_init(&list);
  for(p = fse; p != g_root; p = fsent_parent(p))
  {
    if(p == origin) {
      break;
    }
    if(p == target) {
      break;
    }

    vertices_unshift(&list, &p->vertex);
  }

  if(p == g_root) {
    /* fse is outside both trees */
    vertices_destroy(&list);
    return fse;
  }

  if(p == target) {
    /* already in context of the target tree */
    vertices_destroy(&list);
    return fse;
  }

  /* convert from origin -> target */
  v = &target->vertex;
  vertices_foreach(&list, sv, eln) {
    if((next_v = moria_vertex_downw(v, sv->label, sv->label_len)) == 0) {
      /* create as not yet existing */
      fsent_create(&next_n, VERTEX_FILE, VERTEX_UNCREATED, sv->label, sv->label_len);
      p = containerof(v, typeof(*p), vertex);
      fsedge_connect(next_n, p, invalidation);
      vertex_alloc_probe(&next_n->vertex);
      next_v = &next_n->vertex;
    }

    v = next_v;
  }

  vertices_destroy(&list);
  return containerof(v, fsent, vertex);
}

bool fsent_is_descendant_of(const fsent * restrict ancestor, const fsent * restrict n)
{
  const fsent *p;

  for(p = fsent_parent(n); p; p = fsent_parent(p)) {
    if(p == ancestor) {
      return true;
    }
  }

  return false;
}

bool fsent_is_self_or_descendant(const fsent * restrict ancestor, const fsent * restrict n)
{
  if(n == ancestor) {
    return true;
  }

  return fsent_is_descendant_of(ancestor, n);
}

static bool fsent_possibly_primary(const fsent * restrict fse)
{
  switch(vertex_type_get(&fse->vertex)) {
    case VERTEX_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SYMLINK:
      return true;
    default:
      return false;
  }
}

bool fsent_is_primary(const fsent * restrict fse)
{
  moria_foreach_state st;
  moria_edge *edge;

  if(!fsent_possibly_primary(fse)) {
    return false;
  }

  moria_foreach_edge(&st, &fse->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_BPE_TGT:
      case EDGE_FSENT_FML:
      case EDGE_FSENT_VARSET:
      case EDGE_FSENT_CONFIGSET:
      case EDGE_FSENT_RULESET:
      case EDGE_LOOKUP_PLUGIN_FILE:
        return false;
      default:
        break;
    }
  }

  return true;
}

void fsent_freshen_candidate(fsent * restrict fse)
{
  llist_delete(&fse->freshen_lln);
  llist_push(&fsent_freshenlist, &fse->freshen_lln);
}

void fsent_unmount_candidate(fsent * restrict fse)
{
  llist_delete(&fse->unmount_lln);
  llist_push(&fsent_unmountlist, &fse->unmount_lln);
}

void fsent_unlink_candidate(fsent * restrict fse)
{
  RUNTIME_ASSERT(fsent_possibly_primary(fse));

  llist_delete(&fse->unlink_lln);
  llist_push(&fsent_unlinklist, &fse->unlink_lln);
}

/* preorder */
static void refresh_namespace_visitor(
    moria_vertex *v
  , void *arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *fse;
  const fstree *fst;
  graph_invalidation_context * restrict invalidation;
  namespace *ns;
  moria_foreach_state st;
  moria_edge *e;

  fse = containerof(v, typeof(*fse), vertex);
  invalidation = arg;
  fst = fse->fst;

  /* namespaces to which this fstree is associated */
  moria_foreach_edge_up(&st, &fst->vertex, e) {
    if(edge_type_get(e) != EDGE_FSTREE_NS) {
      continue;
    }

    ns = containerof(e->A, typeof(*ns), vertex);
    namespace_put(ns, fse, invalidation);
  }
}

void fsent_namespace_refresh(graph_invalidation_context * restrict invalidation)
{
  /* preorder traversal of all directories and symlinks from the root */
  moria_traverse_vertices(
      &g_graph
    , &g_root->vertex
    , refresh_namespace_visitor
    , 0
    , (moria_traversal_criteria[]) {{
        /* traverse the fstree graph (directories) */
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        /* visit directories and symlinks */
        , vertex_visit_mask : VERTEX_FILETYPE_DIR_OR_SYMLINK
        , vertex_visit : VERTEX_FILETYPE_DIR_OR_SYMLINK
        /* including specifically the root directory node */
        , min_depth : 0
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , invalidation
  );
}

/* preorder */
static void refresh_fstree_visitor(
    moria_vertex *v
  , void *arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *parent;
  fsent *fse;
  const fstree *fst;

  fse = containerof(v, typeof(*fse), vertex);
  parent = fsent_parent(fse);

  /* this is effective for all nodes under the root to receive the updated fst assignment
   * because, on cleanup, fstrees reset base->fst to point at fstree_root */
  if(fse == g_root) {
    /* root fst assignment is stable */
  } else if(fse->fst == fstree_root || fse->fstree_epoch != fsent_fstree_epoch) {
    fse->fst = parent->fst;
  }
  fst = fse->fst;

  filesystem_stats.fstree_refresh++;

  /* filesystem notify transitions */
  if(fst->invalidation == INVALIDATION_NOTIFY && fst->mount) {
    notify_add_watch(fse);
  } else {
    notify_rm_watch(fse);
  }
}

void fsent_fstree_refresh(graph_invalidation_context * restrict invalidation)
{
  fsent *fse;
  const fstree *fst;
  llist *lln;

  /* preorder traversal of all directories from the root
   *  fse -> fst assignments
   *  notify transitions */
  moria_traverse_vertices(
      &g_graph
    , &g_root->vertex
    , refresh_fstree_visitor
    , 0
    , (moria_traversal_criteria[]) {{
        /* traverse the fstree graph (directories) */
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        /* visit directories */
        , vertex_visit_mask : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
        /* including specifically the root directory node */
        , min_depth : 0
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , 0
  );

  /* directly watched files are also re-considered */
  llist_foreach_safe(&notify_watched_files, fse, notify_watched_files_lln, lln) {
    fst = fsent_parent(fse)->fst;

    if(fst->invalidation == INVALIDATION_NOTIFY && !fst->mount) {
      continue;
    }

    /* this file either no longer needs to be watched, or it is being watched via its directory */
    notify_rm_watch(fse);
  }
}

struct unlink_error_context {
  const fsent *fse;
  const module *mod;
  const fstree *fst;
  int r;
};

static void unlink_denied_build(bacon_writer * restrict builder_writer, const struct unlink_error_context * restrict ctx)
{
  { MAPPING
    STRING("unlink denied")
    { SET
      vertex_identity_describe(&ctx->fse->vertex, builder_writer);
    }
  }
}

static void unlink_failed_build(bacon_writer * restrict builder_writer, const struct unlink_error_context * restrict ctx)
{
  { MAPPING
    STRING("unlink error")
    { SET
      vertex_identity_describe(&ctx->fse->vertex, builder_writer);
      { MAPPING
        STRING("error")
        STRING(strerror(ctx->r))
      }
    }
  }
}

static void unlink_error(const fsent *fse, const module *mod, const fstree *fst, int r, void (*build)(bacon_writer *, const struct unlink_error_context *))
{
  narrator_fixed fixed;
  bacon_writer writer;
  events_context ectx;
  bamipc_message *msg;
  struct unlink_error_context ctx;

  ctx = (typeof(ctx)) {
    .fse = fse,
    .mod = mod,
    .fst = fst,
    .r = r,
  };

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);

  bacon_writer_open(&writer, &fixed.base);
  build(&writer, &ctx);
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

void fsent_managed_reconcile(graph_invalidation_context * restrict invalidation)
{
  fsent *fse;
  fsent *dir;
  module *mod;
  fstree *fst;
  char path[PATH_MAX];
  size_t path_len;
  int r;

  while((fse = llist_shift(&fsent_unlinklist, typeof(*fse), unlink_lln)))
  {
    if(vertex_type_get(&fse->vertex) == VERTEX_DIR) {
      if(!rbtree_empty(&fse->vertex.up)) {
        continue;
      }
      if(!rbtree_empty(&fse->vertex.down)) {
        continue;
      }
    } else if(!fsent_is_primary(fse)) {
      continue;
    }

    if((mod = vertex_module_of(&fse->vertex)) == 0) {
      continue;
    }
    if(!mod->managed) {
      /* only modules with the managed flag set */
      continue;
    }
    if(!mod->disjoint) {
      /* only modules where the build/anchor dirs are unrelated */
      continue;
    }
    if(!fsent_is_descendant_of(mod->build_dir, fse)) {
      /* only files in the build dir can be considered */
      continue;
    }

    dir = fsent_parent(fse);
    fst = vertex_fstree_of(&fse->vertex);
    RUNTIME_ASSERT(dir);
    RUNTIME_ASSERT(fst);

    /* unlink operation has to be permitted by the fstree */
    if(!fst->unlink) {
      dir->dirstats.unlink_no++;
      mod->module_stats.unlink_no++;
      fst->fstree_stats.unlink_no++;
      filesystem_stats.unlink_no++;

      unlink_error(fse, mod, fst, 0, unlink_denied_build);
      llist_push(&fsent_unlinklist, &fse->unlink_lln);
      break;
    }

    path_len = 0;
    path_len += zcatw(path, sizeof(path), mod->build_dir_abspath, mod->build_dir_abspath_len);
    path_len += zcatc(path + path_len, sizeof(path) - path_len, '/');
    path_len += fsent_relative_path_cat(path + path_len, sizeof(path) - path_len, fse, mod->build_dir);
    path[path_len] = 0;
    if(vertex_type_get(&fse->vertex) == VERTEX_DIR) {
      if((r = rmdir(path)) == 0) {
        dir->dirstats.rmdir_ok++;
        mod->module_stats.rmdir_ok++;
        fst->fstree_stats.rmdir_ok++;
        filesystem_stats.rmdir_ok++;
      } else if(errno == ENOENT) {
        dir->dirstats.rmdir_enoent++;
        mod->module_stats.rmdir_enoent++;
        fst->fstree_stats.rmdir_enoent++;
        filesystem_stats.rmdir_enoent++;

        /* unlinked by some other process ; proceed */
        r = 0;
      } else {
        dir->dirstats.rmdir_err++;
        mod->module_stats.rmdir_err++;
        fst->fstree_stats.rmdir_err++;
        filesystem_stats.rmdir_err++;
      }
    } else {
      if((r = unlink(path)) == 0) {
        dir->dirstats.unlink_ok++;
        mod->module_stats.unlink_ok++;
        fst->fstree_stats.unlink_ok++;
        filesystem_stats.unlink_ok++;
      } else if(errno == ENOENT) {
        dir->dirstats.unlink_enoent++;
        mod->module_stats.unlink_enoent++;
        fst->fstree_stats.unlink_enoent++;
        filesystem_stats.unlink_enoent++;

        /* unlinked by some other process ; proceed */
        r = 0;
      } else {
        dir->dirstats.unlink_err++;
        mod->module_stats.unlink_err++;
        fst->fstree_stats.unlink_err++;
        filesystem_stats.unlink_err++;
      }
    }
    if(r != 0) {
      unlink_error(fse, mod, fst, errno, unlink_failed_build);
      llist_push(&fsent_unlinklist, &fse->unlink_lln);
      break;
    }

    graph_demolish(&fse->vertex);

    /* propagate upwards, unlink now-empty directories */
    llist_delete(&dir->unlink_lln);
    llist_push(&fsent_unlinklist, &dir->unlink_lln);
  }
}

void fsent_primary_reconcile(graph_invalidation_context * restrict invalidation)
{
  fsent *fse;
  llist *head, *ent;

  /* freshen all remaining primary files */
  head = &fsent_freshenlist;
  for(ent = head->next; ent != head; ent = ent->next) {
    fse = containerof(ent, typeof(*fse), freshen_lln);

    if(!fsent_is_primary(fse)) {
      continue;
    }

    vertex_mark_fresh(&fse->vertex);
  }

  while(llist_shift(&fsent_freshenlist, typeof(*fse), freshen_lln));
}

void fsent_fstree_reconcile(graph_invalidation_context * restrict invalidation)
{
  fsent *fse;
  fsent *parent;
  struct vertices unlinking;
  struct vertices releasing;
  struct edges edges;

  /* propagate fstree assignments */
  fsent_fstree_refresh(invalidation);

  /* enumerate unmounted subtrees */
  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);
  while((fse = llist_shift(&fsent_unmountlist, typeof(*fse), unmount_lln))) {
    if(vertex_type_get(&fse->vertex) != VERTEX_DIR) {
      continue;
    }

    /* gather upwards edges to disintegrate until reaching a dirent that
     * is either an fstree base or within a mounted fstree */
    while(1)
    {
      /* in the descendants of this mounted fstree ? */
      if(fse->fst->mount && fsent_is_descendant_of(fse->fst->base, fse)) {
        break;
      }

      /* in the ancestry of any fstree ? */
      if(fstree_in_ancestry(fse)) {
        break;
      }

      vertices_push(&unlinking, &fse->vertex);

      parent = fsent_parent(fse);
      fse = parent;
    }
  }

  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
}

void fsent_promote_dir(fsent * restrict fse)
{
  switch(vertex_filetype_get(&fse->vertex)) {
    case VERTEX_FILETYPE_DIR:
      break;
    case VERTEX_FILETYPE_REG:
      vertex_filetype_set(&fse->vertex, VERTEX_FILETYPE_DIR);
      break;
    default:
      RTA;
  }

  fsent_dir_mkdir_push(fse);
}

void fsent_dir_confirm(fsent * restrict fse)
{
  switch(vertex_filetype_get(&fse->vertex)) {
    case VERTEX_FILETYPE_REG:
      fsent_promote_dir(fse);
    case VERTEX_FILETYPE_DIR:
      break;
    default:
      return;
  }

  fsent_dir_mkdir_cancel(fse);
  vertex_state_set(&fse->vertex, VERTEX_OK);
}

static void fsent_stats_collate(fsent * restrict fse, bool reset)
{
  if(reset) {
    memset(&fse->stats, 0, sizeof(fse->stats));
    memset(&fse->dirstats, 0, sizeof(fse->dirstats));
  }

  fse->stats.indexed_notify = fse->in_wd != -1;

  if(vertex_filetype_get(&fse->vertex) != VERTEX_FILETYPE_DIR) {
    return;
  }

  fse->dirstats.dentries = edges_count_down(&fse->vertex, EDGE_FS);
  fse->dirstats.rules = edges_count_up(&fse->vertex, EDGE_DIR_RMA);
  fse->dirstats.indexed_lookup = !!edges_count_up(&fse->vertex, EDGE_DIR_FSNAME);
}

void fsent_stats_build(fsent * restrict fse, bool reset, struct bacon_writer * restrict builder_writer)
{
  fsent_stats_collate(fse, reset);

  stats_build(&fse->stats.record, &descriptor_stats_vertex, builder_writer);

  if(vertex_filetype_get(&fse->vertex) == VERTEX_FILETYPE_DIR) {
    stats_build(&fse->dirstats.record, &descriptor_stats_fsent_dir, builder_writer);
  } else if(vertex_filetype_get(&fse->vertex) == VERTEX_FILETYPE_SYMLINK) {
    /* populate instantaneous counts */
    fse->symlink_stats.readlink_pending = llist_attached(&fse->fs_readlink_lln);
    stats_build(&fse->symlink_stats.record, &descriptor_stats_fsent_symlink, builder_writer);
  }
}

void fsent_dir_describe(fsent * restrict fse, bacon_writer * restrict builder_writer)
{
  const module *mod;

  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      vertex_mappings(&fse->vertex, builder_writer);

      if((mod = vertex_module_of(&fse->vertex))) {
        MAPPING STRING("module")
        vertex_path_describe(&mod->vertex, builder_writer);
      }

      MAPPING STRING("fstree")
      vertex_path_describe(&vertex_fstree_of(&fse->vertex)->vertex, builder_writer);

      { MAPPING STRING("stats")
        { SET
          vertex_stats_describe(&fse->vertex, false, builder_writer);
        }
      }
      vertex_relations_describe(&fse->vertex, builder_writer);
    }
  }
}

void fsent_describe(fsent * restrict fse, bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      vertex_mappings(&fse->vertex, builder_writer);
      { MAPPING STRING("stats")
        { SET
          vertex_stats_describe(&fse->vertex, false, builder_writer);
        }
      }
      vertex_relations_describe(&fse->vertex, builder_writer);
    }
  }
}

void fsent_id(const fsent * restrict fse, const char ** restrict id, uint16_t * restrict idl)
{
  *id = fse->id;
  *idl = fse->id_len;
}

module *fsent_is_build_mod_file(const fsent * restrict fse)
{
  struct moria_edge *edge;
  struct module *mod;

  if((edge = edges_check_up(&fse->vertex, EDGE_FSENT_RULESET))) {
    if((edge = edges_check_up(edge->A, EDGE_RULESET_BUILD_MOD))) {
      mod = containerof(edge->A, typeof(*mod), vertex);
      return mod;
    }
  }

  return 0;
}

formula *fsent_is_formula_file(const fsent * restrict fse)
{
  struct moria_edge *edge;
  struct formula *fml;

  if((edge = edges_check_up(&fse->vertex, EDGE_FSENT_FML))) {
    fml = containerof(edge->A, typeof(*fml), vertex);
    return fml;
  }

  return 0;
}

varset *fsent_is_varset_file(const fsent * restrict fse)
{
  struct moria_edge *edge;
  struct varset *var;

  if((edge = edges_check_up(&fse->vertex, EDGE_FSENT_VARSET))) {
    var = containerof(edge->A, typeof(*var), vertex);
    return var;
  }

  return 0;
}

configset *fsent_is_configset_file(const fsent * restrict fse)
{
  struct moria_edge *edge;
  struct configset *cfg;

  if((edge = edges_check_up(&fse->vertex, EDGE_FSENT_CONFIGSET))) {
    cfg = containerof(edge->A, typeof(*cfg), vertex);
    return cfg;
  }

  return 0;
}

ruleset *fsent_is_ruleset_file(const fsent * restrict fse)
{
  struct moria_edge *edge;
  struct ruleset *rules;

  if((edge = edges_check_up(&fse->vertex, EDGE_FSENT_RULESET))) {
    rules = containerof(edge->A, typeof(*rules), vertex);
    return rules;
  }

  return 0;
}

#if CONFIG_DEVEL
void fsents_show()
{
  fsent *fse;
  narrator_fd nfd;
  int x = 0;

  narrator_fd_init(&nfd, 1);

  printf("fsents, live : %zu free : %zu\n", llist_count(&fsent_list), llist_count(&fsent_freelist));
  llist_foreach(&fsent_list, fse, vertex.owner) {
    dprintf(nfd.fd, "%3u %p", x++, fse);
    dprintf(nfd.fd, "\n");
  }
}
#endif
