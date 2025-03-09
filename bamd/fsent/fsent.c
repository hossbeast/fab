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
#include "bacon/value.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "valyria/hashtable.h"
#include "common/hash.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"

#include "fsent.h"
#include "vertex.h"
#include "config.h"
#include "configset.h"
#include "dependency.h"
#include "events.h"
#include "fstree.h"
#include "formula.h"
#include "fsedge.h"
#include "channel.h"
#include "module.h"
#include "params.h"
#include "rule_module_association.h"
#include "rule_system.h"
#include "dir_rma_edge.h"
#include "rule.h"
#include "shadow.h"
#include "stats.h"
#include "varset.h"
#include "channel.h"
#include "vertex/probes.h"
#include "ruleset.h"
#include "lookup.h"
#include "selection.h"
#include "special_edge.h"
#include "edge.h"
#include "meta_edge.h"
#include "entlist.h"
#include "walker.h"
#include "namespace.h"
#include "plugin.h"
#include "buildplan_entity.h"
#include "config_system.h"
#include "ns_edge.h"
#include "notify.h"

#include "common/attrs.h"
#include "zcat.h"
#include "marshal.h"

// special fsent names
#if CONFIG_DEVEL
#define FSENT_NAME_MODULE "module.bam+devel"
#define FSENT_NAME_RULES  "rules.bam+devel"
#define FSENT_NAME_VAR    "var.bam+devel"
#define FSENT_NAME_CONFIG "config.bam+devel"
#else
#define FSENT_NAME_MODULE "module.bam"
#define FSENT_NAME_RULES  "rules.bam"
#define FSENT_NAME_VAR    "var.bam"
#define FSENT_NAME_CONFIG "config.bam"
#endif

const char *fsent_name_module = FSENT_NAME_MODULE;
const char *fsent_name_rules = FSENT_NAME_RULES;
const char *fsent_name_var = FSENT_NAME_VAR;
const char *fsent_name_config = FSENT_NAME_CONFIG;

uint16_t fsent_module_epoch = 1;
uint16_t fsent_fstree_epoch;
uint16_t fsent_account_epoch;

llist fsent_list = LLIST_INITIALIZER(fsent_list);                       // active fsents
static llist fsent_freelist = LLIST_INITIALIZER(fsent_freelist);        // free fsents
static llist fsent_prunelist = LLIST_INITIALIZER(fsent_prunelist);      // fsent prune candidates
static llist fsent_freshenlist = LLIST_INITIALIZER(fsent_freshenlist);  // fsent freshen candidates

fsent *g_root;
fsent *g_base;
char g_base_dir[PATH_MAX] = "/base";
uint16_t g_base_dir_len;
int g_base_dirfd;

