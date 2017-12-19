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
#include "valyria/list.h"
#include "valyria/map.h"
#include "value.h"
#include "narrator.h"

#include "node.internal.h"
#include "node_operations.h"
#include "filesystem.h"
#include "reconfigure.h"
#include "path.h"
#include "params.h"
#include "logging.h"
#include "ff_node_pattern.h"
#include "ff.h"
#include "FF.errtab.h"
#include "module.h"

#include "zbuffer.h"
#include "attrs32.h"

graph * g_node_graph;
map * g_nodes_by_wd;
node * g_root;
node * g_project_root;

#define RELATION_ATTR_DEF(a, b) + 1
static size_t relation_type_attrs_len = 0 RELATION_TYPE_TABLE;
#undef RELATION_ATTR_DEF

static attrs32 * relation_type_attrs = (typeof(*relation_type_attrs)[]) {
#define RELATION_ATTR_DEF(a, b) { value : (b), name : #a, namel : strlen(#a) },
RELATION_TYPE_TABLE
};

#define NODE_ATTR_DEF(a, b) + 1
static size_t node_fstype_attrs_len = 0 NODE_FSTYPE_TABLE;
#undef NODE_ATTR_DEF

static attrs32 * node_fstype_attrs = (typeof(*node_fstype_attrs)[]) {
#define NODE_ATTR_DEF(a, b) { value : (b), name : #a, namel : strlen(#a) },
NODE_FSTYPE_TABLE
};

static void __attribute__((constructor)) attrs_init()
{
  qsort(relation_type_attrs, relation_type_attrs_len, sizeof(*relation_type_attrs), attrs32_compare_byname);
  qsort(node_fstype_attrs, node_fstype_attrs_len, sizeof(*node_fstype_attrs), attrs32_compare_byname);
}

static xapi node_report_visitor(vertex * v, int distance, void * arg)
{
  enter;

  narrator * N;
  fatal(log_start, L_GRAPH, &N);
  xsayf("%*s%s", distance * 2, "", v->label);
  fatal(log_finish);

  finally : coda;
}

//
// public
//

const char * relation_type_name(uint32_t attrs) { return attrs32_option_name(relation_type_attrs, relation_type_attrs_len, RELATION_TYPE_OPT, attrs); }
const char * node_fstype_name(uint32_t attrs) { return attrs32_option_name(node_fstype_attrs, node_fstype_attrs_len, NODE_FSTYPE_OPT, attrs); }

xapi node_setup()
{
  enter;

  fatal(graph_createx, &g_node_graph, RELATION_TYPE_FS, sizeof(node), node_destroy, 0);
  fatal(map_create, &g_nodes_by_wd);

  finally : coda;
}

xapi node_cleanup()
{
  enter;

  fatal(graph_xfree, g_node_graph);
  fatal(map_xfree, g_nodes_by_wd);

  finally : coda;
}

xapi node_root_init()
{
  enter;

  fatal(node_createw, &g_root, NODE_FSTYPE_DIR, 0, 0, "", 0);

  finally : coda;
}

xapi node_project_init()
{
  enter;

  fatal(node_graft, g_params.proj_dir, &g_project_root);

  finally : coda;
}

xapi node_graft(const char * restrict base, node ** restrict rn)
{
  enter;

  node * parent;
  node * child = 0;

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
      module * mod = 0;
      const filesystem * fs = 0;

      if(parent->mod && parent->mod->leaf)
        mod = parent->mod;
      if(!mod)
        mod = module_lookup(base, end - base);
      if(mod)
        fs = mod->base->fs;
      else if(parent->fs && parent->fs->leaf)
        fs = parent->fs;
      if(!fs)
        fs = filesystem_lookup(base, end - base);

      fatal(node_createw, &child, NODE_FSTYPE_DIR, fs, mod, seg, end - seg);
      fatal(node_connect, parent, child, RELATION_TYPE_FS);
    }
    parent = child;

    seg = end;
    if(*seg)
      seg++;
  }

  *rn = child;

  finally : coda;
}

