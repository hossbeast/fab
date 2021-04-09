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

#include "fab/ipc.h"
#include "moria/graph.h"
#include "narrator.h"
#include "value.h"
#include "valyria/hashtable.h"
#include "valyria/map.h"
#include "xlinux/xstdlib.h"
#include "yyutil/box.h"

#include "fsent.h"
#include "CONFIG.errtab.h"
#include "config.internal.h"
#include "dependency.h"
#include "events.h"
#include "filesystem.internal.h"
#include "formula.h"
#include "fsedge.h"
#include "handler.h"
#include "logging.h"
#include "module.internal.h"
#include "params.h"
#include "rule_module.h"
#include "rule_system.h"
#include "shadow.h"
#include "stats.h"
#include "var.h"
#include "variant.h"

#include "common/attrs.h"
#include "zbuffer.h"

static uint16_t fsent_fs_epoch;
uint16_t fsent_valid_epoch;
uint16_t fsent_module_epoch;

const char *fsent_model_name;
uint16_t fsent_model_name_len;
const char *fsent_module_name = "module.bam";
uint16_t fsent_module_name_len = 10;
const char *fsent_var_name = "model.bam";
uint16_t fsent_var_name_len = 9;
const char *fsent_formula_suffix;
uint16_t fsent_formula_suffix_len;

llist fsent_list = LLIST_INITIALIZER(fsent_list);                 // active fsents
static llist fsent_freelist = LLIST_INITIALIZER(fsent_freelist);  // free fsents

map * g_fsent_by_wd;
fsent * g_root;
fsent * g_project_root;
fsent * g_project_self;
fsent * g_project_shadow;

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

static xapi __attribute__((nonnull(1, 2))) node_invalidate_visitor_first(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  *result = MORIA_TRAVERSE_PRUNE;
  fatal(fsent_invalidate_visitor, v, arg, mode, distance, 0);

  finally : coda;
}

/*
 * visit rules attached to a directory, queue up their rma for re-evaluation
 */
static xapi children_changed_visitor(moria_edge * e, void * ctx, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  rule_dirnode_edge *rde;
  rule_module_edge *rme;
  graph_invalidation_context *invalidation;

  RUNTIME_ASSERT(e->attrs == EDGE_RULE_DIR);
  rde = containerof(e, typeof(*rde), edge);
  rme = rde->rme;
  rule_system_rma_enqueue(rme);

  invalidation = ctx;
  invalidation->any = true;

  finally : coda;
}

static xapi invalidate_fml_rule_visitor(moria_edge * e, void * arg, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  graph_invalidation_context * invalidation = arg;
  rule_module_edge *rme;
  dependency *ne;
  fsent *n;
  int x;

  /* the formula attached to the rule for this rma has been invalidated */
  /* invalidate the targets */
  rme = containerof(e, typeof(*rme), edge);
  llist_foreach(&rme->dependencies, ne, dependencies_lln) {
    e = &ne->edge;
    if(!(e->attrs & MORIA_EDGE_HYPER)) {
      n = containerof(e->A, fsent, vertex);
      fatal(fsent_invalidate, n, invalidation);
    }
    else
    {
      for(x = 0; x < e->Alen; x++)
      {
        n = containerof(e->Alist[x].v, fsent, vertex);
        fatal(fsent_invalidate, n, invalidation);
      }
    }
  }

  finally : coda;
}

static void __attribute__((nonnull)) fsent_dispose(fsent * restrict n)
{
  llist_delete_node(&n->vertex.owner);
  llist_append(&fsent_freelist, n, vertex.owner);
}

/* create the root fsent (g_root) */
static xapi root_init()
{
  enter;

  fsent * n = 0;

  fatal(fsent_create, &n, VERTEX_DIR, 0, MMS("(root)"));

  n->fst = &fstree_root;
  n->fs = n->fst->fs;

  g_root = n;

  finally : coda;
}