attrs32 * fsent_property_attrs = (attrs32[]) {{
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
  llist_delete(&n->vertex.owner);
  llist_append(&fsent_freelist, &n->vertex.owner);
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

  if((n = llist_shift(&fsent_freelist, typeof(*n), vertex.owner)) == 0) {
    xmalloc(&n, sizeof(*n));
  } else {
    memset(n, 0, sizeof(*n));
  }

  fsent_name_init(&n->name, label, label_len);
  moria_vertex_initw(&n->vertex, &g_graph, attrs | kind | VERTEX_KIND_FSENT, n->name.name, n->name.namel);

  n->in_epoch = -1;
  n->in_wd = -1;
  n->mod = 0;

  n->fst = &fstree_root;

  llist_init(&n->pending.lln);
  n->pending.kind = SWEEPER_EVENT_SELF;
  rbtree_init(&n->pending_child);
  llist_init(&n->fs_reconcile_lln);
  llist_init(&n->prune_lln);
  llist_init(&n->freshen_lln);
  llist_init(&n->notify_watched_files_lln);
  llist_init(&n->bpe_reconcile_lln);

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

void fsent_compute_identifiers(fsent * restrict fse)
{
  const fsent *parent;

  if(fse->path_hash == 0) {
    parent = fsent_parent(fse);
    fse->path_hash = vertex_compute_hash(&fse->vertex, parent->path_hash);
  }

  fse->id_len = zcatf(fse->id, sizeof(fse->id), "%016"PRIx64, fse->path_hash);
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
  llist_delete(&n->fs_reconcile_lln);
  llist_delete(&n->prune_lln);
  llist_delete(&n->freshen_lln);
  llist_delete(&n->notify_watched_files_lln);
  llist_delete(&n->bpe_reconcile_lln);

  vertex_deleted_probe(&n->vertex);
  fsent_dispose(n);
}

//
// public
//

/* create the root fsent (g_root) */
void fsent_root_setup()
{
  fsent * n = 0;

  fsent_alloc(&n, VERTEX_DIR, VERTEX_PROTECT_BIT | VERTEX_EXISTS_BIT, MMS("(root)"));

  n->fst = &fstree_root;
  n->path_hash = 0xface;
  fsent_compute_identifiers(n);

  g_root = n;
}

void fsent_base_setup()
{
  fsent_graft(g_base_dir, &g_base);
}

void fsent_cleanup()
{
  llist *T;
  fsent *n;

  llist_foreach_safe(&fsent_list, n, vertex.owner, T) {
    fsent_dispose(n);
  }

  llist_foreach_safe(&fsent_freelist, n, vertex.owner, T) {
    free(n);
  }
}

void fsent_reconfigure(config * restrict cfg, bool dryrun)
{
  return;
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
      fsent_create(&child, VERTEX_DIR, VERTEX_OK | VERTEX_PROTECT_BIT, seg, end - seg);

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
  if(property == FSENT_PROPERTY_NAME)
    z += zcatw(dst + z, sz - z, n->name.name, n->name.namel);
  else if(property == FSENT_PROPERTY_EXT)
    z += zcatw(dst + z, sz - z, n->name.ext, n->name.extl);
  else if(property == FSENT_PROPERTY_SUFFIX)
    z += zcatw(dst + z, sz - z, n->name.suffix, n->name.suffixl);
  else if(property == FSENT_PROPERTY_BASE)
    z += zcatw(dst + z, sz - z, n->name.base, n->name.basel);
  else if(property == FSENT_PROPERTY_ABSDIR || property == FSENT_PROPERTY_RELDIR)
  {
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
  }
  else if(property == FSENT_PROPERTY_ABSPATH || property == FSENT_PROPERTY_RELPATH)
  {
    if(property == FSENT_PROPERTY_ABSPATH || vertex_shadow_get(&n->vertex))
    {
      z += fsent_absolute_path_cat(dst + z, sz - z, n);
    }
    else
    {
      z += fsent_relative_path_cat(dst + z, sz - z, n, ctx->cwd);
    }
  }
  else if(property == FSENT_PROPERTY_STATE)
  {
    attrs = n->vertex.attrs & VERTEX_STATE_OPT;
    if(attrs)
    {
      z += attrs32_zcat(dst + z, sz - z, graph_vertex_attrs, attrs);
    }
  }
  else if(property == FSENT_PROPERTY_TYPE)
  {
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

fsent *fsent_path_lookup(
    const fsent * restrict base
  , const struct module * restrict mod
  , const char * restrict path
  , uint16_t path_len
  , bool nofollow
)
{
  uint16_t label_len;
  const moria_vertex *v;
  const moria_vertex *next;

  if(path_len >= 2 && memcmp(path, "//", 2) == 0) {
    v = &g_shadow->vertex;
    path_len -= 2;
    path += 2;
  } else if(path_len >= 1 && path[0] == '/') {
    v = &g_root->vertex;
    path += 1;
    path_len -= 1;
  } else {
    v = &base->vertex;
  }

  while(v && path_len)
  {
    label_len = 0;
    while(path[label_len] != '/' && label_len != path_len) {
      label_len++;
    }

    if(memncmp(path, label_len, ".", 1) != 0)
    {
      if(memncmp(path, label_len, "..", 2) == 0)
      {
        if((v = moria_vertex_up(v)) == 0) {
          break;
        }
      }
      else
      {
        if((v = moria_vertex_downw(v, path, label_len)) == 0) {
          break;
        }
      }

      next = v;
      do {
        v = next;
        switch(vertex_type_get(v)) {
          case VERTEX_CONTEXT_MODULE:
            if(mod) {
              next = &mod->shadow->vertex;
            }
            break;
          case VERTEX_SHADOW_LINK:
            /* this particular link is always followed */
            if(vertex_type_get(v->ref) == VERTEX_CONTEXT_MODULE) {
              next = v->ref;
            } else if(!nofollow) {
              next = v->ref;
            }
          default:
            break;
        }
      } while(next != v);
    }

    if(path[label_len] == '/') {
      label_len++;
    }

    path += label_len;
    path_len -= label_len;
  }

  if(path_len == 0 && v != NULL) {
    return containerof(v, fsent, vertex);
  }

  return 0;
}

void fsent_formula_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  formula *fml;
  uint64_t u64;

  if(vertex_type_get(&fse->vertex) == VERTEX_FORMULA_FILE) {
    return;
  }

  formula_create(&fml, &g_graph);
  fsent_absolute_path_cat(fml->abspath, sizeof(fml->abspath), fse);
  fml->abspath_len = strlen(fml->abspath);

  // realpath hash
  u64 = hash64(0, fml->abspath, fml->abspath_len);
  fml->id_len = zcatf(fml->id, sizeof(fml->id), "%016"PRIx64, u64);
  fml->vertex.label = fml->id;
  fml->vertex.label_len = fml->id_len;

  fml->self_node = fse;
  fse->self_fml = fml;
  special_connect(&fse->vertex, &fml->vertex, invalidation);

  /* shadow link to the special node itself */
  shadow_link(g_shadow_formulas, fml->id, fml->id_len, &fml->vertex, &fml->link);
  vertex_type_set(&fse->vertex, VERTEX_FORMULA_FILE);
}

void fsent_varset_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  struct varset *var;
  uint64_t u64;

  if(vertex_type_get(&fse->vertex) == VERTEX_VARSET_FILE) {
    return;
  }

  varset_alloc(&var, &g_graph);
  fsent_absolute_path_cat(var->self_node_abspath, sizeof(var->self_node_abspath), fse);
  var->self_node_abspath_len = strlen(var->self_node_abspath);

  // realpath hash
  u64 = hash64(0, var->self_node_abspath, var->self_node_abspath_len);
  var->id_len = zcatf(var->id, sizeof(var->id), "%016"PRIx64, u64);
  var->vertex.label = var->id;
  var->vertex.label_len = var->id_len;

  fse->self_var = var;
  var->self_node = fse;
  special_connect(&fse->vertex, &var->vertex, invalidation);

  /* shadow link to the special node itself */
  shadow_link(g_shadow_varsets, var->id, var->id_len, &var->vertex, &var->link);
  vertex_type_set(&fse->vertex, VERTEX_VARSET_FILE);
}

void fsent_configset_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  configset *cfg;
  uint64_t u64;

  if(vertex_type_get(&fse->vertex) == VERTEX_CONFIGSET_FILE) {
    return;
  }

  configset_alloc(&cfg);
  fsent_absolute_path_cat(cfg->self_node_abspath, sizeof(cfg->self_node_abspath), fse);
  cfg->self_node_abspath_len = strlen(cfg->self_node_abspath);

  // realpath hash
  u64 = hash64(0, cfg->self_node_abspath, cfg->self_node_abspath_len);
  cfg->id_len = zcatf(cfg->id, sizeof(cfg->id), "%016"PRIx64, u64);
  cfg->vertex.label = cfg->id;
  cfg->vertex.label_len = cfg->id_len;

  fse->self_configset = cfg;
  cfg->self_node = fse;
  special_connect(&fse->vertex, &cfg->vertex, invalidation);

  /* shadow link to the special node itself */
  shadow_link(g_shadow_configsets, cfg->id, cfg->id_len, &cfg->vertex, &cfg->link);
  vertex_type_set(&fse->vertex, VERTEX_CONFIGSET_FILE);
}

void fsent_plugin_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  struct plugin *plugin;
  uint64_t u64;

  if(vertex_type_get(&fse->vertex) == VERTEX_PLUGIN_FILE) {
    return;
  }

  plugin_alloc(&plugin);
  fsent_absolute_path_cat(plugin->self_node_abspath, sizeof(plugin->self_node_abspath), fse);
  plugin->self_node_abspath_len = strlen(plugin->self_node_abspath);

  // realpath hash
  u64 = hash64(0, plugin->self_node_abspath, plugin->self_node_abspath_len);
  plugin->id_len = zcatf(plugin->id, sizeof(plugin->id), "%016"PRIx64, u64);
  plugin->vertex.label = plugin->id;
  plugin->vertex.label_len = plugin->id_len;

  fse->self_plugin = plugin;
  plugin->self_node = fse;
  special_connect(&fse->vertex, &plugin->vertex, invalidation);

  /* shadow link to the special node itself */
  shadow_link(g_shadow_plugins, plugin->id, plugin->id_len, &plugin->vertex, &plugin->link);
  vertex_type_set(&fse->vertex, VERTEX_PLUGIN_FILE);
}

