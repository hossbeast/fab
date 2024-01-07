/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "bam/ipc.h"
#include "bam/stats.desc.h"
#include "moria/graph.h"
#include "moria/traverse.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "moria/connect.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "valyria/hashtable.h"
#include "common/hash.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "bam/vertex.h"

#include "fsent.h"
#include "vertex.h"
#include "configuration.h"
#include "config.h"
#include "dependency.h"
#include "events.h"
#include "filesystem.internal.h"
#include "formula.h"
#include "fsedge.h"
#include "channel.h"
#include "module.h"
#include "params.h"
#include "rule_module_association.h"
#include "rule_system.h"
#include "dir_rma.h"
#include "rule.h"
#include "shadow.h"
#include "stats.h"
#include "var.h"
#include "channel.h"
#include "vertex/probes.h"
#include "rules.h"
#include "lookup.h"
#include "selection.h"
#include "special_edge.h"
#include "edge.h"
#include "meta_edge.h"
#include "entlist.h"
#include "walker.h"
#include "fsent_index.h"

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

uint16_t fsent_valid_epoch;
uint16_t fsent_module_epoch;
uint16_t fsent_account_epoch;

llist fsent_list = LLIST_INITIALIZER(fsent_list);                   // active fsents
static llist fsent_freelist = LLIST_INITIALIZER(fsent_freelist);    // free fsents
static llist fsent_prunelist = LLIST_INITIALIZER(fsent_prunelist);  // fsent prune candidates

fsent *g_root;
fsent *g_base;
char g_base_path[PATH_MAX];
uint16_t g_base_path_len;
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

  fsname_init(&n->name, label, label_len);
  moria_vertex_initw(&n->vertex, &g_graph, attrs | kind | VERTEX_KIND_FSENT | VERTEX_FSE_BPE, n->name.name, n->name.namel);

  n->in_epoch = -1;
  n->in_wd = -1;
  n->mod = 0;
  n->valid_epoch = fsent_valid_epoch;
  n->life_token = atomic_fetch_inc(&vertex_token_counter);

  /* temp */
  n->fst = &fstree_root;
  n->fs = n->fst->fs;

  llist_init(&n->pending.lln);
  n->pending.kind = SWEEPER_EVENT_SELF;
  rbtree_init(&n->pending_child);
  llist_init(&n->fs_reconcile_lln);
  llist_init(&n->prune_lln);
  llist_init(&n->bpe_reconcile_lln);

  llist_append(&fsent_list, &n->vertex.owner);

  *np = n;
}

/* create the root fsent (g_root) */
static void root_init()
{
  fsent * n = 0;

  fsent_alloc(&n, VERTEX_DIR, VERTEX_PROTECT_BIT | VERTEX_EXISTS_BIT, MMS("(root)"));

  n->fst = &fstree_root;
  n->fs = n->fst->fs;
  n->path_hash = 0xface;
  fsent_compute_identifiers(n);

  g_root = n;
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
  if(n == subject)
    goto end;

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

    if(z)
      z += zcatw_fn(dst + z, sz - z, "/", 1);
    z += zcatw_fn(dst + z, sz - z, n->name.name, n->name.namel);
  }

  ((char*)dst)[z] = 0;

  return z;
}

static void refresh_module(fsent * restrict n)
{
  fsent *parent;

  if(vertex_type_get(&n->vertex) == VERTEX_SHADOW_MODULE) {
    return;
  }

  if(n->module_epoch == fsent_module_epoch) {
    return;
  }

  n->module_epoch = fsent_module_epoch;

  /* module directory has the module cached in the dirnode */
  switch(vertex_type_get(&n->vertex)) {
    case VERTEX_MODULE_DIR:
      return;
    default:
  }

  /* the root node does not refresh */
  if((parent = fsent_parent(n)) == 0) {
    return;
  }

  refresh_module(parent);
  n->mod = parent->mod;
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
  fse->token_len = zcatf(fse->token, sizeof(fse->token), "%016"PRIx64, hash64(fse->path_hash, MM(fse->life_token)));
}

void fsent_release(fsent * restrict n)
{
  moria_vertex *v;

  /* should be fully excised from the graph previously */
  v = &n->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  //if(vertex_filetype_get(v) == VERTEX_FILETYPE_DIR && vertex_shadowtype_get(v) == 0 && n->wd != -1) {
  //  notify_thread_rm_watch(n);
  //}
  notify_thread_rm_watch(n);
  llist_delete(&n->fs_reconcile_lln);
  llist_delete(&n->prune_lln);
  llist_delete(&n->bpe_reconcile_lln);

  vertex_deleted_probe(&n->vertex);
  fsent_dispose(n);
}

//
// public
//