/* create the project node (g_project_root) and attach it to the root */
static xapi project_init()
{
  enter;

  graph_invalidation_context invalidation = { 0 };

#if DEVEL
  if(!g_params.proj_dir) {
    goto XAPI_FINALIZE;
  }
#endif

  fatal(graph_invalidation_begin, &invalidation);
  fatal(fsent_graft, g_params.proj_dir, &g_project_root, &invalidation);

finally:
  graph_invalidation_end(&invalidation);
coda;
}

static size_t relative_path_znload(void * restrict dst, size_t sz, const fsent * subject, const fsent * base, size_t (*znloadw_fn)(void * restrict, size_t, const void * restrict, size_t))
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
    z += znloadw_fn(dst + z, sz - z, ".", 1);
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
    if(z)
      z += znloadw_fn(dst + z, sz - z, "/", 1);
    z += znloadw_fn(dst + z, sz - z, "..", 2);
  }

  /* a symlink to the project module */
  if(nsl > 2 && fsent_shadowtype_get(ns[nsl - 1]) == VERTEX_SHADOWTYPE_MODULES && ns[nsl - 2] == g_project_shadow)
  {
    z += znloadw_fn(dst + z, sz - z, "module", 7);
    nsl -= 2;
  }

  /* path from nearest common ancestor to the subject */
  for(x = nsl - 1; x >= 0; x--)
  {
    n = ns[x];

    if(z)
      z += znloadw_fn(dst + z, sz - z, "/", 1);
    z += znloadw_fn(dst + z, sz - z, n->name.name, n->name.namel);
  }

  ((char*)dst)[z] = 0;

  return z;
}

static void refresh_fstree(fsent * restrict n)
{
  fsent * parent;

  if(n->fs_epoch == fsent_fs_epoch)
    return;

  /* the root node does not refresh */
  if((parent = fsent_parent(n)) == 0)
    return;

  STATS_INC(g_stats.fstree_refresh);

  refresh_fstree(parent);

  n->fst = 0;
  if(parent->fst)
  {
    if((n->fst = fstree_down((struct fstree *)parent->fst, n->name.name, n->name.namel)))
    {
      n->fs = n->fst->fs;
    }
  }

  if(!n->fs)
    n->fs = parent->fs;

  n->fs_epoch = fsent_fs_epoch;
}

static void refresh_module(fsent * restrict n)
{
  fsent * parent;

  if(n->module_epoch == fsent_module_epoch)
    return;

  n->module_epoch = fsent_module_epoch;

  if(fsent_kind_get(n) == VERTEX_MODULE_DIR) {
    return;
  }

  /* the root node does not refresh */
  if((parent = fsent_parent(n)) == 0)
    return;

  refresh_module(parent);

  n->mod = parent->mod;
}

//
// internal
//

size_t fsent_relative_path_znload(void * restrict dst, size_t sz, const fsent * subject, const fsent * base)
{
  return relative_path_znload(dst, sz, subject, base, znloadw);
}

size_t fsent_relative_path_znload_bacon(void * restrict dst, size_t sz, const fsent * subject, const fsent * base)
{
  return relative_path_znload(dst, sz, subject, base, value_string_znloadw);
}

xapi fsent_relative_path_say(const fsent * n, const fsent * base, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = fsent_relative_path_znload(path, sizeof(path), n, base);

  xsayw(path, pathl);

  finally : coda;
}