uint64_t fsent_abspath_hash(const fsent * restrict fse)
{
  char path[PATH_MAX];
  size_t path_len;
  uint64_t u64;

  // realpath hash
  path_len = fsent_absolute_path_cat(path, sizeof(path), fse);
  u64 = hash64(0, path, path_len);

  return u64;
}

void fsent_unlink(fsent *n, entlist * restrict edges, graph_invalidation_context * restrict invalidation)
{
  event_bacon *msg;
  uint16_t z;
  moria_vertex *v;
  moria_edge *edge;
  moria_foreach_state st;
  fsent *p;
  events_context ectx;

  fsindex_del(n, invalidation);
  llist_delete(&n->fs_reconcile_lln);

  v = &n->vertex;
  if(!vertex_exists_get(v)) {
    /* idempotency */
    return;
  }

  /* RMAs attached to the directory must be re-run */
  if((p = fsent_parent(n))) {
    vertex_invalidate_direct(&fsent_parent(n)->vertex, invalidation);
  } else {
    RUNTIME_ASSERT(vertex_type_get(v) == VERTEX_SHADOW_LINK);
  }

  n->path_hash = 0;
  vertex_invalidate(v, invalidation);

  /* proactive edge disconnect - remove the edges that should not prevent the fsent
   * from actually being deleted, as opposed to be marked as not exists */
  moria_foreach_edge_up(&st, v, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_SPECIAL:
      case EDGE_DIR_RMA:
        entlist_push(edges, edge);
    /* NOTE dependency edges are preserved here. Those will be reaped the next time
     * the rule which created them is run and fails to match the file since it is now
     * VERTEX_UNLINKED */
      default:
        break;
    }
  }

  vertex_state_set(v, VERTEX_UNLINKED);
  vertex_unlinked_probe(&n->vertex);

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_NODE_DELETE))) {
    z = vertex_path_cat(msg->text, sizeof(msg->text), &n->vertex);
    msg->size = z;
    msg->id = 0;
    events_publish(&ectx);
  }
}

