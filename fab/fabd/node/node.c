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

#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "value.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "node.internal.h"
#include "shadow.h"
#include "formula.h"
#include "filesystem.h"
#include "filesystem.internal.h"
#include "logging.h"
#include "module.internal.h"
#include "node_operations.h"
#include "params.h"
#include "path.h"
#include "pattern.h"
#include "config.internal.h"
#include "MODULE.errtab.h"
#include "walker.h"
#include "extern.h"
#include "variant.h"
#include "var.h"
#include "stats.h"

#include "attrs.h"
#include "zbuffer.h"
#include "hash.h"

static uint8_t node_fs_epoch;

set * g_parse_nodes;
map * g_nodes_by_wd;
node * g_root;
node * g_project_root;
node * g_project_shadow;

attrs32 * node_property_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      NODE_PROPERTY_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
NODE_PROPERTY_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(node_property_attrs);
}

/// node_root_init
//
// SUMMARY
//  create the root node (g_root)
//
static xapi root_init()
{
  enter;

  vertex * v;
  node * n;
  path * p = 0;

  STATS_INC(nodes);
  fatal(vertex_create, &v, g_graph, VERTEX_FILETYPE_DIR | VERTEX_TYPE_NODE);
  n = vertex_value(v);
  n->self_bpe.typemark = BPE_NODE;

  fatal(path_creates, &p, "(root)");
  n->name = p;
  p = 0;

  v->label = n->name->name;
  v->label_len = n->name->namel;

  n->wd = -1;
  n->fst = &fstree_root;
  n->fs = n->fst->fs;

  g_root = n;

finally:
  fatal(path_free, p);
coda;
}

/// node_project_init
//
// SUMMARY
//  create the project node (g_project_root) and attach it to the root
//
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
  fatal(node_graft, g_params.proj_dir, &g_project_root, &invalidation);

finally:
  graph_invalidation_end(&invalidation);
coda;
}

//
// internal
//

size_t node_get_relative_path(const node * subject, const node * base, void * restrict dst, size_t sz)
{
  const node *n;
  const node *bs[64];
  size_t bsl = 0;
  const node *ns[64];
  size_t nsl = 0;
  size_t z = 0;
  int x;

  /* subject and base are the same node */
  if(subject == base)
  {
    z += znloads(dst + z, sz - z, ".");
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
    n = node_fsparent(n);
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
    n = node_fsparent(n);
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
      z += znloads(dst + z, sz - z, "/");
    z += znloads(dst + z, sz - z, "..");
  }

  /* symlink to the project module */
  if(nsl > 2 && node_shadowtype_get(ns[nsl - 1]) == VERTEX_SHADOWTYPE_MODULES && ns[nsl - 2] == g_project_shadow)
  {
    z += znloads(dst + z, sz - z, "module");
    nsl -= 2;
  }

  /* path from nearest common ancestor to the subject */
  for(x = nsl - 1; x >= 0; x--)
  {
    n = ns[x];

    if(z)
      z += znloads(dst + z, sz - z, "/");
    z += znloadw(dst + z, sz - z, n->name->name, n->name->namel);
  }

  ((char*)dst)[z] = 0;

  return z;
}

xapi node_relative_path_say(const node * n, const node * base, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = node_get_relative_path(n, base, path, sizeof(path));

  xsayw(path, pathl);

  finally : coda;
}

//
// public
//

xapi node_setup_minimal()
{
  enter;

  fatal(map_create, &g_nodes_by_wd);
  fatal(set_create, &g_parse_nodes);

  finally : coda;
}

xapi node_setup()
{
  enter;

  fatal(node_setup_minimal);

  // setup basic nodes
  fatal(root_init);
  fatal(project_init);

  node_fs_epoch = 1;

  finally : coda;
}

xapi node_cleanup()
{
  enter;

  fatal(map_xfree, g_nodes_by_wd);
  fatal(set_xfree, g_parse_nodes);

  finally : coda;
}

xapi node_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  if(dry)
  {

  }
  else if(cfg->filesystems.attrs & CONFIG_CHANGED)
  {
    /* invalidate node -> filesystem assignments */
    node_fs_epoch++;
  }

  finally : coda;
}

xapi node_full_refresh(void)
{
  enter;

  int walk_id;
  //const filesystem *fs;
  //invalidate_type iv;
  graph_invalidation_context invalidation = { 0 };

  fatal(graph_invalidation_begin, &invalidation);
  walk_id = walker_descend_begin();

  /* refresh the project tree */
  fatal(walker_descend, 0, g_project_root, 0, g_params.proj_dir, walk_id, &invalidation);
  fatal(walker_ascend, g_project_root, walk_id, &invalidation);

  /* refresh extern trees */
  fatal(extern_refresh, walk_id, &invalidation);

finally:
  graph_invalidation_end(&invalidation);
coda;
}

