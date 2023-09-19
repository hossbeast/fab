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


#include "moria/edge.h"
#include "moria/graph.h"
#include "moria/traverse.h"
#include "narrator.h"
#include "value/writer.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"

#include "graph.h"
#include "dependency.h"
#include "fsedge.h"
#include "fsent.h"
#include "module.h"

#include "zbuffer.h"
#include "common/attrs.h"

/* fstree is the identity subgraph - moria treats it specially */
STATIC_ASSERT(EDGE_TYPE_FSTREE == MORIA_EDGE_IDENTITY);

/* moria treats links specially */
STATIC_ASSERT(VERTEX_FILETYPE_LINK == MORIA_VERTEX_LINK);

static llist edge_list = LLIST_INITIALIZER(edge_list);              // active
static llist edge_freelist = LLIST_INITIALIZER(edge_freelist);      // freelist

moria_graph g_graph = MORIA_GRAPH_INITIALIZER(g_graph);
hashtable * g_graph_ht;

attrs32 * graph_vertex_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_KIND_TABLE
      VERTEX_STATE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_KIND_TABLE
VERTEX_STATE_TABLE
  }
}};

attrs32 * graph_edge_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      EDGE_KIND_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
EDGE_KIND_TABLE
  }
}};

attrs32 * graph_edge_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      EDGE_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
EDGE_TYPE_TABLE
  }
}};

attrs32 * graph_vertex_kind_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_KIND_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_KIND_TABLE
  }
}};

attrs32 * graph_vertex_state_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_STATE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_STATE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(graph_edge_attrs);
  attrs32_init(graph_vertex_attrs);
  attrs32_init(graph_edge_type_attrs);
  attrs32_init(graph_vertex_kind_attrs);
  attrs32_init(graph_vertex_state_attrs);
}

//
// public
//

void graph_edge_alloc(moria_edge ** ep)
{
  moria_edge *e;

  if((e = llist_shift(&edge_freelist, typeof(*e), owner)) == 0)
  {
    xmalloc(&e, sizeof(*e));
  }

  moria_edge_init(e, &g_graph);
  llist_append(&edge_list, e, owner);

  *ep = e;
}

void graph_edge_release(struct moria_edge * restrict e)
{
  RUNTIME_ASSERT(e->attrs == EDGE_RULE_FML || e->attrs == EDGE_MOD_DIR);

  llist_delete_node(&e->owner);
  llist_append(&edge_freelist, e, owner);
}

void graph_setup()
{
  hashtable_createx(
      &g_graph_ht
    , sizeof(moria_vertex_entry)
    , 100
    , moria_vertex_entry_hash
    , moria_vertex_entry_cmp
    , 0
  );
}

void graph_cleanup()
{
  moria_edge *e;
  llist *T;

  moria_graph_destroy(&g_graph);
  hashtable_xfree(g_graph_ht);

  llist_splice_head(&edge_freelist, &edge_list);
  llist_foreach_safe(&edge_freelist, e, owner, T) {
    wfree(e);
  }
}

