/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <fcntl.h>

#include "valyria/llist.h"
#include "narrator/fd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstdlib.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "moria/vertex.h"
#include "moria/foreach.h"
#include "moria/edge.h"
#include "moria/connect.h"

#include "build_record.h"
#include "build.h"
#include "vertex.h"
#include "exec.h"
#include "times.h"
#include "graph.h"
#include "disintegrate.h"
#include "shadow.h"
#include "edges.h"
#include "vertices.h"
#include "fsedge.h"
#include "edge.h"
#include "ref_edge.h"
#include "formula_record.h"
#include "fsent.h"
#include "vertex/probes.h"
#include "args.h"
#include "identity.h"

llist build_record_list = LLIST_INITIALIZER(build_record_list);
static llist freelist = LLIST_INITIALIZER(freelist);
static uint32_t livelist_len;

/* for a given build directory, stable sequence of build ids */
static uint64_t counter;
static uint32_t epoch;

build_record *build_record_alloc()
{
  build_record *rec;
  identity_ctx id;

  if((rec = llist_shift(&freelist, typeof(*rec), vertex.owner))) {
    ASAN_CLEAN(rec);
    memset(rec, 0, sizeof(*rec));
  } else {
    xmalloc(&rec, sizeof(*rec));
  }

  rec->counter = counter++;

  identity_init(&id);
  build_record_compute_id(rec, &id);
  rec->id_len = identity_mk(&id, rec->id, sizeof(rec->id));

  moria_vertex_initw(&rec->vertex, &g_graph, VERTEX_BUILD_RECORD | VERTEX_OK, rec->id, rec->id_len);

  llist_append(&build_record_list, &rec->vertex.owner);
  livelist_len++;

  shadow_mkref(g_shadow_build_records, rec->id, rec->id_len, &rec->vertex);

  vertex_alloc_probe(&rec->vertex);

  return rec;
}

void build_record_release(build_record *rec)
{
  moria_vertex __attribute__((unused)) *v;

  /* must have been fully excised from the graph previously */
  v = &rec->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&rec->vertex.owner);
  llist_append(&freelist, &rec->vertex.owner);
  ASAN_TAINT(rec);
}

void build_record_setup()
{
}

void build_record_cleanup()
{
  build_record *rec;

  while((rec = llist_shift(&build_record_list, typeof(*rec), vertex.owner))) {
    graph_demolish(&rec->vertex);
  }

  while((rec = llist_shift(&freelist, typeof(*rec), vertex.owner))) {
    ASAN_CLEAN(rec);
    free(rec);
  }
}

void build_record_describe(const build_record * restrict rec, bacon_writer * restrict builder_writer)
{
  char buf[128];

  { MAPPING
    vertex_path_describe(&rec->vertex, builder_writer);
    { SET
      vertex_mappings(&rec->vertex, builder_writer);
      { MAPPING STRING("token") HEX64(rec->token) }
      { MAPPING STRING("label") BYTES(rec->label, rec->label_len) }
      { MAPPING STRING("pid") UINT(rec->pid) }
      { MAPPING STRING("autorun") BOOL(rec->autorun) }

      { MAPPING STRING("runtime")
        timespec_ns_cat(buf, sizeof(buf), timespec_delta(&rec->start_time, &rec->end_time));
        STRING(buf)
      }

      vertex_relations_describe(&rec->vertex, builder_writer);
    }
  }
}

void build_record_identity_describe(const build_record * restrict rec, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(rec->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id");
    BYTES(rec->id, rec->id_len);
  }
  { MAPPING
    STRING("order");
    BYTES(rec->order, rec->order_len);
  }
}

size_t build_record_path_cat(void * restrict dst, size_t sz, const build_record * restrict rec)
{
  return zcatf(dst, sz, "//bm/build-records/%.*s", rec->id_len, rec->id);
}

void build_record_stats_build(build_record * restrict rec, bool reset, struct bacon_writer * restrict writer)
{

}

void build_record_add(build_record * restrict rec)
{
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  graph_invalidation_context invalidation = { };
  fsent *link;
  uint16_t x;
  moria_foreach_state st;
  moria_edge *edge;

  epoch++;

  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);
  graph_invalidation_begin(&invalidation);

  /* reap aged-out builds */
  while (livelist_len > build_system_retention_max_entries) {
    rec = llist_shift(&build_record_list, typeof(*rec), vertex.owner);
    livelist_len--;

    vertices_push(&unlinking, &rec->vertex);
  }

  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);
  graph_invalidation_end(&invalidation);

  /* renumber byorder refs, reverse chronological */
  x = 0;
  llist_foreach_reverse(&build_record_list, rec, vertex.owner) {
    rec->order_len = zcatf(rec->order, sizeof(rec->order), "%05hu", x);
    shadow_link_noref(g_shadow_build_records_byorder, rec->order, rec->order_len, &rec->vertex, &link);
    link->refmark = epoch;
    x++;
  }

  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);
  graph_invalidation_begin(&invalidation);

  /* reap byorder refs not visited which are now stale */
  moria_foreach_edge_down(&st, &g_shadow_build_records_byorder->vertex, edge) {
    link = containerof(edge->B, typeof(*link), vertex);
    if(link->refmark == epoch) {
      continue;
    }

    vertices_push(&unlinking, &link->vertex);
  }

  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);
  graph_invalidation_end(&invalidation);
}

void build_record_unlink(build_record * restrict rec, struct graph_disintegrate_context * restrict ctx)
{

}

void build_record_id(const struct build_record * restrict build_record, const char ** restrict id, uint16_t * restrict idl)
{
  *id = build_record->id;
  *idl = build_record->id_len;
}

void build_record_compute_id(const build_record * restrict rec, identity_ctx * restrict id)
{
  char str[256];
  uint16_t str_len;

  str_len = zcatf(str, sizeof(str), "%010u", rec->counter);

  identity_add(id, g_args.bamd_id, g_args.bamd_id_len);
  identity_add(id, "\n", 1);
  identity_name(id, VERTEX_BUILD_RECORD, str, str_len);
}

#if CONFIG_DEVEL
void build_records_show()
{
  build_record *rec;
  int x = 0;

tracef("build-records %zu live %zu free", llist_count(&build_record_list), llist_count(&freelist));
  llist_foreach(&build_record_list, rec, vertex.owner) {
    dprintf(1, "%3u ", x);
    vertex_identity_say(&rec->vertex, g_narrator_stdout);
    dprintf(1, "\n");
  }
}
#endif