xapi node_graft(const char * restrict base, node ** restrict rn, graph_invalidation_context * restrict invalidation)
{
  enter;

  node * parent;
  node * child = 0;
  module * mod = 0;
  const filesystem * fs = 0;

  parent = g_root;
  const char * seg = base + 1;
  while(*seg)
  {
    const char * end = seg + 1;
    while(*end && *end != '/')
      end++;

    vertex * v = vertex_downw(vertex_containerof(parent), seg, end - seg);
    if(v)
    {
      child = vertex_value(v);
    }
    else
    {

      fs = node_filesystem_get(parent);

      fatal(node_createw, &child, VERTEX_FILETYPE_DIR | VERTEX_OK, fs, mod, seg, end - seg);
      fatal(node_connect, parent, child, EDGE_TYPE_FS, invalidation, 0, 0);
    }
    parent = child;

    seg = end;
    if(*seg)
      seg++;
  }

  *rn = child;

  finally : coda;
}

static void node_fstree_refresh(node * restrict n)
{
  node * parent;

  if(n->fs_epoch == node_fs_epoch)
    return;

  /* the root node does not refresh */
  if((parent = node_fsparent(n)) == 0)
    return;

  STATS_INC(fstree_refresh);

  node_fstree_refresh(parent);

  n->fst = 0;
  if(parent->fst)
  {
    if((n->fst = fstree_down((struct fstree *)parent->fst, n->name->name, n->name->namel)))
    {
      n->fs = n->fst->fs;
    }
  }

  if(!n->fs)
    n->fs = parent->fs;

  n->fs_epoch = node_fs_epoch;
}

const struct filesystem * node_filesystem_get(node * restrict n)
{
  enum vertex_filetype filetype;

  filetype = node_filetype_get(n);
  if(filetype != VERTEX_FILETYPE_DIR)
    n = node_fsparent(n);

  node_fstree_refresh(n);

  RUNTIME_ASSERT(n->fs);
  return n->fs;
}

xapi node_createw(
    node ** restrict n
  , uint32_t attrs
  , const filesystem * restrict fs
  , module * restrict mod
  , const char * restrict name
  , uint16_t name_len
)
{
  enter;

  fatal(node_vertex_create, n, attrs, name, name_len);

  if(mod || fs)
  {
    RUNTIME_ASSERT(((attrs & ~(VERTEX_STATE_OPT)) | VERTEX_TYPE_NODE) == VERTEX_DIR);
    (*n)->mod = mod;
    (*n)->fs = fs;
  }

  if(attrs == VERTEX_DIR)
  {
    (*n)->fs_epoch = node_fs_epoch;
  }

  finally : coda;
}

xapi node_creates(
    node ** restrict n
  , vertex_filetype filetype
  , const filesystem * restrict fs
  , module * restrict mod
  , const char * restrict name
)
{
  xproxy(node_createw, n, filetype, fs, mod, name, strlen(name));
}

xapi node_xdestroy(node * restrict n)
{
  enter;

  vertex_kind kind;

  kind = node_kind_get(n);
  if(kind == VERTEX_FML)
  {
    fatal(formula_xfree, n->self_fml);
  }
  else if(kind == VERTEX_MODULE_BAM)
  {
    fatal(module_xfree, n->self_mod);
  }
  else if(kind == VERTEX_MODEL_BAM)
  {
    fatal(module_xfree, n->self_mod);
  }

  path_xfree(&n->name);

  finally : coda;
}

size_t node_get_absolute_path(const node * restrict n, void * restrict dst, size_t sz)
{
  size_t z = 0;
  const node *base;

  if(node_shadowtype_get(n))
  {
    z += znloads(dst, sz, "//");
    base = g_shadow;
  }
  else
  {
    z += znloads(dst, sz, "/");
    base = g_root;
  }
  z += node_get_relative_path(n, base, dst + z, sz - z);

  return z;
}

size_t node_get_path(const node * restrict n, void * restrict dst, size_t dst_size)
{
  /* absolute path for shadow nodes */
  if(node_shadowtype_get(n))
  {
    return node_get_absolute_path(n, dst, dst_size);
  }
  else
  {
    return node_get_project_relative_path(n, dst, dst_size);
  }
}