static void graph_vertex_write(value_writer * restrict writer, const moria_vertex * restrict v)
{
  const fsent *n;
  vertex_kind vk;
  size_t z;
  char name[512];
  module *mod;

  z = 0;
  z += znloadf(name + z, sizeof(name) - z, "%p ", v);
  z += znload_attrs32(name + z, sizeof(name) - z, graph_vertex_attrs, v->attrs);
  z += znloads(name + z, sizeof(name) - z, " ");

  if((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT) {
    n = containerof(v, fsent, vertex);

    z += fsent_path_znload(name + z, sizeof(name) - z, n);

    vk = fsent_kind_get(n);
    if(vk == VERTEX_SHADOW_LINK)
    {
      z += znloads(name + z, sizeof(name) - z, " => ");
      z += fsent_path_znload(name + z, sizeof(name) - z, containerof(v->ref, fsent, vertex));
    }
  } else if((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_RULE) {
    z += znloadf(name + z, sizeof(name) - z, "(rule)");
  } else if((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_MODULE) {
    mod = containerof(v, module, vertex);
    z += znloadf(name + z, sizeof(name) - z, "(module %.*s : %s)", 16, mod->id, mod->dir_node_abspath);
  }

  value_writer_bytes(writer, name, z);
}

void graph_edge_say(const moria_edge * restrict e, narrator * restrict N)
{
  int x;
  dependency *ne = 0;
  value_writer writer;
  char buf[64];
  size_t z;

  value_writer_init(&writer);
  value_writer_open(&writer, N);

  if(e->attrs & EDGE_DEPENDENCY) {
    ne = containerof(e, typeof(*ne), edge);
  }

  if(!(e->attrs & MORIA_EDGE_HYPER) || e->Alen)
  {
    value_writer_push_set(&writer);
    value_writer_push_mapping(&writer);
    value_writer_string(&writer, "up");
    value_writer_push_set(&writer);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      graph_vertex_write(&writer, e->A);
    }
    else
    {
      for(x = 0; x < e->Alen; x++)
      {
        graph_vertex_write(&writer, e->Alist[x].v);
      }
    }
    value_writer_pop_set(&writer);
    value_writer_pop_mapping(&writer);
  }

  value_writer_push_mapping(&writer);
  value_writer_string(&writer, "relation");
  z = znload_attrs32(buf, sizeof(buf), graph_edge_attrs, e->attrs);
  value_writer_bytes(&writer, buf, z);
  value_writer_pop_mapping(&writer);

  if(!(e->attrs & MORIA_EDGE_HYPER) || e->Blen)
  {
    value_writer_push_mapping(&writer);
    value_writer_string(&writer, "down");
    value_writer_push_set(&writer);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      graph_vertex_write(&writer, e->B);
    }
    else
    {
      for(x = 0; x < e->Blen; x++)
      {
        graph_vertex_write(&writer, e->Blist[x].v);
      }
    }
    value_writer_pop_set(&writer);
    value_writer_pop_mapping(&writer);
  }

  if(ne && ne->fml)
  {
    value_writer_push_mapping(&writer);
    value_writer_string(&writer, "fml");
    graph_vertex_write(&writer, &ne->fml->vertex);
    value_writer_pop_mapping(&writer);
  }
  value_writer_mapping_string_bool(&writer, "hyper", e->attrs & MORIA_EDGE_HYPER);

  value_writer_pop_set(&writer);
  value_writer_close(&writer);
}

void graph_invalidation_begin(graph_invalidation_context * restrict invalidation)
{
  RUNTIME_ASSERT(invalidation->vertex_traversal == 0);
  moria_vertex_traversal_begin(&g_graph, &invalidation->vertex_traversal);

  RUNTIME_ASSERT(invalidation->edge_traversal == 0);
  moria_edge_traversal_begin(&g_graph, &invalidation->edge_traversal);
  invalidation->any = false;
}

void graph_invalidation_end(graph_invalidation_context * restrict invalidation)
{
  moria_vertex_traversal_end(&g_graph, invalidation->vertex_traversal);
  invalidation->vertex_traversal = 0;

  moria_edge_traversal_end(&g_graph, invalidation->edge_traversal);
  invalidation->edge_traversal = 0;
}

fab_fsent_type vertex_kind_remap(vertex_kind kind)
{
   switch(kind) {
    case VERTEX_DIR:
      return FAB_FSENT_TYPE_DIR;
    case VERTEX_MODULE_DIR:
      return FAB_FSENT_TYPE_MODULE_DIR;
    case VERTEX_FILE:
      return FAB_FSENT_TYPE_FILE;
    case VERTEX_MODULE_FILE:
      return FAB_FSENT_TYPE_MODULE_FILE;
    case VERTEX_MODEL_FILE:
      return FAB_FSENT_TYPE_MODEL_FILE;
    case VERTEX_FORMULA_FILE:
      return FAB_FSENT_TYPE_FORMULA_FILE;
    case VERTEX_VAR_FILE:
      return FAB_FSENT_TYPE_VAR_FILE;
    case VERTEX_CONFIG_FILE:
      return FAB_FSENT_TYPE_CONFIG_FILE;
    case VERTEX_SHADOW_DIR:
      return FAB_FSENT_TYPE_SHADOW_DIR;
    case VERTEX_SHADOW_FILE:
      return FAB_FSENT_TYPE_SHADOW_FILE;
    case VERTEX_SHADOW_LINK:
      return FAB_FSENT_TYPE_SHADOW_LINK;
    case VERTEX_SHADOW_MODULE:
      return FAB_FSENT_TYPE_SHADOW_MODULE;
    case VERTEX_SHADOW_MODULES:
      return FAB_FSENT_TYPE_SHADOW_MODULES;
    default:
      return 0;
  }
}

fab_fsent_state vertex_state_remap(vertex_state state)
{
  switch(state) {
    case VERTEX_OK:
      return FAB_FSENT_STATE_FRESH;
    case VERTEX_INVALID:
      return FAB_FSENT_STATE_STALE;
    case VERTEX_UNCREATED:
      return FAB_FSENT_STATE_UNCREATED;
    case VERTEX_UNLINKED:
      return FAB_FSENT_STATE_UNLINKED;
  }

  return 0;
}

void graph_hyperconnect(
    moria_connect_context * restrict ctx
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_edge * restrict e
  , uint32_t attrs
)
{
  moria_connect_hyper(ctx, &g_graph, e, Alist, Alen, Blist, Blen, attrs);
}

void graph_connect(
    moria_connect_context * restrict ctx
  , moria_vertex * restrict A
  , moria_vertex * restrict B
  , moria_edge * restrict e
  , uint32_t attrs
)
{
  moria_connect(ctx, &g_graph, e, A, B, attrs);
}

void graph_disconnect(moria_edge * restrict e)
{
  moria_edge_disconnect(&g_graph, e);
}

void graph_disintegrate(moria_edge * restrict e, graph_invalidation_context * restrict invalidation)
{
  moria_vertex *v;
  fsent *n, *Bn;
  int x;
  llist edges;
  llist vertices;

  RUNTIME_ASSERT((e->attrs & EDGE_KIND_OPT) == EDGE_FSTREE || (e->attrs & EDGE_KIND_OPT) == EDGE_DEPENDS);

  /* initialize work lists */
  llist_init_node(&edges);
  llist_init_node(&vertices);

  llist_append(&edges, e, lln);
  while(!llist_empty(&edges) || !llist_empty(&vertices))
  {
    if((e = llist_first(&edges, typeof(*e), lln)))
    {
      llist_delete(e, lln);

      /*
       * list of nodes in the edge, in bottom-up order. The order matters because when a directory
       * node is considered, its downward edges must have already been deleted, if they are going to
       * be deleted here, so that it will be recognized as empty
       */
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(!llist_attached(e->A, lln)) {
          llist_append(&vertices, e->A, lln);
        }

        if(!llist_attached(e->B, lln)) {
          llist_append(&vertices, e->B, lln);
        }
      }
      else
      {
        for(x = 0; x < e->Alen; x++) {
          if(!llist_attached(e->Alist[x].v, lln)) {
            llist_append(&vertices, e->Alist[x].v, lln);
          }
        }

        for(x = 0; x < e->Blen; x++) {
          if(!llist_attached(e->Blist[x].v, lln)) {
            llist_append(&vertices, e->Blist[x].v, lln);
          }
        }
      }

      if((e->attrs & EDGE_TYPE_OPT) == EDGE_TYPE_FSTREE)
      {
        Bn = containerof(e->B, fsent, vertex);

        // certain nodes are never actually deleted (config nodes, project module node)
        if(e->B->attrs & VERTEX_PROTECT_BIT) {
          continue;
        }

        if(fsent_exists_get(Bn)) {
          fsent_invalidate(Bn, invalidation);
          fsent_dirnode_children_changed(containerof(e->A, fsent, vertex), invalidation);
          fsent_unlink(Bn, invalidation);
        }

        if(!rbtree_empty(&e->B->up)) {
          continue;
        }
        if(!rbtree_empty(&e->B->down)) {
          continue;
        }

        fsent_fstree_disconnecting(Bn, invalidation);
        fsedge_disconnect(containerof(e, fsedge, edge));
      }
      else
      {
        dependency_disconnect(containerof(e, dependency, edge), invalidation);
      }
    }

    if((v = llist_shift(&vertices, typeof(*v), lln)))
    {
      if(!rbtree_empty(&v->up)) {
        continue;
      }
      if(!rbtree_empty(&v->down)) {
        continue;
      }

      if(v->attrs & VERTEX_PROTECT_BIT) {
        continue;
      }

      n = containerof(v, fsent, vertex);
      if(fsent_exists_get(n)) {
        continue;
      }

      // node has no references other than its upwards facing identity edge
      if(v->up_identity) {
        if(!llist_attached(v->up_identity, lln)) {
          llist_append(&edges, v->up_identity, lln);
        }
        continue;
      }

      fsent_release(containerof(v, fsent, vertex));
    }
  }
}
