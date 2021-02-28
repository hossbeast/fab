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

#include "valyria/map.h"
#include "valyria/dictionary.h"
#include "valyria/set.h"
#include "valyria/hashtable.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/edge.h"
#include "narrator.h"
#include "fab/fsent.h"
#include "value/writer.h"

#include "graph.h"
#include "fsent.h"
#include "rule.h"
#include "module.h"
#include "node_operations.h"
#include "logging.h"
#include "params.h"
#include "stats.h"
#include "zbuffer.h"
#include "dependency.h"
#include "rule_module.h"
#include "fsedge.h"

#include "common/attrs.h"
#include "locks.h"

static llist edge_list = LLIST_INITIALIZER(edge_list);              // active
static llist edge_freelist = LLIST_INITIALIZER(edge_freelist);      // freelist

moria_graph g_graph = MORIA_GRAPH_INITIALIZER(g_graph);
hashtable * g_graph_mm;

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


attrs16 * graph_edge_direction_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, y) + 1
    num : 0 EDGE_DIRECTION_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT16_C(y), range : EDGE_DIRECTION_OPT },
EDGE_DIRECTION_TABLE
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
  attrs16_init(graph_edge_direction_attrs);
  attrs32_init(graph_vertex_kind_attrs);
  attrs32_init(graph_vertex_state_attrs);
//  attrs32_init(graph_node_attrs);
}

//
// public
//

xapi graph_edge_alloc(moria_edge ** ep)
{
  enter;

  moria_edge *e;

  if((e = llist_shift(&edge_freelist, typeof(*e), owner)) == 0)
  {
    fatal(xmalloc, &e, sizeof(*e));
  }

  moria_edge_init(e, &g_graph);
  llist_append(&edge_list, e, owner);

  *ep = e;

  finally : coda;
}

void graph_edge_release(struct moria_edge * restrict e)
{
  RUNTIME_ASSERT(e->attrs == EDGE_RULE_FML || e->attrs == EDGE_MOD_DIR);

  llist_delete_node(&e->owner);
  llist_append(&edge_freelist, e, owner);
}

xapi graph_setup()
{
  enter;

  fatal(hashtable_createx,
      &g_graph_mm
    , sizeof(moria_vertex_entry)
    , 100
    , moria_vertex_entry_hash
    , moria_vertex_entry_cmp
    , 0
    , 0
  );

  finally : coda;
}

xapi graph_cleanup()
{
  enter;

  moria_edge *e;
  llist *T;

  moria_graph_destroy(&g_graph);
  fatal(hashtable_xfree, g_graph_mm);

  llist_splice_head(&edge_freelist, &edge_list);
  llist_foreach_safe(&edge_freelist, e, owner, T) {
    wfree(e);
  }

  finally : coda;
}

static xapi graph_vertex_write(value_writer * restrict writer, const moria_vertex * restrict v)
{
  enter;

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

  fatal(value_writer_bytes, writer, name, z);

  finally : coda;
}

