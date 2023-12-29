/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/graph.h"
#include "moria/connect.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "moria/foreach.h"
#include "narrator.h"

#include "fsent_index.h"
#include "index_vertex.h"
#include "index_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "filesystem.h"

uint32_t fsent_index_count;

void fsent_index_put(fsent * restrict n, graph_invalidation_context * restrict invalidation)
{
  const filesystem *fs;
  index_vertex *index;
  int __attribute__((unused)) r;
  moria_connect_context ctx;
  index_edge *ie;

  /* already indexed */
  if(n->ie) {
    return;
  }

  /* only in indexed filesystems */
  fs = fsent_filesystem_get(n);
  if(!fs->indexed) {
    return;
  }

  /* only certain types of nodes are indexed */
  switch(vertex_type_get(&n->vertex)) {
    case VERTEX_DIR:
      break;
    default:
      return;
  }

  STATS_INC(g_stats.fsent_index_put);

  if(!(index = index_vertex_lookup(n->vertex.label, n->vertex.label_len))) {
    fsent_index_count++;
    index = index_vertex_create(n->vertex.label, n->vertex.label_len);
  }

  r = moria_preconnect(&ctx, &g_graph, &n->vertex, &index->vertex, EDGE_INDEX, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  index_edge_alloc(&ie);
  moria_connect(&ctx, &g_graph, &ie->edge, &n->vertex, &index->vertex, EDGE_INDEX);

  vertex_invalidate(&index->vertex, invalidation);

  index->down++;
  n->ie = ie;
}

void fsent_index_del(fsent * restrict n, graph_invalidation_context * restrict invalidation)
{
  index_vertex *index;

  /* not indexed */
  if(!n->ie) {
    return;
  }

  STATS_INC(g_stats.fsent_index_del);

  index = containerof(n->ie->edge.A, typeof(*index), vertex);
  moria_edge_disconnect(&g_graph, &n->ie->edge);
  index_edge_release(n->ie);
  n->ie = 0;

  vertex_invalidate(&index->vertex, invalidation);

  index->down--;
  if(index->down == 0) {
    fsent_index_count--;
    index_vertex_release(index, invalidation);
  }
}

void fsent_index_describe(struct bacon_writer * restrict builder_writer)
{
  moria_foreach_state st, st2;
  moria_edge *e, *e2;
  index_vertex *iv;

  { SET
    { MAPPING STRING("fsent-index-count") UINT(fsent_index_count) }
    { MAPPING STRING("fsent-index-put") UINT(g_stats.fsent_index_put) }
    { MAPPING STRING("fsent-index-del") UINT(g_stats.fsent_index_del) }
    moria_foreach_edge_down(&st, &g_index->vertex, e) {
      iv = containerof(e->B, typeof(*iv), vertex);
      { MAPPING
        BYTES(iv->vertex.label, iv->vertex.label_len)
        { SET
          moria_foreach_edge_down(&st2, &iv->vertex, e2) {
            vertex_identity_describe(e2->B, builder_writer);
          }
        }
      }
    }
  }
}

void fsent_index_show(void)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  fsent_index_describe(builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void fsent_index_setup(void)
{
}

void fsent_index_cleanup(void)
{

}