xapi node_reconfigure(reconfigure_context * ctx, const value * restrict config, uint32_t dry)
{
  enter;

  char path[512];
  size_t pathl;

  const list * vertices;
  int x;

  if(!dry && ctx->filesystems_changed)
  {
    vertices = graph_vertices(g_node_graph);

    for(x = 0; x < vertices->l; x++)
    {
      node * n = vertex_value(list_get(vertices, x));
      if(node_fstype_get(n) == NODE_FSTYPE_DIR)
      {
        pathl = node_get_absolute_path(n, path, sizeof(path));
        n->fs = filesystem_lookup(path, pathl);
      }
    }
  }

  finally : coda;
}

xapi graph_node_create(
    struct vertex ** const restrict v
  , graph * const restrict g
  , uint32_t attrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  enter;

  node * n = 0;

  // child is set to FILE in connect
  node_fstype fstype = NODE_FSTYPE_DIR;

  const filesystem * fs = NULL;
  module * mod = NULL;

  fatal(node_createw, &n, fstype, fs, mod, label, label_len);
  *v = vertex_containerof(n);

  finally : coda;
}

xapi node_createw(
    node ** restrict n
  , node_fstype fstype
  , const filesystem * restrict fs
  , module * restrict mod
  , const char * restrict name
  , uint16_t name_len
)
{
  enter;

  vertex * v;
  path * p = 0;

  fatal(vertex_create, &v, g_node_graph, fstype);
  *n = vertex_value(v);

  fatal(path_createw, &p, name, name_len);
  (*n)->name = p;
  p = 0;

  // the vertex label points at the node name
  v->label = (*n)->name->name;
  v->label_len = (*n)->name->namel;
  (*n)->fs = fs;
  (*n)->mod = mod;

  // index the node for lookup
  fatal(graph_identity_indexw, g_node_graph, v, (*n)->name->name, (*n)->name->namel);

  if(memncmp((*n)->name->ext, (*n)->name->extl, MMS("fab")) == 0)
  {
    if(memncmp((*n)->name->base, (*n)->name->basel, MMS("common")))
      fatal(graph_identity_indexw, g_node_graph, v, (*n)->name->base, (*n)->name->basel);
  }

  if(fstype == NODE_FSTYPE_DIR)
    (*n)->wd = -1;

finally:
  fatal(path_xfree, &p);
coda;
}

xapi node_creates(
    node ** restrict n
  , node_fstype fstype
  , const filesystem * restrict fs
  , module * restrict mod
  , const char * restrict name
)
{
  xproxy(node_createw, n, fstype, fs, mod, name, strlen(name));
}