xapi fsent_create(
    fsent ** restrict np
  , vertex_kind kind
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
{
  enter;

  fsent *n;

  RUNTIME_ASSERT(label_len);
  RUNTIME_ASSERT((attrs & VERTEX_KIND_OPT) == 0);

  if((n = llist_shift(&fsent_freelist, typeof(*n), vertex.owner)) == 0)
  {
    fatal(xmalloc, &n, sizeof(*n));
  }

  fsname_init(&n->name, label, label_len);
  moria_vertex_initw(&n->vertex, &g_graph, attrs | kind | VERTEX_TYPE_FSENT, n->name.name, n->name.namel);

  n->notify_epoch = -1;
  n->wd = -1;
  n->not_parsed = 1;
  n->not_loaded = 1;

  llist_init_node(&n->pending.lln);
  n->pending.kind = SWEEPER_EVENT_SELF;
  rbtree_init(&n->pending_child);

  llist_append(&fsent_list, n, vertex.owner);

  *np = n;

  finally : coda;
}

void fsent_release(fsent * restrict n)
{
  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
  v = &n->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  fsent_dispose(n);
}

//
// public
//

xapi fsent_setup_minimal()
{
  enter;

  fatal(map_create, &g_fsent_by_wd);

  finally : coda;
}

xapi fsent_setup()
{
  enter;

  fatal(fsent_setup_minimal);

  // setup basic nodes
  fatal(root_init);
  fatal(project_init);

  fsent_fs_epoch = 1;
  fsent_valid_epoch = 1;
  fsent_module_epoch = 1;

  finally : coda;
}

xapi fsent_cleanup()
{
  enter;

  llist *T;
  fsent *n;

  fatal(map_xfree, g_fsent_by_wd);

  llist_foreach_safe(&fsent_list, n, vertex.owner, T) {
    fsent_dispose(n);
  }

  llist_foreach_safe(&fsent_freelist, n, vertex.owner, T) {
    wfree(n);
  }

  finally : coda;
}

xapi fsent_reconfigure(configblob * restrict cfg, bool dry)
{
  enter;

  if(dry)
  {
    if(cfg->special.module == 0 || cfg->special.module->l == 0) {
      xapi_info_pushs("field", "special.module");
      fail(CONFIG_INVALID);
    }
    if(cfg->special.model == 0 || cfg->special.model->l == 0) {
      xapi_info_pushs("field", "special.model");
      fail(CONFIG_INVALID);
    }
    if(cfg->special.var == 0 || cfg->special.var->l == 0) {
      xapi_info_pushs("field", "special.var");
      fail(CONFIG_INVALID);
    }
    if(cfg->special.formula_suffix == 0 || cfg->special.formula_suffix->l == 0) {
      xapi_info_pushs("field", "special.formula-suffix");
      fail(CONFIG_INVALID);
    }
  }
  else
  {
    if(cfg->filesystems.attrs & CONFIG_CHANGED)
    {
      /* invalidate node -> filesystem assignments */
      fsent_fs_epoch++;
    }

    fsent_module_name = cfg->special.module->v;
    fsent_module_name_len = cfg->special.module->l;
    fsent_model_name = cfg->special.model->v;
    fsent_model_name_len = cfg->special.model->l;
    fsent_var_name = cfg->special.var->v;
    fsent_var_name_len = cfg->special.var->l;
    fsent_formula_suffix = cfg->special.formula_suffix->v;
    fsent_formula_suffix_len = cfg->special.formula_suffix->l;
  }

  finally : coda;
}

xapi fsent_graft(const char * restrict base, fsent ** restrict rn, graph_invalidation_context * restrict invalidation)
{
  enter;

  fsent * parent;
  fsent * child = 0;
  const char *seg;
  const char *end;
  moria_vertex *v;
  int __attribute__((unused)) r;
  moria_connect_context ctx;
  dependency *dep;

  parent = g_root;
  seg = base + 1;
  while(*seg)
  {
    end = seg + 1;
    while(*end && *end != '/')
      end++;

    v = moria_vertex_downw(&parent->vertex, seg, end - seg);
    if(v)
    {
      child = containerof(v, fsent, vertex);
    }
    else
    {
      fatal(fsent_create, &child, VERTEX_DIR, VERTEX_UNCREATED, seg, end - seg);
      r = moria_preconnect(
          &ctx
        , &g_graph
        , &parent->vertex
        , &child->vertex
        , EDGE_FSTREE
        , 0
      );
      RUNTIME_ASSERT(r == MORIA_NOEDGE);
      fatal(dependency_alloc, &dep, &g_graph, 1, 1);
      moria_connect(&ctx, &g_graph, &dep->edge, &parent->vertex, &child->vertex, EDGE_FSTREE);
    }
    parent = child;

    seg = end;
    if(*seg)
      seg++;
  }

  *rn = child;

  finally : coda;
}

const struct filesystem * fsent_filesystem_get(fsent * restrict n)
{
  enum vertex_filetype filetype;

  filetype = fsent_filetype_get(n);
  if(filetype != VERTEX_FILETYPE_DIR)
    n = fsent_parent(n);

  refresh_fstree(n);

  RUNTIME_ASSERT(n->fs);
  return n->fs;
}

struct module * fsent_module_get(const fsent * restrict n)
{
  enum vertex_filetype filetype;

  filetype = fsent_filetype_get(n);
  if(filetype != VERTEX_FILETYPE_DIR)
    n = fsent_parent(n);

  refresh_module((fsent*)n);

  return n->mod;
}

size_t fsent_absolute_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
{
  size_t z = 0;
  const fsent *base;

  if(fsent_shadowtype_get(n))
  {
    z += znloads(dst, sz, "//");
    base = g_shadow;
  }
  else
  {
    z += znloads(dst, sz, "/");
    base = g_root;
  }
  z += fsent_relative_path_znload(dst + z, sz - z, n, base);

  return z;
}

size_t fsent_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
{
  if(fsent_shadowtype_get(n))
  {
    return fsent_absolute_path_znload(dst, sz, n);
  }
  else
  {
    return fsent_project_relative_path_znload(dst, sz, n);
  }
}

size_t fsent_project_relative_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
{
  if(g_project_root == 0)
  {
    /* project not yet initialized */
    return fsent_absolute_path_znload(dst, sz, n);
  }

  return fsent_relative_path_znload(dst, sz, n, g_project_root);
}

size_t fsent_module_relative_path_znload(void * restrict dst, size_t sz, const fsent * restrict n)
{
  const module *mod;

  if((mod = fsent_module_get(n)) == 0) {
    return fsent_absolute_path_znload(dst, sz, n);
  }

  return fsent_relative_path_znload(dst, sz, n, mod->dir_node);
}

size_t fsent_property_znload(void * dst, size_t sz, const fsent * restrict n, fsent_property property, const fsent_property_context * restrict ctx)
{
  const filesystem *fs;
  uint32_t attrs;
  size_t z;

  z = 0;
  if(property == FSENT_PROPERTY_NAME)
    z += znloadw(dst + z, sz - z, n->name.name, n->name.namel);
  else if(property == FSENT_PROPERTY_EXT)
    z += znloadw(dst + z, sz - z, n->name.ext, n->name.extl);
  else if(property == FSENT_PROPERTY_SUFFIX)
    z += znloadw(dst + z, sz - z, n->name.suffix, n->name.suffixl);
  else if(property == FSENT_PROPERTY_BASE)
    z += znloadw(dst + z, sz - z, n->name.base, n->name.basel);
  else if(property == FSENT_PROPERTY_ABSDIR || property == FSENT_PROPERTY_RELDIR)
  {
    if(fsent_filetype_get(n) != VERTEX_FILETYPE_DIR) {
      n = fsent_parent(n);
    }

    if(property == FSENT_PROPERTY_ABSDIR || fsent_shadowtype_get(n))
    {
      z += fsent_absolute_path_znload(dst + z, sz - z, n);
    }
    else
    {
      z += fsent_relative_path_znload(dst + z, sz - z, n, ctx->mod->dir_node);
    }
  }
  else if(property == FSENT_PROPERTY_ABSPATH || property == FSENT_PROPERTY_RELPATH)
  {
    if(property == FSENT_PROPERTY_ABSPATH || fsent_shadowtype_get(n))
    {
      z += fsent_absolute_path_znload(dst + z, sz - z, n);
    }
    else
    {
      z += fsent_relative_path_znload(dst + z, sz - z, n, ctx->mod->dir_node);
    }
  }
  else if(property == FSENT_PROPERTY_VARIANT)
  {
    if(n->var)
    {
      z += znloadw(dst + z, sz - z, n->var->norm, n->var->norm_len);
    }
  }
  else if(property == FSENT_PROPERTY_FSROOT)
  {
    fs = fsent_filesystem_get((fsent*)n);
    z += filesystem_absolute_path_znload(dst + z, sz - z, fs);
  }
  else if(property == FSENT_PROPERTY_INVALIDATION)
  {
    fs = fsent_filesystem_get((fsent*)n);
    z += znload_attrs16(dst + z, sz - z, invalidate_attrs, fs->invalidate);
  }
  else if(property == FSENT_PROPERTY_STATE)
  {
    attrs = n->vertex.attrs & VERTEX_STATE_OPT;
    if(attrs)
    {
      z += znload_attrs32(dst + z, sz - z, graph_vertex_state_attrs, attrs);
    }
  }
  else if(property == FSENT_PROPERTY_TYPE)
  {
    z += znload_attrs32(dst + z, sz - z, graph_vertex_kind_attrs, n->vertex.attrs & VERTEX_KIND_OPT);
  }

  return z;
}

xapi fsent_module_relative_path_say(const fsent * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = fsent_module_relative_path_znload(path, sizeof(path), n);
  xsayw(path, pathl);

  finally : coda;
}

xapi fsent_project_relative_path_say(const fsent * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = fsent_project_relative_path_znload(path, sizeof(path), n);
  xsayw(path, pathl);

  finally : coda;
}

xapi fsent_absolute_path_say(const fsent * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = fsent_absolute_path_znload(path, sizeof(path), n);
  xsayw(path, pathl);

  finally : coda;
}

xapi fsent_path_say(fsent * restrict n, struct narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = fsent_path_znload(path, sizeof(path), n);
  xsayw(path, pathl);

  finally : coda;
}

xapi fsent_property_say(const fsent * restrict n, fsent_property property, const fsent_property_context * restrict ctx, narrator * restrict N)
{
  enter;

  char space[512];
  size_t sz;

  sz = fsent_property_znload(space, sizeof(space), n, property, ctx);
  xsayw(space, sz);

  finally : coda;
}

fsent *fsent_path_lookup(const module * restrict mod, const char * restrict path, uint16_t path_len)
{
  uint16_t label_len;
  moria_vertex *v;

  if(path_len > 2 && memcmp(path, "//", 2) == 0) {
    v = &g_shadow->vertex;
    path_len -= 2;
    path += 2;
  } else if(path_len > 1 && path[0] == '/') {
    v = &g_root->vertex;
    path += 1;
    path_len -= 1;
  } else {
    v = &mod->dir_node->vertex;
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

      if(fsent_shadowtype_get(containerof(v, fsent, vertex)) == VERTEX_SHADOWTYPE_MODULE) {
        v = &mod->shadow->vertex;
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

xapi fsent_formula_bootstrap(fsent * restrict n, bool * restrict reconciled)
{
  enter;

  formula *fml;

  if(fsent_kind_get(n) != VERTEX_FORMULA_FILE) {
    RUNTIME_ASSERT(fsent_kind_get(n) == VERTEX_FILE);
    fsent_kind_set(n, VERTEX_FORMULA_FILE);

    fatal(formula_create, &fml, &g_graph);

    fsent_absolute_path_znload(fml->self_node_abspath, sizeof(fml->self_node_abspath), n);
    fml->abspath_len = strlen(fml->self_node_abspath);

    fml->self_node = n;
    n->self_fml = fml;

    fsent_invalid_set(n);
  }

  fatal(formula_reconcile, n->self_fml, reconciled);

  finally : coda;
}

xapi fsent_var_bootstrap(fsent * restrict n, bool * restrict reconciled)
{
  enter;

  var *vp;

  if(fsent_kind_get(n) != VERTEX_VAR_FILE) {
    RUNTIME_ASSERT(fsent_kind_get(n) == VERTEX_FILE);
    fsent_kind_set(n, VERTEX_VAR_FILE);

    fatal(var_alloc, &vp, &g_graph);

    fsent_absolute_path_znload(vp->self_node_abspath, sizeof(vp->self_node_abspath), n);
    vp->self_node_abspath_len = strlen(vp->self_node_abspath);

    n->self_var = vp;
    vp->self_node = n;
    fsent_invalid_set(n);
  }

  fatal(var_reconcile, n->self_var, reconciled);

  finally : coda;
}

xapi fsent_invalidate(fsent * restrict n, graph_invalidation_context * restrict invalidation)
{
  enter;

  vertex_fsenttype nt;
  vertex_filetype ft;
  int min;
  module *mod;

  ft = fsent_filetype_get(n);
  nt = fsent_fsenttype_get(n);

  /* plain files with no dependencies are never invalid */
  min = 0;
  if(fsent_kind_get(n) == VERTEX_FILE && !n->dep) {
    min++;
  }

  // propagate
  fatal(moria_traverse_vertices
    , &g_graph
    , &n->vertex
    , fsent_invalidate_visitor
    , invalidation->vertex_traversal
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_DEPENDENCY
        , edge_visit : EDGE_DEPENDENCY
        , min_depth : min
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , invalidation
  );

  if(ft == VERTEX_FILETYPE_DIR)
  {
    /* propagate : directory node -> attached rules */
    fatal(fsent_dirnode_children_changed, n, invalidation);
  }
  else if(nt == VERTEX_FSENTTYPE_MODULE || nt == VERTEX_FSENTTYPE_MODEL)
  {
    /* propagate : module -> uses, requires, imports -> module */
    if((mod = fsent_module_get(n)))
    {
      fatal(moria_traverse_vertices
        , &g_graph
        , &mod->vertex
        , fsent_invalidate_visitor
        , 0
        , (moria_traversal_criteria[]) {{
              edge_travel : EDGE_MODULES
            , vertex_visit : VERTEX_MODULE_DIR
          }}
        , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
        , invalidation
      );
    }
  }
  else if(nt == VERTEX_FSENTTYPE_FML)
  {
    /* propagate : formula-file -> rules -> rmas */
    fatal(moria_traverse_vertex_edges
      , &g_graph
      , &n->self_fml->vertex
      , invalidate_fml_rule_visitor
      , invalidation->edge_traversal
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_MOD_RULE_FML
          , edge_visit : EDGE_MOD_RULE
        }}
      , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
      , invalidation
    );

    /* propagate : formula-file -> module -> uses -> module */
    if((mod = fsent_module_get(n)))
    {
      fatal(moria_traverse_vertices
        , &g_graph
        , &mod->vertex
        , fsent_invalidate_visitor
        , 0
        , (moria_traversal_criteria[]) {{
              edge_travel : EDGE_USES_MOD_DIR
            , vertex_visit : VERTEX_MODULE_DIR
          }}
        , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
        , invalidation
      );
    }
  }
  else if(nt == VERTEX_FSENTTYPE_VAR)
  {
    /* propagate : var.bam -> this and descendant modules */
    fatal(moria_traverse_vertices
      , &g_graph
      , &fsent_parent(n)->vertex
      , fsent_invalidate_visitor
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_FSTREE
          , vertex_visit : VERTEX_MODULE_DIR
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
      , invalidation
    );
  }

  finally : coda;
}

xapi fsent_ok(fsent * restrict n)
{
  enter;

  narrator * N;
  char space[512];
  uint16_t z;
  handler_context *handler;
  fabipc_message *msg;

  if(fsent_state_get(n) == VERTEX_OK && n->valid_epoch == fsent_valid_epoch) {
    goto XAPI_FINALIZE;
  }

  fsent_state_set(n, VERTEX_OK);
  n->valid_epoch = fsent_valid_epoch;

  if(log_would(L_DEPGRAPH))
  {
    z = fsent_relative_path_znload(space, sizeof(space), n, g_project_root);
    fatal(log_start, L_DEPGRAPH, &N);
    xsays("fresh ");
    xsayw(space, z);
    fatal(log_finish);
  }

  STATS_INC(n->stats.fsent_fresh);
  STATS_INC(g_stats.fsent_fresh);

  if(events_would(FABIPC_EVENT_NODE_FRESH, &handler, &msg))
  {
    z = fsent_relative_path_znload(msg->text, sizeof(msg->text), n, g_project_root);
    msg->size = z;
    msg->id = 0;
    events_publish(handler, msg);
  }

  finally : coda;
}

xapi fsent_unlink_related(fsent *n, graph_invalidation_context * restrict invalidation)
{
  enter;

  moria_vertex *v;
  moria_edge *upe;
  llist head;
  rbnode *rbn;

  v = &n->vertex;
  llist_init_node(&head);

  /* If there are edges other than those which need to be opportunistically deleted, bail out
   * because the node is not actually going to be deleted in that case */

  if(fsent_filetype_get(n) == VERTEX_FILETYPE_DIR) {
    rbtree_foreach(&v->up, upe, rbn_up) {
      if(upe->attrs == EDGE_RULE_DIR) {
        llist_append(&head, &upe->rbn_up, lln);
      } else {
        goto XAPI_FINALIZE;
      }
    }
  } else if(fsent_kind_get(n) == VERTEX_FORMULA_FILE) {
    rbtree_foreach(&v->up, upe, rbn_up) {
      if(upe->attrs == EDGE_RULE_FML) {
        llist_append(&head, &upe->rbn_up, lln);
      } else {
        goto XAPI_FINALIZE;
      }
    }

//    fatal(fsent_invalidate, n, invalidation);
  } else if(fsent_kind_get(n) == VERTEX_SHADOW_LINK) {
  } else {
//    fatal(fsent_invalidate, n, invalidation);
  }

  llist_foreach(&head, rbn, lln) {
    upe = containerof(rbn, typeof(*upe), rbn_up);
    fatal(moria_edge_disconnect, &g_graph, upe);
  }

  finally : coda;
}

xapi fsent_unlink(fsent *n, graph_invalidation_context * restrict invalidation)
{
  enter;

  fabipc_message *msg;
  uint16_t z;
  handler_context *handler;
  moria_vertex *v;

  v = &n->vertex;
  if(fsent_kind_get(n) == VERTEX_MODULE_FILE)
  {
    /* when a module.bam file is unlinked, invalidate its immediate parent module */
    fatal(moria_traverse_vertices
      , &g_graph
      , &fsent_module_get(n)->dir_node->vertex
      , node_invalidate_visitor_first
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_FSTREE
          , vertex_visit : VERTEX_MODULE_DIR
          , min_depth : 1
          , max_depth : UINT16_MAX
        }}
      , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
      , invalidation
    );
  }

  /* also remove related edges, e.g. //module/{imports,uses} */
  if(n->import_scope_edge)
  {
    fatal(fsedge_disconnect, n->import_scope_edge);
  }
  if(n->use_scope_edge)
  {
    fatal(fsedge_disconnect, n->use_scope_edge);
  }

  fsent_state_set(n, VERTEX_UNLINKED);

  if(log_would(L_NODE))
  {
    narrator * N;
    fatal(log_start, L_NODE, &N);
    xsayf("delete %p %s", n, v->label);
    fatal(log_finish);
  }

  if(events_would(FABIPC_EVENT_NODE_DELETE, &handler, &msg))
  {
    z = fsent_relative_path_znload_bacon(msg->text, sizeof(msg->text), n, g_project_root);
    msg->size = z;
    msg->id = 0;
    events_publish(handler, msg);
  }

  finally : coda;
}