xapi graph_edge_say(const moria_edge * restrict e, narrator * restrict N)
{
  enter;

  int x;
  dependency *ne = 0;
  value_writer writer;
  char buf[64];
  size_t z;

  value_writer_init(&writer);
  fatal(value_writer_open, &writer, N);

  if(e->attrs & EDGE_DEPENDENCY) {
    ne = containerof(e, typeof(*ne), edge);
  }

  if(!(e->attrs & MORIA_EDGE_HYPER) || e->Alen)
  {
    fatal(value_writer_push_set, &writer);
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "up");
    fatal(value_writer_push_set, &writer);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      fatal(graph_vertex_write, &writer, e->A);
    }
    else
    {
      for(x = 0; x < e->Alen; x++)
      {
        fatal(graph_vertex_write, &writer, e->Alist[x].v);
      }
    }
    fatal(value_writer_pop_set, &writer);
    fatal(value_writer_pop_mapping, &writer);
  }

  fatal(value_writer_push_mapping, &writer);
  fatal(value_writer_string, &writer, "relation");
  z = znload_attrs32(buf, sizeof(buf), graph_edge_attrs, e->attrs);
  fatal(value_writer_bytes, &writer, buf, z);
  fatal(value_writer_pop_mapping, &writer);

  if(!(e->attrs & MORIA_EDGE_HYPER) || e->Blen)
  {
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "down");
    fatal(value_writer_push_set, &writer);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      fatal(graph_vertex_write, &writer, e->B);
    }
    else
    {
      for(x = 0; x < e->Blen; x++)
      {
        fatal(graph_vertex_write, &writer, e->Blist[x].v);
      }
    }
    fatal(value_writer_pop_set, &writer);
    fatal(value_writer_pop_mapping, &writer);
  }

  if(ne && ne->fml)
  {
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "fml");
    fatal(graph_vertex_write, &writer, &ne->fml->vertex);
    fatal(value_writer_pop_mapping, &writer);
  }
  fatal(value_writer_mapping_string_bool, &writer, "hyper", e->attrs & MORIA_EDGE_HYPER);

  char foo[64];
  snprintf(foo, sizeof(foo), "0x%016lx", (uintptr_t)e);
  fatal(value_writer_mapping_string_string, &writer, "addr", foo);

  fatal(value_writer_pop_set, &writer);
  fatal(value_writer_close, &writer);

  finally : coda;
}

xapi graph_invalidation_begin(graph_invalidation_context * restrict invalidation)
{
  enter;

  fatal(moria_vertex_traversal_begin, &g_graph, &invalidation->vertex_traversal);
  fatal(moria_edge_traversal_begin, &g_graph, &invalidation->edge_traversal);
  invalidation->any = false;

  finally : coda;
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
   switch((uint32_t)kind) {
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
  }

  return 0;
}