xapi node_report()
{
  enter;

  if(log_would(L_GRAPH))
  {
    logs(L_GRAPH, "node fs graph");

    fatal(graph_traverse_vertices_all
      , g_node_graph
      , node_report_visitor
      , (traversal_criteria[]) {{
            edge_travel : RELATION_TYPE_FS
          , edge_visit : RELATION_TYPE_FS
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
      , 0
    );
  }

  finally : coda;
}

void node_destroy(node * restrict n)
{
  path_xfree(&n->name);
}

size_t node_get_absolute_path(const node * restrict n, void * restrict dst, size_t sz)
{
  const node * ns[64];
  size_t nsl = 0;
  size_t z = 0;
  int x;

  // relative to the root
  while(n && nsl < (sizeof(ns) / sizeof(ns[0])))
  {
    ns[nsl++] = n;
    n = node_fsparent(n);

    if(n == g_root)
      break;
  }

  for(x = nsl - 1; x >= 0; x--)
  {
    n = ns[x];

    z += znloads(dst + z, sz - z, "/");
    z += znloadw(dst + z, sz - z, n->name->name, n->name->namel);
  }

  return z;
}

size_t node_get_relative_path(const node * restrict n, void * restrict dst, size_t sz)
{
  const node * ns[64];
  size_t nsl = 0;
  size_t z = 0;
  int x;

  // relative to the project root
  while(n && nsl < (sizeof(ns) / sizeof(ns[0])))
  {
    ns[nsl++] = n;
    n = node_fsparent(n);

    if(n == g_project_root)
      break;
  }

  for(x = nsl - 1; x >= 0; x--)
  {
    n = ns[x];

    if(z)
      z += znloads(dst + z, sz - z, "/");
    z += znloadw(dst + z, sz - z, n->name->name, n->name->namel);
  }

  return z;
}

xapi node_relative_path_say(const node * restrict n, narrator * restrict N)
{
  enter;

  char path[512];
  size_t pathl;

  pathl = node_get_relative_path(n, path, sizeof(path));

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

typedef struct lookup_path_context {
  const char * path;
  uint16_t path_len;

  const char * segment;
} lookup_path_context;

static xapi __attribute__((nonnull(1))) lookup_path_callback(
    void * restrict _ctx
  , const char ** restrict label
  , uint16_t * restrict label_len
)
{
  enter;

  lookup_path_context * ctx = _ctx;

  if(label)
  {
    *label = 0;
    if(ctx->segment)
    {
      uint16_t seg_len = 0;
      if(*ctx->segment == '/')
      {
        ctx->segment--;
        seg_len++;
      }
      while(ctx->segment != ctx->path && *ctx->segment != '/')
      {
        ctx->segment--;
        seg_len++;
      }

      if(ctx->segment == ctx->path)
      {
        *label = ctx->segment;
        *label_len = seg_len;
        ctx->segment = 0;
      }
      else
      {
        *label = ctx->segment + 1;
        *label_len = seg_len - 1;
      }
    }
  }

  // rewind
  else
  {
    ctx->segment= ctx->path + ctx->path_len;
  }

  finally : coda;
}

xapi node_lookup_path(const ff_node * restrict ffn, const char * restrict path, node ** restrict rn)
{
  enter;

  vertex * vertices[2];
  void * mm_tmp = 0;
  int r;

  lookup_path_context ctx = { .path = path, .path_len = strlen(path) };
  fatal(graph_lookup, g_node_graph, lookup_path_callback, &ctx, &mm_tmp, vertices, &r);

  if(r == 2)
  {
    fatal(ffn_semantic_error, FF_EXTERN_AMBIGREF, ffn, path, 0, vertices);
  }
  else if(r == 0)
  {
    fatal(ffn_semantic_error, FF_EXTERN_NOREF, ffn, path, 0, 0);
  }
  *rn = vertex_value(vertices[0]);

finally:
  wfree(mm_tmp);
coda;
}

typedef struct lookup_pattern_context {
  const ff_node_pattern * ref;
  char tmp[512];
  ffn_bydir_context walk_context;
} lookup_pattern_context;

static xapi __attribute__((nonnull(1))) lookup_pattern_callback(
    void * restrict _ctx
  , const char ** restrict label
  , uint16_t * restrict label_len
)
{
  enter;

  lookup_pattern_context * ctx = _ctx;

  if(label)
  {
    fatal(ffn_pattern_bydir_strings_rtl, &ctx->walk_context, ctx->tmp, sizeof(ctx->tmp), label, label_len);
  }

  // rewind
  else
  {
    ffn_bydir_rtl_setup(&ctx->walk_context, ctx->ref->chain);
  }

  finally : coda;
}

xapi node_lookup_pattern(const ff_node_pattern * restrict ref, node ** restrict rn)
{
  enter;

  vertex * vertices[2];
  void * mm_tmp = 0;
  int r;

  lookup_pattern_context ctx = { ref : ref };
  fatal(graph_lookup, g_node_graph, lookup_pattern_callback, &ctx, &mm_tmp, vertices, &r);

  if(r == 0)
  {
    fatal(ffn_semantic_error, FF_EXTERN_NOREF, ref, 0, 0, 0);
  }
  else if(r == 2)
  {
    fatal(ffn_semantic_error, FF_EXTERN_AMBIGREF, ref, 0, 0, vertices);
  }

  *rn = vertex_value(vertices[0]);

finally:
  wfree(mm_tmp);
coda;
}

xapi node_get_ffn(node * restrict n, ff_node ** ffn)
{
  enter;

  char path[512];

  if(n->ffn == 0)
  {
    node_get_absolute_path(n, path, sizeof(path));
    fatal(ff_load_pathf, &n->ffn, path);
  }

  if(ffn)
    *ffn = n->ffn;

  finally : coda;
}
