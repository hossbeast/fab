/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/graph.h"
#include "moria/connect.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "moria/foreach.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "common/hash.h"

#include "fsindex.h"
#include "fsname.h"
#include "fsindex_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "fstree.h"
#include "shadow.h"

#include "zcat.h"

static union stats_fsindex fsindex_stats;
moria_vertex *g_fsindex = (moria_vertex[]){{ }};
char g_fsindex_id[40];
uint16_t g_fsindex_id_len;

static fsname *fsname_create(const char * restrict label, uint16_t label_len)
{
  fsname *entry;
  int __attribute__((unused)) r;
  moria_connect_context ctx;
  fsindex_edge *ie;
  uint64_t u64;

  entry = fsname_alloc(label, label_len);
  r = moria_preconnect(&ctx, &g_graph, &entry->vertex, g_fsindex, EDGE_FSNAME_FSINDEX, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  ie = fsindex_edge_alloc();
  moria_connect(&ctx, &g_graph, &ie->edge, &entry->vertex, g_fsindex, EDGE_FSNAME_FSINDEX);

  u64 = hash64(0x44aa123110, label, label_len);
  entry->id_len = zcatf(entry->id, sizeof(entry->id), "%016"PRIx64, u64);

  /* shadow link to the special node itself */
  shadow_link(g_shadow_fsnames, entry->id, entry->id_len, &entry->vertex, &entry->links[0]);
  shadow_link(g_shadow_fsnames_byname, entry->name, entry->name_len, &entry->vertex, &entry->links[1]);

  return entry;
}

fsname *fsindex_miss(const char * restrict label, uint16_t label_len)
{
  return fsname_create(label, label_len);
}

void fsindex_put(fsent * restrict n, graph_invalidation_context * restrict invalidation)
{
  const fstree *fst;
  fsname *entry;
  int __attribute__((unused)) r;
  moria_connect_context ctx;
  fsindex_edge *ie;

  /* already indexed */
  if(n->ie) {
    return;
  }

  /* only in indexed filesystems */
  fst = vertex_fstree_get(&n->vertex);
  if(!fst->index) {
    return;
  }

  /* only certain types of nodes are indexed */
  switch(vertex_type_get(&n->vertex)) {
    case VERTEX_DIR:
      break;
    default:
      return;
  }

  fsindex_stats.put++;

  if(!(entry = fsindex_lookup(n->vertex.label, n->vertex.label_len))) {
    fsindex_stats.count++;
    entry = fsname_create(n->vertex.label, n->vertex.label_len);
  }

  r = moria_preconnect(&ctx, &g_graph, &n->vertex, &entry->vertex, EDGE_DIR_FSNAME, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  ie = fsindex_edge_alloc();
  moria_connect(&ctx, &g_graph, &ie->edge, &n->vertex, &entry->vertex, EDGE_DIR_FSNAME);

  vertex_invalidate(&entry->vertex, invalidation);

  entry->down++;
  n->ie = ie;
}

void fsindex_del(fsent * restrict n, graph_invalidation_context * restrict invalidation)
{
  fsname *entry;

  /* not indexed */
  if(!n->ie) {
    return;
  }

  fsindex_stats.del++;

  entry = containerof(n->ie->edge.A, typeof(*entry), vertex);
  moria_edge_disconnect(&g_graph, &n->ie->edge);
  fsindex_edge_release(n->ie);
  n->ie = 0;

  vertex_invalidate(&entry->vertex, invalidation);

  entry->down--;
  if(entry->down == 0) {
    fsindex_stats.count--;
    fsname_release(entry, invalidation);
  }
}

fsname *fsindex_lookup(const char * restrict label, uint16_t label_len)
{
  fsname *entry;
  moria_vertex *v;

  if((v = moria_vertex_downw(g_fsindex, label, label_len))) {
    entry = containerof(v, typeof(*entry), vertex);
    return entry;
  }

  return 0;
}

void fsindex_stats_build(bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&fsindex_stats.record, &descriptor_stats_fsindex, builder_writer);
}

void fsindex_describe(struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(g_fsindex, builder_writer);
    { SET
      vertex_mappings(g_fsindex, builder_writer);
      { MAPPING
        STRING("stats")
        { SET
          fsindex_stats_build(false, builder_writer);
        }
      }
    }
  }
}

void fsindex_show(void)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  fsindex_describe(builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void fsindex_setup(void)
{
  moria_vertex_initw(g_fsindex, &g_graph, VERTEX_FSINDEX | VERTEX_OK, MMS("(index)"));
  g_fsindex_id_len = zcatf(g_fsindex_id, sizeof(g_fsindex_id), "%016"PRIx64, hash64(0x5678, MMS("(index)")));
}

void fsindex_cleanup(void)
{

}

size_t fsindex_id_cat(char *dst, size_t sz)
{
  return zcatw(dst, sz, g_fsindex_id, g_fsindex_id_len);
}