void fsent_setup()
{
  // setup basic nodes
  root_init();

  fsent_valid_epoch = 1;
  fsent_module_epoch = 1;
}

/* create g_base and attach it to the root */
void fsent_system_bootstrap()
{
  graph_invalidation_context invalidation = { 0 };

  graph_invalidation_begin(&invalidation);

  fsent_graft(g_base_path, &g_base, &invalidation);
  g_base_dirfd = xopens(O_PATH | O_DIRECTORY, g_base_path);

  graph_invalidation_end(&invalidation);
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

void fsent_reconfigure(configuration * restrict cfg, bool dryrun)
{
  return;
}

void fsent_graft(const char * restrict base, fsent ** restrict rn, graph_invalidation_context * restrict invalidation)
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

      r = moria_preconnect(&ctx, &g_graph, &child->vertex, &parent->vertex, EDGE_FSTREE, 0);
      RUNTIME_ASSERT(r == MORIA_NOEDGE);
      fsedge_alloc(&fse);
      moria_connect(&ctx, &g_graph, &fse->edge, &child->vertex, &parent->vertex, EDGE_FSTREE);

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

const struct filesystem *fsent_filesystem_get(fsent * restrict n)
{
  enum vertex_filetype filetype;

  filetype = vertex_filetype_get(&n->vertex);
  if(filetype != VERTEX_FILETYPE_DIR) {
    n = fsent_parent(n);
  }

  RUNTIME_ASSERT(n->fs);
  return n->fs;
}

struct module *fsent_module_get(const fsent * restrict n)
{
  enum vertex_filetype filetype;
  const moria_vertex *v;

  v = &n->vertex;
  while(vertex_type_get(v) == VERTEX_SHADOW_LINK) {
    v = v->ref;
    if(!vertex_exists_get(v)) {
      return 0; // dangling link
    }
  }
  n = containerof(v, typeof(*n), vertex);

  filetype = vertex_filetype_get(&n->vertex);
  if(filetype != VERTEX_FILETYPE_DIR) {
    n = fsent_parent(n);
  }

  refresh_module((fsent*)n);

  return n->mod;
}

size_t fsent_absolute_path_cat(void * restrict dst, size_t sz, const fsent * restrict n)
{
  size_t z = 0;
  const fsent *base;

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
  else if(vertex_shadowtype_get(&n->vertex))
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
  if(vertex_shadowtype_get(&n->vertex))
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
  const filesystem *fs;
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

    if(property == FSENT_PROPERTY_ABSDIR || vertex_shadowtype_get(&n->vertex))
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
    if(property == FSENT_PROPERTY_ABSPATH || vertex_shadowtype_get(&n->vertex))
    {
      z += fsent_absolute_path_cat(dst + z, sz - z, n);
    }
    else
    {
      z += fsent_relative_path_cat(dst + z, sz - z, n, ctx->cwd);
    }
  }
  else if(property == FSENT_PROPERTY_FSROOT)
  {
    fs = fsent_filesystem_get((fsent*)n);
    z += filesystem_absolute_path_cat(dst + z, sz - z, fs);
  }
  else if(property == FSENT_PROPERTY_INVALIDATION)
  {
    fs = fsent_filesystem_get((fsent*)n);
    z += attrs16_zcat(dst + z, sz - z, invalidate_attrs, fs->invalidate);
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

    if(memncmp(path, label_len, ".", 1) == 0)
    {

    }
    else if(memncmp(path, label_len, "..", 2) == 0)
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

      while(!nofollow && vertex_type_get(v) == VERTEX_SHADOW_LINK) {
        v = v->ref;
      }
      if(vertex_shadowtype_get(v) == VERTEX_SHADOWTYPE_CONTEXT_MODULE) {
        if(mod) {
          v = &mod->shadow->vertex;
        } else {
          v = 0;
        }
      }
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
  uint64_t token;

  if(vertex_type_get(&fse->vertex) == VERTEX_FORMULA_FILE) {
    return;
  }
  vertex_type_set(&fse->vertex, VERTEX_FORMULA_FILE);

  formula_create(&fml, &g_graph);
  fsent_absolute_path_cat(fml->abspath, sizeof(fml->abspath), fse);
  fml->abspath_len = strlen(fml->abspath);

  // realpath hash
  u64 = hash64(0, fml->abspath, fml->abspath_len);
  fml->id_len = zcatf(fml->id, sizeof(fml->id), "%016"PRIx64, u64);
  token = atomic_fetch_inc(&vertex_token_counter);
  fml->token_len = zcatf(fml->token, sizeof(fml->token), "%016"PRIx64, hash64(u64, MM(token)));

  fml->self_node = fse;
  fse->self_fml = fml;
  special_connect(&fse->vertex, &fml->vertex, EDGE_FML_SPECIAL, invalidation);
}

void fsent_var_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  struct var *var;
  uint64_t u64;
  uint64_t token;

  if(vertex_type_get(&fse->vertex) == VERTEX_VAR_FILE) {
    return;
  }
  vertex_type_set(&fse->vertex, VERTEX_VAR_FILE);

  var_alloc(&var, &g_graph);
  fsent_absolute_path_cat(var->self_node_abspath, sizeof(var->self_node_abspath), fse);
  var->self_node_abspath_len = strlen(var->self_node_abspath);

  // realpath hash
  u64 = hash64(0, var->self_node_abspath, var->self_node_abspath_len);
  var->id_len = zcatf(var->id, sizeof(var->id), "%016"PRIx64, u64);
  token = atomic_fetch_inc(&vertex_token_counter);
  var->token_len = zcatf(var->token, sizeof(var->token), "%016"PRIx64, hash64(u64, MM(token)));

  fse->self_var = var;
  var->self_node = fse;
  special_connect(&fse->vertex, &var->vertex, EDGE_VAR_SPECIAL, invalidation);
}