/*
 * n - directory node
 */
xapi fsent_dirnode_children_changed(fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
{
  enter;

  // traverse to rules attached to this directory node
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , &n->vertex
    , children_changed_visitor
    , invalidation->edge_traversal
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_RULE_DIR
        , edge_visit : EDGE_RULE_DIR
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST
    , invalidation
  );

  finally : coda;
}

/*
 * moria callback to invalidate a node
 *
 * arg      - invalidation
 * mode     - unused
 * distance - unused
 * result   - unused
 */
xapi fsent_invalidate_visitor(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  fsent * n;
  narrator * N;
  char space[512];
  uint16_t z;
  handler_context *handler;
  fabipc_message *msg;
  graph_invalidation_context *invalidation;

  n = containerof(v, fsent, vertex);
  if(fsent_invalid_get(n)) {
    goto XAPI_FINALIZE;
  }

  fsent_invalid_set(n);
  invalidation = arg;
  invalidation->any = true;

  if(log_would(L_DEPGRAPH))
  {
    if(g_project_root)
    {
      z = fsent_relative_path_znload_bacon(space, sizeof(space), n, g_project_root);

      fatal(log_start, L_DEPGRAPH, &N);
      xsays("stale ");
      xsayw(space, z);
      fatal(log_finish);
    }
  }

  STATS_INC(n->stats.fsent_stale);
  STATS_INC(g_stats.fsent_stale);

  if(events_would(FABIPC_EVENT_NODE_STALE, &handler, &msg))
  {
    z = fsent_relative_path_znload_bacon(msg->text, sizeof(msg->text), n, g_project_root);
    msg->size = z;
    msg->id = 0;
    events_publish(handler, msg);
  }

  finally : coda;
}

