/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/foreach.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"

#include "fsname.h"
#include "ns_edge.h"
#include "vertex.h"
#include "edge.h"
#include "stats.h"
#include "vertex/probes.h"
#include "edges.h"
#include "namespace.h"
#include "identity.h"
#include "shadow.h"
#include "disintegrate.h"

#include "zcat.h"

llist fsname_list = LLIST_INITIALIZER(fsname_list);                   // active fsents
static llist fsname_freelist = LLIST_INITIALIZER(fsname_freelist);    // free fsents

static void __attribute__((nonnull)) fsname_dispose(fsname * restrict n)
{
  llist_delete(&n->vertex.owner);
  llist_append(&fsname_freelist, &n->vertex.owner);
  ASAN_TAINT(n);
}

static fsname *fsname_alloc(const char * restrict label, uint16_t label_len)
{
  fsname *fsn;

  RUNTIME_ASSERT(label_len);

  if((fsn = llist_shift(&fsname_freelist, typeof(*fsn), vertex.owner))) {
    ASAN_CLEAN(fsn);
    memset(fsn, 0, sizeof(*fsn));
  } else {
    xmalloc(&fsn, sizeof(*fsn));
  }

  fsn->name_len = zcatw(fsn->name, sizeof(fsn->name), label, label_len);
  moria_vertex_initw(&fsn->vertex, &g_graph, VERTEX_FSNAME | VERTEX_OK, fsn->name, fsn->name_len);

  vertex_alloc_probe(&fsn->vertex);
  llist_append(&fsname_list, &fsn->vertex.owner);

  return fsn;
}

fsname *fsname_create(namespace * restrict ns, const char * restrict label, uint16_t label_len)
{
  identity_ctx id;
  fsname *fsn;

  fsn = fsname_alloc(label, label_len);
  fsn->ns = ns;

  identity_init(&id);
  fsname_compute_id(fsn, &id);
  fsn->id_len = identity_mk(&id, fsn->id, sizeof(fsn->id));

  ns_connect(&fsn->vertex, &ns->vertex, EDGE_FSNAME_NS);
  vertex_alloc_probe(&fsn->vertex);

  /* setup links */
  shadow_mkref(g_shadow_fsnames, fsn->id, fsn->id_len, &fsn->vertex);
  shadow_mkref(ns->shadow_fsnames, fsn->id, fsn->id_len, &fsn->vertex);
  shadow_mkref(ns->shadow_fsnames_byname, fsn->name, fsn->name_len, &fsn->vertex);

  return fsn;
}

void fsname_release(fsname * restrict entry)
{
  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
  v = &entry->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  fsname_dispose(entry);
}

void fsname_setup(void)
{
}

void fsname_cleanup(void)
{
  fsname *n;

  while((n = llist_shift(&fsname_freelist, typeof(*n), vertex.owner))) {
    ASAN_CLEAN(n);
    free(n);
  }
}

size_t fsname_path_cat(void * restrict dst, size_t sz, const fsname * restrict entry)
{
  return zcatf(dst, sz, "//bm/fsnames/%.*s", entry->id_len, entry->id);
}

void fsname_stats_build(const fsname * restrict entry, bool reset, struct bacon_writer * restrict builder_writer)
{
  stats_build(&entry->fsname_stats.record, &descriptor_stats_fsname, builder_writer);
}

void fsname_describe(const fsname * restrict entry, struct bacon_writer * restrict builder_writer)
{
  moria_edge *e;
  edges list, *eln;

  { MAPPING
    vertex_path_describe(&entry->vertex, builder_writer);
    { SET
      vertex_mappings(&entry->vertex, builder_writer);
      { MAPPING STRING("name")
        BYTES(entry->name, entry->name_len)
      }
      if(edges_gather_up(&entry->vertex, EDGE_FSNAME_MOD, &list)) {
        MAPPING STRING("modules")
        { SET
          edges_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        edges_destroy(&list);
      }
      { MAPPING
        STRING("stats")
        { SET
          fsname_stats_build(entry, false, builder_writer);
        }
      }

      vertex_relations_describe(&entry->vertex, builder_writer);
    }
  }
}

void fsname_identity_describe(const struct fsname * restrict entry, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(entry->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(entry->id, entry->id_len);
  }
  { MAPPING
    STRING("namespace")
    if(entry->ns) {
      BYTES(entry->ns->id, entry->ns->id_len);
    } else {
      STRING("_")
    }
  }
  { MAPPING
    STRING("label")
    BYTES(entry->vertex.label, entry->vertex.label_len);
  }
}

bool fsname_reap(struct fsname * restrict fsn, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *e;

  moria_foreach_edge(&st, &fsn->vertex, e) {
    switch(edge_type_get(e)) {
      case EDGE_FSNAME_PLUGIN:
      case EDGE_FSNAME_MOD:
      case EDGE_FSNAME_RMA:
      case EDGE_DIR_FSNAME:
        return false;
      default:
        break;
    }
  }

  /* no longer referenced */
  return true;
}

void fsname_unlink(struct fsname * restrict fsname, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  moria_foreach_edge(&st, &fsname->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_DIR_FSNAME:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

void fsname_id(const struct fsname * restrict fsname, const char ** restrict id, uint16_t * restrict idl)
{
  *id = fsname->id;
  *idl = fsname->id_len;
}

void fsname_compute_id(const struct fsname * restrict fsn, struct identity_ctx * restrict id)
{
  namespace_compute_id(fsn->ns, id);
  identity_add(id, "\n", 1);
  identity_name(id, VERTEX_FSNAME, fsn->name, fsn->name_len);
}

#if CONFIG_DEVEL
void fsnames_show(void)
{
  fsname *fml;
  int x;

  printf("fsnames, live : %zu free : %zu\n", llist_count(&fsname_list), llist_count(&fsname_freelist));
  x = 0;
  llist_foreach(&fsname_list, fml, vertex.owner) {
    printf("%3u %p", x++, fml);
    printf("\n");
  }
}
#endif