fsent *fsent_translate(fsent * fse, const fsent *origin, const fsent *target, graph_invalidation_context * restrict invalidation)
{
  const moria_vertex *v, *sv, *next_v;
  fsent *p, *next_n;
  entlist list, *eln;

  entlist_init(&list);
  for(p = fse; p != g_root; p = fsent_parent(p))
  {
    if(p == origin) {
      break;
    }
    if(p == target) {
      break;
    }

    entlist_unshift(&list, &p->vertex);
  }

  if(p == g_root) {
    /* fse is outside both trees */
    entlist_destroy(&list);
    return fse;
  }

  if(p == target) {
    /* already in context of the target tree */
    entlist_destroy(&list);
    return fse;
  }

  /* convert from origin -> target */
  v = &target->vertex;
  entlist_foreach(&list, sv, eln) {
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

  entlist_destroy(&list);
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

  moria_foreach_edge_down(&st, &fse->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_BPE_TGT:
        return false;
      default:
    }
  }

  return true;
}

void fsent_freshen_candidate(fsent * restrict fse)
{
  if(!fsent_is_primary(fse)) {
    return;
  }

  llist_delete(&fse->freshen_lln);
  llist_push(&fsent_freshenlist, &fse->freshen_lln);
}

void fsent_prune_candidate(fsent * restrict fse)
{
  llist_delete(&fse->prune_lln);
  llist_push(&fsent_prunelist, &fse->prune_lln);
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
  graph_invalidation_context * restrict invalidation;

  fse = containerof(v, typeof(*fse), vertex);
  invalidation = arg;

  parent = fsent_parent(fse);

  /* this is effective for all nodes under the root to receive the updated fst assignment
   * because, on cleanup, fstrees reset base->fst to point at fstree_root */
  if(fse->fst == &fstree_root) {
    fse->fst = parent->fst;
  }
  fst = fse->fst;

  filesystem_stats.fstree_refresh++;

  /* filesystem indexed transitions */
  if(fst->index && !fse->ie) {
    fsindex_put(fse, invalidation);
  } else if(!fst->index && fse->ie) {
    fsindex_del(fse, invalidation);
  }

  /* filesystem notify transitions */
  if(fst->invalidation == INVALIDATION_NOTIFY) {
    if(fst->mount) {
      notify_add_watch(fse);
    }
  } else if(fst->invalidation != INVALIDATION_NOTIFY) {
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
   */
  moria_traverse_vertices(
      &g_graph
    , &g_root->vertex
    , refresh_fstree_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , invalidation
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

uint32_t fsent_invalidate_primary(graph_invalidation_context * restrict invalidation)
{
  fsent *fse;
  uint32_t count;

  count = 0;
  llist_foreach(&fsent_list, fse, vertex.owner) {
    if(vertex_invalid_get(&fse->vertex)) {
      continue;
    }
    if(!fsent_is_primary(fse)) {
      continue;
    }

    count++;
    vertex_invalidate(&fse->vertex, invalidation);
  }

  return count;
}

void fsent_freshen_reconcile()
{
  fsent *fse;
  llist *head, *ent;

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
  moria_edge *edge;
  entlist list;

  /* propagate fstree assignments */
  fsent_fstree_refresh(invalidation);

  /* enumerate unmounted subtrees */
  entlist_init(&list);
  while((fse = llist_shift(&fsent_prunelist, typeof(*fse), prune_lln))) {
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

      parent = fsent_parent(fse);
      edge = moria_edge_between(&fse->vertex, &parent->vertex);
      entlist_push(&list, edge);
      fse = parent;
    }
  }

  /* disintegrate unmounted subtrees */
  fsedge_disintegrate_bulk(&list, invalidation);
  entlist_destroy(&list);
}

/* preorder */
static void refresh_modules_visitor(
    moria_vertex *v
  , void *arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *parent;
  fsent *fse;

  fse = containerof(v, typeof(*fse), vertex);

  /* the root node does not refresh */
  if((parent = fsent_parent(fse)) == 0) {
    return;
  }

  /* propagate fse->mod */
  if(vertex_type_get(&fse->vertex) == VERTEX_DIR) {
    fse->mod = parent->mod;
  }
}

void fsent_modules_reconcile()
{
  /* preorder traversal of directories from the root
   *  fse -> mod assignments
   */
  moria_traverse_vertices(
      &g_graph
    , &g_root->vertex
    , refresh_modules_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , 0
  );
}

void fsent_promote_dir(fsent * restrict fse)
{
  switch(vertex_filetype_get(&fse->vertex)) {
    case VERTEX_FILETYPE_REG:
      vertex_filetype_set(&fse->vertex, VERTEX_FILETYPE_DIR);
    case VERTEX_FILETYPE_DIR:
      break;
    case VERTEX_FILETYPE_LINK:
      RTA;
  }

  if(vertex_type_get(&fse->vertex) != VERTEX_DIR) {
    return;
  }

  filesystem_mkdir_push(fse);
}

static void fsent_stats_collate(fsent * restrict fse, bool reset)
{
  moria_foreach_state st;
  moria_edge *edge;
  stats_fsent_dir *dirstats;

  if(reset) {
    memset(&fse->stats, 0, sizeof(fse->stats));
    memset(&fse->dirstats, 0, sizeof(fse->dirstats));
  }

  fse->stats.indexed_notify = fse->in_wd != -1;

  if(vertex_filetype_get(&fse->vertex) != VERTEX_FILETYPE_DIR) {
    return;
  }

  dirstats = &fse->dirstats;
  dirstats->indexed_lookup = fse->ie != NULL;
  dirstats->rules = 0;
  moria_foreach_edge(&st, &fse->vertex, edge) {
    if(edge_type_get(edge) == EDGE_DIR_RMA) {
      dirstats->rules++;
    }
  }
}

void fsent_stats_build(fsent * restrict fse, bool reset, struct bacon_writer * restrict builder_writer)
{
  fsent_stats_collate(fse, reset);

  stats_build(&fse->stats.record, &descriptor_stats_vertex, builder_writer);

  if(vertex_filetype_get(&fse->vertex) == VERTEX_FILETYPE_DIR) {
    stats_build(&fse->dirstats.record, &descriptor_stats_fsent_dir, builder_writer);
  }
}

void fsent_mappings(const fsent * restrict fse, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;
  bool b;

  vertex_mappings(&fse->vertex, builder_writer);

  if(vertex_filetype_get(&fse->vertex) == VERTEX_FILETYPE_DIR) {
    MAPPING STRING("fstree")
    vertex_path_describe(&vertex_fstree_get(&fse->vertex)->vertex, builder_writer);
    if (fse->ie) {
      { MAPPING STRING("index")
        vertex_path_describe(fse->ie->edge.A, builder_writer);
      }
    }
  }

  /* bpe sources */
  b = false;
  moria_foreach_edge(&st, &fse->vertex, e) {
    switch(edge_type_get(e)) {
      case EDGE_SRC_BPE:
        b = true;
        break;
      default:
    }
    break;
  }
  if(b) {
    { MAPPING STRING("bpe-src")
      { SET
        moria_foreach_edge(&st, &fse->vertex, e) {
          switch(edge_type_get(e)) {
            case EDGE_SRC_BPE:
              if((e->attrs & MORIA_EDGE_HYPER)) {
                vertex_path_describe(e->Alist[0].v, builder_writer);
              } else {
                vertex_path_describe(e->A, builder_writer);
              }
              break;
            default:
          }
        }
      }
    }
  }

  /* bpe targets */
  b = false;
  moria_foreach_edge(&st, &fse->vertex, e) {
    switch(edge_type_get(e)) {
      case EDGE_BPE_TGT:
        b = true;
        break;
      default:
        continue;
    }
    break;
  }
  if(b) {
    { MAPPING STRING("bpe-tgt")
      { SET
        moria_foreach_edge(&st, &fse->vertex, e) {
          switch(edge_type_get(e)) {
            case EDGE_BPE_TGT:
              if((e->attrs & MORIA_EDGE_HYPER)) {
                vertex_path_describe(e->Blist[0].v, builder_writer);
              } else {
                vertex_path_describe(e->B, builder_writer);
              }
              break;
            default:
              continue;
          }
        }
      }
    }
  }
}

void fsent_describe(fsent * restrict fse, bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      fsent_mappings(fse, builder_writer);
      { MAPPING STRING("stats")
        { SET
          fsent_stats_build(fse, false, builder_writer);
        }
      }
    }
  }
}