xapi fsent_lookup_index(fsent * n)
{
  enter;

  moria_vertex_entry *entry, key;

  if(rbnode_attached(&n->vertex.rbn_lookup)) {
    goto XAPI_FINALLY;
  }

  /* shadow nodes are not indexed */
  if(fsent_shadowtype_get(n)) {
    goto XAPI_FINALLY;
  }

  RUNTIME_ASSERT(fsent_kind_get(n) == VERTEX_DIR);

  key.label = n->vertex.label;
  key.label_len = n->vertex.label_len;
  entry = hashtable_search(g_graph_ht, &key, sizeof(key), moria_vertex_entry_hash, moria_vertex_entry_key_cmp);

  if(!entry)
  {
    /* entry is using this nodes label */
    rbtree_init(&key.rbt);
    fatal(hashtable_put, g_graph_ht, &key);
    entry = hashtable_get(g_graph_ht, &key);
  }

  rbtree_put(&entry->rbt, n, vertex.rbn_lookup, (void*)ptrcmp);

  finally : coda;
}

xapi fsent_lookup_disindex(fsent * n)
{
  enter;

  moria_vertex_entry *entry, key;
  fsent *fse;

  if(!rbnode_attached(&n->vertex.rbn_lookup)) {
    goto XAPI_FINALLY;
  }

  key.label = n->vertex.label;
  key.label_len = n->vertex.label_len;
  entry = hashtable_search(g_graph_ht, &key, sizeof(key), moria_vertex_entry_hash, moria_vertex_entry_key_cmp);
  if(entry == 0) {
    RUNTIME_ASSERT(!rbnode_attached(&n->vertex.rbn_lookup));
    goto XAPI_FINALLY;
  }

  /* delete this node from the tree */
  rbtree_delete_node(&entry->rbt, &n->vertex.rbn_lookup);

  if(!rbtree_empty(&entry->rbt))
  {
    /* update the entry to use the label from another of its nodes - the labels have the same contents
     * though because they are in the same bucket */
    fse = rbtree_first(&entry->rbt, fsent, vertex.rbn_lookup);
    entry->label = fse->vertex.label;
  }

  finally : coda;
}
