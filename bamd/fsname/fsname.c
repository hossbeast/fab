/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/graph.h"
#include "moria/foreach.h"
#include "moria/connect.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "fsname.h"
#include "fsindex_edge.h"
#include "fsent.h"
#include "fsedge.h"
#include "vertex.h"
#include "edge.h"
#include "stats.h"
#include "filesystem.h"
#include "vertex/probes.h"
#include "entlist.h"

#include "zcat.h"

llist fsname_list = LLIST_INITIALIZER(fsname_list);                   // active fsents
static llist fsname_freelist = LLIST_INITIALIZER(fsname_freelist);    // free fsents

static void __attribute__((nonnull)) fsname_dispose(fsname * restrict n)
{
  llist_delete(&n->vertex.owner);
  llist_append(&fsname_freelist, &n->vertex.owner);
}

fsname *fsname_alloc(const char * restrict label, uint16_t label_len)
{
  fsname *n;

  RUNTIME_ASSERT(label_len);

  if((n = llist_shift(&fsname_freelist, typeof(*n), vertex.owner)) == 0) {
    xmalloc(&n, sizeof(*n));
  } else {
    memset(n, 0, sizeof(*n));
  }

  n->name_len = zcatw(n->name, sizeof(n->name), label, label_len);
  moria_vertex_initw(&n->vertex, &g_graph, VERTEX_FSNAME | VERTEX_OK, n->name, n->name_len);

  llist_append(&fsname_list, &n->vertex.owner);

  return n;
}

void fsname_release(fsname * restrict entry, graph_invalidation_context * restrict invalidation)
{
  moria_foreach_state st;
  entlist edges, vertices;
  moria_edge *edge;
  moria_vertex __attribute__((unused)) *v;
  int x;

  /* proactive edge disconnect */
  entlist_init(&edges);
  moria_foreach_edge(&st, &entry->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_DIR_FSNAME:
      case EDGE_FSNAME_RMA:
      case EDGE_FSNAME_FSINDEX:
      case EDGE_FSNAME_MOD:
        entlist_push(&edges, edge);
        break;
      default:
        break;
    }
  }

  for(x = 0; x < ARRAY_LEN(entry->links); x++) {
    if(entry->links[x]) {
      entlist_push(&edges, &entry->links[x]->edge);
    }
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  /* should be fully excised from the graph previously */
  v = &entry->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  vertex_deleted_probe(&entry->vertex);
  fsname_dispose(entry);
}

void fsname_setup(void)
{
}

void fsname_cleanup(void)
{
  llist *T;
  fsname *n;

  llist_foreach_safe(&fsname_list, n, vertex.owner, T) {
    fsname_dispose(n);
  }

  llist_foreach_safe(&fsname_freelist, n, vertex.owner, T) {
    free(n);
  }
}

size_t fsname_path_cat(void * restrict dst, size_t sz, const fsname * restrict entry)
{
  return zcatf(dst, sz, "//fsnames/%.*s", entry->id_len, entry->id);
}

void fsname_stats_build(fsname * restrict entry, bool reset, struct bacon_writer * restrict builder_writer)
{

}

void fsname_describe(const fsname * restrict entry, struct bacon_writer * restrict builder_writer)
{
  moria_edge *e;
  entlist list, *eln;

  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&entry->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);

    { SET
      { MAPPING
        STRING("attrs")
        attrs32_describe(entry->vertex.attrs, graph_vertex_attrs, builder_writer);
      }
      { MAPPING
        STRING("id")
        BYTES(entry->id, entry->id_len)
      }
      { MAPPING
        STRING("name")
        BYTES(entry->name, entry->name_len)
      }
      if(edges_gather_down(&entry->vertex, EDGE_DIR_FSNAME, &list)) {
        MAPPING STRING("dirs")
        { SET
          entlist_foreach(&list, e, eln) {
            vertex_path_describe(e->B, builder_writer);
          }
        }
        entlist_destroy(&list);
      }
      if(edges_gather_up(&entry->vertex, EDGE_FSNAME_RMA, &list)) {
        MAPPING STRING("rmas")
        { SET
          entlist_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        entlist_destroy(&list);
      }
      if(edges_gather_up(&entry->vertex, EDGE_FSNAME_MOD, &list)) {
        MAPPING STRING("modules")
        { SET
          entlist_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        entlist_destroy(&list);
      }
    }
  }
}

void fsname_identity_say(const struct fsname * restrict entry, struct narrator * restrict N)
{
  attrs32_say(entry->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s label %.*s", entry->id_len, entry->id, entry->vertex.label_len, entry->vertex.label);
}