size_t node_get_project_relative_path(const node * restrict n, void * restrict dst, size_t sz)
{
  if(g_project_root == 0)
  {
    /* project not yet initialized */
    return node_get_absolute_path(n, dst, sz);
  }

  return node_get_relative_path(n, g_project_root, dst, sz);
}

size_t node_get_module_relative_path(const node * restrict n, void * restrict dst, size_t sz)
{
  const module *mod;

  if((mod = node_module_get(n)) == 0) {
    /* WTF IS THIS SHIT */
    return node_get_absolute_path(n, dst, sz);
  }

  return node_get_relative_path(n, mod->dir_node, dst, sz);
}

xapi node_module_relative_path_say(const node * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = node_get_module_relative_path(n, path, sizeof(path));
  xsayw(path, pathl);

  finally : coda;
}

xapi node_project_relative_path_say(const node * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = node_get_project_relative_path(n, path, sizeof(path));
  xsayw(path, pathl);

  finally : coda;
}

xapi node_absolute_path_say(const node * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = node_get_absolute_path(n, path, sizeof(path));
  xsayw(path, pathl);

  finally : coda;
}

xapi node_path_say(node * restrict n, struct narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = node_get_path(n, path, sizeof(path));
  xsayw(path, pathl);

  finally : coda;
}

xapi node_property_say(const node * restrict n, node_property property, const node_property_context * restrict ctx, narrator * restrict N)
{
  enter;

  const filesystem *fs;
  char path[512];
  uint16_t pathl;
  uint32_t attrs;

  /* node name */
  if(property == NODE_PROPERTY_NAME)
    fatal(narrator_xsayw, N, n->name->name, n->name->namel);
  else if(property == NODE_PROPERTY_EXT)
    fatal(narrator_xsayw, N, n->name->ext, n->name->extl);
  else if(property == NODE_PROPERTY_SUFFIX)
    fatal(narrator_xsayw, N, n->name->suffix, n->name->suffixl);
  else if(property == NODE_PROPERTY_BASE)
    fatal(narrator_xsayw, N, n->name->base, n->name->basel);

  else if(property == NODE_PROPERTY_ABSDIR || property == NODE_PROPERTY_RELDIR)
  {
    if(node_filetype_get(n) != VERTEX_FILETYPE_DIR)
    {
      n = node_fsparent(n);
    }

    if(property == NODE_PROPERTY_ABSDIR)
    {
      fatal(node_absolute_path_say, n, N);
    }
    else if(node_shadowtype_get(n))
    {
      fatal(node_absolute_path_say, n, N);
    }
    else
    {
      fatal(node_relative_path_say, n, ctx->mod->dir_node, N);
    }
  }
  else if(property == NODE_PROPERTY_ABSPATH || property == NODE_PROPERTY_RELPATH)
  {
    if(property == NODE_PROPERTY_ABSPATH)
    {
      fatal(node_absolute_path_say, n, N);
    }
    else if(node_shadowtype_get(n))
    {
      fatal(node_absolute_path_say, n, N);
    }
    else
    {
      fatal(node_relative_path_say, n, ctx->mod->dir_node, N);
    }
  }
  /* derived properties */
  else if(property == NODE_PROPERTY_VARIANT)
  {
    if(n->var)
    {
      fatal(narrator_xsayw, N, n->var->norm, n->var->norm_len);
    }
  }
  else if(property == NODE_PROPERTY_FSROOT)
  {
    fs = node_filesystem_get((node*)n);
    pathl = filesystem_get_absolute_path(fs, path, sizeof(path));
    fatal(narrator_xsayw, N, path, pathl);
  }
  else if(property == NODE_PROPERTY_INVALIDATION)
  {
    fs = node_filesystem_get((node*)n);
    fatal(narrator_xsayf, N, "%s 0x%04x"
      , attrs16_name_byvalue(invalidate_attrs, fs->invalidate)
      , fs->attrs
    );
  }
  else if(property == NODE_PROPERTY_STATE)
  {
    attrs = vertex_containerof(n)->attrs & VERTEX_STATE_OPT;
    if(attrs)
    {
      fatal(narrator_xsays, N, attrs32_name_byvalue(graph_state_attrs, vertex_containerof(n)->attrs & VERTEX_STATE_OPT));
    }
  }
  else if(property == NODE_PROPERTY_TYPE)
  {
    fatal(narrator_xsayf, N, "%s 0x%08x"
      , attrs32_name_byvalue(graph_kind_attrs, vertex_containerof(n)->attrs & VERTEX_KIND_OPT)
      , vertex_containerof(n)->attrs & VERTEX_KIND_OPT
    );
  }

  finally : coda;
}