void fsent_config_bootstrap(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  config *cfg;
  uint64_t u64;
  uint64_t token;

  if(vertex_type_get(&fse->vertex) == VERTEX_CONFIG_FILE) {
    return;
  }
  vertex_type_set(&fse->vertex, VERTEX_CONFIG_FILE);

  config_alloc(&cfg);
  fsent_absolute_path_cat(cfg->self_node_abspath, sizeof(cfg->self_node_abspath), fse);
  cfg->self_node_abspath_len = strlen(cfg->self_node_abspath);

  // realpath hash
  u64 = hash64(0, cfg->self_node_abspath, cfg->self_node_abspath_len);
  cfg->id_len = zcatf(cfg->id, sizeof(cfg->id), "%016"PRIx64, u64);
  token = atomic_fetch_inc(&vertex_token_counter);
  cfg->token_len = zcatf(cfg->token, sizeof(cfg->token), "%016"PRIx64, hash64(u64, MM(token)));

  fse->self_config = cfg;
  cfg->self_node = fse;
  special_connect(&fse->vertex, &cfg->vertex, EDGE_CONFIG_SPECIAL, invalidation);
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
  bamipc_message *msg;
  uint16_t z;
  channel *chan;
  moria_vertex *v;
  moria_edge *edge;
  moria_foreach_state st;
  fsent *p;

  fsent_index_del(n, invalidation);
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
      case EDGE_MODULE_SPECIAL:
      case EDGE_RULES_SPECIAL:
      case EDGE_VAR_SPECIAL:
      case EDGE_CONFIG_SPECIAL:
      case EDGE_FML_SPECIAL:
      case EDGE_DIR_RMA:
        entlist_push(edges, edge);
    /* NOTE dependency edges are preserved here. Those will be reaped the next time
     * the rule which created them is run and fails to match the file since it is now
     * VERTEX_UNLINKED */
      default:
    }
  }

  /* also remove related edges, e.g. //module/{imports,uses} */
  if(n->shadow_imports_edge) {
    entlist_push(edges, n->shadow_imports_edge);
  }
  if(n->shadow_uses_edge) {
    entlist_push(edges, n->shadow_uses_edge);
  }
  if(n->shadow_rules_edge) {
    entlist_push(edges, n->shadow_rules_edge);
  }
  if(n->shadow_var_edge) {
    entlist_push(edges, n->shadow_var_edge);
  }

  if(v->attrs & VERTEX_SHIELD_BIT) {
    vertex_state_set(v, VERTEX_UNCREATED);
  } else {
    vertex_state_set(v, VERTEX_UNLINKED);
  }

  vertex_unlinked_probe(&n->vertex);

  if(events_would(BAMIPC_EVENT_NODE_DELETE, &chan, &msg))
  {
    z = vertex_path_load(msg->text, sizeof(msg->text), &n->vertex);
    msg->size = z;
    msg->id = 0;
    events_publish(chan, msg);
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

size_t fsent_dir_stats_collate(void *dst, size_t sz, bool reset, fsent * restrict n)
{
  size_t z;
  bam_fsent_dir_stats *stats;
  bam_fsent_dir_stats tmp;
  moria_foreach_state st;
  moria_edge *edge;

  /* base fsent stats */
  z = 0;
  z += vertex_stats_collate(dst + z, sz - z, reset, &n->vertex);

  /* dirnode specific stats */
  memcpy(&tmp, &n->dirstats, sizeof(tmp));
  if(reset) {
    memset(&n->dirstats, 0, sizeof(n->dirstats));
  }
  stats = &tmp;

  moria_foreach_edge_up(&st, &n->vertex, edge) {
    if(edge_type_get(edge) == EDGE_DIR_RMA) {
      stats->rules++;
    }
  }

  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_fsent_dir_stats, &stats->record, 0);

  return z;
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

bool fsent_is_mounted(const fsent * restrict n)
{
  const fsent *p;
  const walker_entry *entry;

  for(p = n; p; p = fsent_parent(p)) {
    if(p == g_base) {
      return true;
    }

    llist_foreach(&walker_include_entry_list, entry, lln) {
      if(p == entry->n) {
        return true;
      }
    }
  }

  return false;
}

void fsent_prune_candidate(fsent * restrict fse)
{
  llist_delete(&fse->prune_lln);
  llist_push(&fsent_prunelist, &fse->prune_lln);
}

/* returns true if the node is in a referenced subtree */
static bool fsent_account_check(const fsent * restrict p)
{
  const fsent *n;
  const walker_entry *entry;

  n = p;
  if(vertex_type_get(&n->vertex) != VERTEX_DIR) {
    n = fsent_parent(n);
  }

  /* if n is in the ancestry of one of the mount points */
  if(fsent_is_descendant_of(n, g_base)) {
    return true;
  }
  if(fsent_is_descendant_of(n, system_config_node)) {
    return true;
  }
  if(fsent_is_descendant_of(n, user_config_node)) {
    return true;
  }
  llist_foreach(&walker_include_entry_list, entry, lln) {
    if(fsent_is_descendant_of(n, entry->n)) {
      return true;
    }
  }

  /* if n lies within one of the mount points */
  while(n) {
    if(n->account_epoch == fsent_account_epoch) {
      return true;
    }

    n = fsent_parent(n);
  }

  return false;
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
  fsent *n;
  graph_invalidation_context * restrict invalidation;

  n = containerof(v, typeof(*n), vertex);
  invalidation = arg;

  /* the root node does not refresh */
  if((parent = fsent_parent(n)) == 0) {
    return;
  }

  STATS_INC(g_stats.fstree_refresh);

  n->fst = 0;
  n->fs = 0;
  if(parent->fst) {
    if((n->fst = fstree_down((struct fstree *)parent->fst, n->name.name, n->name.namel))) {
      n->fs = n->fst->fs;
    }
  }

  if(!n->fs) {
    n->fs = parent->fs;
  }

  /* filesystem indexed transitions */
  if(n->fs->indexed && !n->ie) {
    fsent_index_put(n, invalidation);
  } else if(!n->fs->indexed && n->ie) {
    fsent_index_del(n, invalidation);
  }

  /* filesystem notify transitions */
  if(n->fs->invalidate == INVALIDATE_NOTIFY) {
    if(fsent_is_mounted(n)) {
      notify_thread_add_watch(n);
    }
  } else if(n->fs->invalidate != INVALIDATE_NOTIFY) {
    notify_thread_rm_watch(n);
  }
}

void fsent_fstree_refresh(graph_invalidation_context * restrict invalidation)
{
  /* preorder traversal of directories from the root
   *  fse -> fst assignments
   *  fse -> module assignments
   */
  moria_traverse_vertices(
      &g_graph
    , &g_root->vertex
    , refresh_fstree_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FSTREE
        , edge_visit : EDGE_FSTREE
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , invalidation
  );
}

void fsent_reconcile(graph_invalidation_context * restrict invalidation)
{
  fsent *fse;
  fsent *parent;
  walker_entry *entry;
  moria_edge *edge;
  entlist list;

  /* mark mounted trees */
  fsent_account_epoch++;
  g_base->account_epoch = fsent_account_epoch;
  entry = llist_first(&walker_include_entry_list, typeof(*entry), lln);
  while(entry) {
    entry->n->account_epoch = fsent_account_epoch;
    entry = llist_next(&walker_include_entry_list, entry, lln);
  }

  /* list of unrefeferenced subtrees */
  entlist_init(&list);
  while((fse = llist_shift(&fsent_prunelist, typeof(*fse), prune_lln))) {
    if(vertex_type_get(&fse->vertex) != VERTEX_DIR) {
      continue;
    }
    if(fsent_account_check(fse)) {
      continue;
    }

    /* traverse upwards to the root of the unreferenced subtree */
    do {
      parent = fsent_parent(fse);
      edge = moria_edge_between(&fse->vertex, &parent->vertex);

      entlist_push(&list, edge);
      fse = parent;
    } while(!fsent_account_check(fse));
  }

  /* disintegrate */
  fsedge_disintegrate_bulk(&list, invalidation);
  entlist_destroy(&list);

  fsent_fstree_refresh(invalidation);
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