fab_fsent_state vertex_state_remap(vertex_state state)
{
  switch((uint32_t)state) {
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

xapi graph_hyperconnect(
    moria_connect_context * restrict ctx
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_edge * restrict e
  , uint32_t attrs
)
{
  enter;

  moria_connect_hyper(ctx, &g_graph, e, Alist, Alen, Blist, Blen, attrs);

//  printf("connect ");
//  fatal(graph_edge_say, e, g_narrator_stdout);
//  printf("\n");

  finally : coda;
}

xapi graph_connect(
    moria_connect_context * restrict ctx
  , moria_vertex * restrict A
  , moria_vertex * restrict B
  , moria_edge * restrict e
  , uint32_t attrs
)
{
  enter;

  moria_connect(ctx, &g_graph, e, A, B, attrs);

#if 0
  uint64_t cat = 0;

  cat = 0;
  if((attrs & EDGE_TYPE_OPT) == EDGE_TYPE_FSTREE) {
    cat = L_FSGRAPH;
  } else if (attrs & EDGE_DEPENDENCY) {
    cat = L_DEPGRAPH;
  } else if (attrs & EDGE_MODULES) {
    cat = L_MODULE;
  }

  if(attrs)
  {
    printf("connect ");
    fatal(graph_edge_say, e, g_narrator_stdout);
    printf("\n");
  }
#endif

//  printf("connect ");
//  fatal(graph_edge_say, e, g_narrator_stdout);
//  printf("\n");

  finally : coda;
}

xapi graph_disconnect(moria_edge * restrict e)
{
  enter;

//  printf("disconnect ");
//  fatal(graph_edge_say, e, g_narrator_stdout);
//  printf("\n");

  fatal(moria_edge_disconnect, &g_graph, e);

  finally : coda;
}

xapi graph_disintegrate(moria_edge * restrict e, graph_invalidation_context * restrict invalidation)
{
  enter;

  moria_vertex *v;
  fsent *n;
  int x;
  llist edges;
  llist vertices;
  edge_kind ek;

  ek = e->attrs & EDGE_KIND_OPT;
  RUNTIME_ASSERT(ek == EDGE_FSTREE || ek == EDGE_DEPENDS);

  /* initialize work lists */
  llist_init_node(&edges);
  llist_init_node(&vertices);

  llist_append(&edges, e, lln);
  while(!llist_empty(&edges) || !llist_empty(&vertices))
  {
//printf("\n");
//printf("edges %zu\n", llist_count(&edges));
//printf("nodes %zu\n", llist_count(&vertices));
    if((e = llist_first(&edges, typeof(*e), lln)))
    {
      llist_delete(e, lln);

//printf(" e ");
//fatal(graph_edge_say, e, g_narrator_stdout);
//printf(" ");

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
        n = containerof(e->B, fsent, vertex);

        // some nodes are never actually deleted (config nodes, project module node)
        if(e->B->attrs & VERTEX_PROTECT_BIT) {
//printf("%d", __LINE__);
          goto nexte;
        }

        if(fsent_exists_get(n))
        {
          /* opportunistically deleted related edges */
          fatal(fsent_unlinking, n, invalidation);
        }

        /* cant unlink from fstree while any references exist */
        if(!rbtree_empty(&e->B->down)) {
//printf("%d", __LINE__);
          goto nexte;
        }

        /* cant unlink from fstree while any references exist */
        if(!rbtree_empty(&e->B->up)) {
//printf("%d", __LINE__);
          goto nexte;
        }

        if(fsent_exists_get(n))
        {
          /* the delete event is fired when the node becomes unlinked - it may not actually be released yet */
          fatal(fsent_unlink, n, invalidation);
          fatal(fsent_dirnode_children_changed, containerof(e->A, fsent, vertex), invalidation);
          fsent_state_set(n, VERTEX_UNLINKED);
        }

      }

      ek = e->attrs & EDGE_KIND_OPT;
      if(ek == EDGE_FSTREE) {
        fatal(fsedge_disconnect, containerof(e, fsedge, edge));
      } else if(ek == EDGE_DEPENDS) {
        fatal(dependency_disconnect, containerof(e, dependency, edge), invalidation);
      }
nexte:
//printf("\n");
continue;
    }

    if((v = llist_shift(&vertices, typeof(*v), lln)))
    {
//printf(" v ");
//fatal(fsent_absolute_path_say, containerof(v, fsent, vertex), g_narrator_stdout);
//printf(" ");

      if(!rbtree_empty(&v->down)) {
//printf("%d", __LINE__);
        goto nextv;
      }

      if(v->attrs & VERTEX_PROTECT_BIT) {
//printf("%d", __LINE__);
        goto nextv;
      }

      n = containerof(v, fsent, vertex);
      if(fsent_exists_get(n)) {
//printf("%d", __LINE__);
        goto nextv;
      }

      // node is still referenced, other than by its upwards facing identity edge
      if(!rbtree_empty(&v->up)) {
//printf("%d", __LINE__);
        goto nextv;
      }

      if(v->up_identity) {
        if(!llist_attached(v->up_identity, lln)) {
          llist_append(&edges, v->up_identity, lln);
        }
//printf("%d", __LINE__);
        goto nextv;
      }

//printf("deleted %p", n);
      fsent_release(containerof(v, fsent, vertex));
nextv:
//printf("\n");
continue;
    }
  }

  finally : coda;
}

#if 0
        /* only actually unlink from the fstree if *no* other references exist */
        if(!rbtree_empty(&e->B->down) || !rbtree_empty(&e->B->up)) {
//printf("%d up %s down %s", __LINE__, rbtree_empty(&e->B->up) ? "empty" : "nonempty", rbtree_empty(&e->B->down) ? "empty" : "nonempty");

printf("%d", __LINE__);
          goto nexte;
        }

attrs32 * graph_node_attrs = (attrs32[]) {{
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
#endif
