/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/graph.h"
#include "moria/foreach.h"
#include "moria/connect.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "index_vertex.h"
#include "index_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "stats.h"
#include "filesystem.h"
#include "vertex/probes.h"
#include "entlist.h"

llist index_vertex_list = LLIST_INITIALIZER(index_vertex_list);                   // active fsents
static llist index_vertex_freelist = LLIST_INITIALIZER(index_vertex_freelist);    // free fsents

index_vertex *g_index;

static void index_vertex_alloc(index_vertex ** restrict vp, const char * restrict label, uint16_t label_len)
{
  index_vertex *n;

  RUNTIME_ASSERT(label_len);

  if((n = llist_shift(&index_vertex_freelist, typeof(*n), vertex.owner)) == 0) {
    xmalloc(&n, sizeof(*n));
  } else {
    memset(n, 0, sizeof(*n));
  }

  fsname_init(&n->name, label, label_len);
  moria_vertex_initw(&n->vertex, &g_graph, VERTEX_INDEX, n->name.name, n->name.namel);

  llist_append(&index_vertex_list, &n->vertex.owner);

  *vp = n;
}

static void __attribute__((nonnull)) index_vertex_dispose(index_vertex * restrict n)
{
  llist_delete(&n->vertex.owner);
  llist_append(&index_vertex_freelist, &n->vertex.owner);
}

void index_vertex_release(index_vertex * restrict n, graph_invalidation_context * restrict invalidation)
{
  moria_foreach_state st;
  entlist edges, vertices;
  moria_edge *edge;
  moria_vertex __attribute__((unused)) *v;

  /* proactive edge disconnect */
  entlist_init(&edges);
  moria_foreach_edge_up(&st, &n->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_INDEX_DIR:
      case EDGE_INDEX_RMA:
        entlist_push(&edges, edge);
        break;
      default:
    }
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  /* should be fully excised from the graph previously */
  v = &n->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  vertex_deleted_probe(&n->vertex);
  index_vertex_dispose(n);
}

void index_vertex_setup(void)
{
  index_vertex_alloc(&g_index, MMS("(index)"));
  g_index->hash = 0xbb3b;
}

void index_vertex_cleanup(void)
{
  llist *T;
  index_vertex *n;

  llist_foreach_safe(&index_vertex_list, n, vertex.owner, T) {
    index_vertex_dispose(n);
  }

  llist_foreach_safe(&index_vertex_freelist, n, vertex.owner, T) {
    free(n);
  }
}

index_vertex *index_vertex_create(const char * restrict label, uint16_t label_len)
{
  index_vertex *index;
  int __attribute__((unused)) r;
  moria_connect_context ctx;
  index_edge *ie;

  index_vertex_alloc(&index, label, label_len);
  r = moria_preconnect(&ctx, &g_graph, &index->vertex, &g_index->vertex, EDGE_INDEX_DIR, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  index_edge_alloc(&ie);
  moria_connect(&ctx, &g_graph, &ie->edge, &index->vertex, &g_index->vertex, EDGE_INDEX_DIR);

  return index;
}

index_vertex *index_vertex_lookup(const char * restrict label, uint16_t label_len)
{
  index_vertex *index;
  moria_vertex *v;

  if((v = moria_vertex_downw(&g_index->vertex, label, label_len))) {
    index = containerof(v, typeof(*index), vertex);
    return index;
  }

  return 0;
}
