/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "moria/foreach.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "common/hash.h"

#include "bpe.h"
#include "dependency.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "formula.h"
#include "rma.h"
#include "edges.h"
#include "system.h"
#include "events/events.h"
#include "shadow.h"
#include "formula_record.h"
#include "special_edge.h"
#include "meta_edge.h"
#include "build_record.h"
#include "identity.h"
#include "disintegrate.h"

#include "zcat.h"

llist bpe_list = LLIST_INITIALIZER(bpe_list);                 // active
static llist bpe_freelist = LLIST_INITIALIZER(bpe_freelist);  // free
static hashtable bpe_cache;
static llist bpe_reconcile_list = LLIST_INITIALIZER(bpe_reconcile_list);    // struct fsent

void buildplan_entity_alloc(buildplan_entity ** restrict rv)
{
  buildplan_entity *bpe;

  if((bpe = llist_shift(&bpe_freelist, typeof(*bpe), vertex.owner))) {
    ASAN_CLEAN(bpe);
    memset(bpe, 0, sizeof(*bpe));
  } else {
    xmalloc(&bpe, sizeof(*bpe));
  }

  moria_vertex_init(&bpe->vertex, &g_graph, VERTEX_BPE | VERTEX_OK);

  llist_push(&bpe_list, &bpe->vertex.owner);
  *rv = bpe;
}

static void buildplan_entity_dispose(buildplan_entity * restrict bpe)
{
  llist_delete(&bpe->vertex.owner);
  llist_push(&bpe_freelist, &bpe->vertex.owner);
  ASAN_TAINT(bpe);
}

static uint32_t bpe_hash32(uint32_t h, const void *val)
{
  const buildplan_entity *bpe;

  bpe = val;
  return hash32(0, bpe->id, bpe->id_len);
}

static int vertices_cmp(void *ctx, llist * restrict a, llist * restrict b)
{
  const char *aid;
  uint16_t aid_len;

  const char *bid;
  uint16_t bid_len;

  vertex_id(containerof(a, vertices, lln)->p, &aid, &aid_len);
  vertex_id(containerof(b, vertices, lln)->p, &bid, &bid_len);

  return memncmp(aid, aid_len, bid, bid_len);
}

static int vertex_cmp(const void * restrict A, const void * restrict B)
{
  const moria_vertex *a;
  const char *aid;
  uint16_t aid_len;

  const moria_vertex *b;
  const char *bid;
  uint16_t bid_len;

  a = *(const moria_vertex **)A;
  vertex_id(a, &aid, &aid_len);

  b = *(const moria_vertex **)B;
  vertex_id(b, &bid, &bid_len);

  return memncmp(aid, aid_len, bid, bid_len);
}

/* construct bpe id with sources/targets specified as arrays */
static void bpe_mk_id_arrays(
    identity_ctx * restrict id
  , const rule_module_association * restrict rma
  , formula * restrict fml
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_vertex ** restrict Alist
  , uint16_t Alen
)
{
  int x;

  /* rma */
  if(rma) {
    vertex_compute_id(&rma->vertex, id);
  }
  identity_add(id, "\n", 1);

  /* fml */
  if(fml) {
    vertex_compute_id(&fml->vertex, id);
  }
  identity_add(id, "\n", 1);

  /* sources */
  qsort(Blist, Blen, sizeof(*Blist), vertex_cmp);
  for(x = 0; x < Blen; x++) {
    vertex_compute_id(Blist[x], id);
  }
  identity_add(id, "\n", 1);

  /* targets */
  qsort(Alist, Alen, sizeof(*Alist), vertex_cmp);
  for(x = 0; x < Alen; x++) {
    vertex_compute_id(Alist[x], id);
  }
  identity_add(id, "\n", 1);

  /* bpe */
  identity_type(id, VERTEX_BPE);
}

/* construct bpe id with sources/targets specified as lists */
static void bpe_mk_id_lists(
    identity_ctx * restrict id
  , const rule_module_association * restrict rma
  , const formula * restrict fml
  , vertices * restrict sources
  , vertices * restrict targets
)
{
  vertices *eln;
  const moria_vertex *v;

  /* rma */
  if(rma) {
    vertex_compute_id(&rma->vertex, id);
  }
  identity_add(id, "\n", 1);

  /* fml */
  if(fml) {
    vertex_compute_id(&fml->vertex, id);
  }
  identity_add(id, "\n", 1);

  /* sources */
  llist_sort(&sources->lln, vertices_cmp, 0);
  vertices_foreach(sources, v, eln) {
    vertex_compute_id(v, id);
  }
  identity_add(id, "\n", 1);

  /* targets */
  llist_sort(&targets->lln, vertices_cmp, 0);
  vertices_foreach(targets, v, eln) {
    vertex_compute_id(v, id);
  }
  identity_add(id, "\n", 1);

  /* bpe */
  identity_type(id, VERTEX_BPE);
}

static int search_key_cmp(const void *A, const void *K)
{
  const buildplan_entity *bpe = A;
  const struct buildplan_entity_search_context *ctx = K;

  return memncmp(bpe->id, bpe->id_len, ctx->id, ctx->id_len);
}

static int bpe_cmp(const void * _A, const void *_B)
{
  const buildplan_entity *A = _A;
  const buildplan_entity *B = _B;

  return memncmp(A->id, A->id_len, B->id, B->id_len);
}

//
// public
//

void buildplan_entity_setup()
{
  hashtable_initx(&bpe_cache, 0, bpe_hash32, bpe_cmp, 0);
}

void buildplan_entity_cleanup()
{
  buildplan_entity *bpe;

  hashtable_destroy(&bpe_cache);

  while((bpe = llist_shift(&bpe_freelist, typeof(*bpe), vertex.owner))) {
    ASAN_TAINT(bpe);
    free(bpe);
  }
}

buildplan_entity *buildplan_entity_cache_search(struct buildplan_entity_search_context * restrict ctx)
{
  identity_ctx id;
  int r;

  /* compute hash for the key */
  identity_init(&id);
  bpe_mk_id_arrays(&id, ctx->rma, ctx->fml, ctx->Blist, ctx->Blen, ctx->Alist, ctx->Alen);
  ctx->id_len = identity_mk(&id, ctx->id, sizeof(ctx->id));

  ctx->htx.hash = hash32(0, ctx->id, ctx->id_len);

  ctx->htx.entry = ctx;
  ctx->htx.cmp_fn = search_key_cmp;
  if((r = hashtable_search(&bpe_cache, &ctx->htx))) {
    return hashtable_fetch(&bpe_cache, &ctx->htx);
  }

  return 0;
}

void buildplan_entity_cache_put(struct buildplan_entity_search_context * restrict ctx, buildplan_entity * restrict bpe)
{
  ctx->htx.entry = bpe;
  hashtable_set(&bpe_cache, &ctx->htx);
}

void buildplan_entity_validate_enqueue(buildplan_entity * restrict bpe)
{
  vertices list, *eln;
  fsent *fse;
  moria_vertex *v;

  if(!bpe->fml) {
    return;
  }

  vertices_init(&list);
  buildplan_entity_targets(bpe, &list);

  vertices_foreach(&list, v, eln) {
    fse = containerof(v, typeof(*fse), vertex);
    llist_delete(&fse->bpe_reconcile_lln);
    llist_push(&bpe_reconcile_list, &fse->bpe_reconcile_lln);
  }

  vertices_destroy(&list);
}

buildplan_entity *buildplan_entity_create_hyper(
    moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , formula * restrict fml
  , rule_module_association * restrict rma
  , struct graph_invalidation_context * restrict invalidation
)
{
  buildplan_entity *bpe;
  identity_ctx id;

  buildplan_entity_alloc(&bpe);
  dependency_hyperconnect(Blist, Blen, (moria_vertex *[]) { &bpe->vertex }, 1, EDGE_SRC_BPE);
  dependency_hyperconnect((moria_vertex *[]) { &bpe->vertex }, 1, Alist, Alen, EDGE_BPE_TGT);

  /* cached */
  bpe->rma = rma;
  bpe->fml = fml;

  identity_init(&id);
  bpe_compute_id(bpe, &id);
  bpe->id_len = identity_mk(&id, bpe->id, sizeof(bpe->id));

  bpe->vertex.label = bpe->id;
  bpe->vertex.label_len = bpe->id_len;

  /* build out the shadow dir */
  shadow_create(g_shadow_bpes, &bpe->shadow, bpe->id, bpe->id_len, VERTEX_SHADOW_DIR, invalidation);

  /* enable traversal via specials graph */
  special_connect(&bpe->shadow->vertex, &bpe->vertex, EDGE_SPECIAL, invalidation);

  return bpe;
}

void buildplan_entity_source_edges(const buildplan_entity * restrict bpe, edges * restrict list)
{
  moria_edge *edge;
  moria_foreach_state st;

  moria_foreach_edge_down(&st, &bpe->vertex, edge) {
    if(edge_type_get(edge) != EDGE_SRC_BPE) {
      continue;
    }
    edges_push(list, edge);
  }
}

void buildplan_entity_sources(const buildplan_entity * restrict bpe, vertices * restrict list)
{
  edges edges, *eln;
  moria_edge *edge;
  int x;

  edges_init(&edges);
  buildplan_entity_source_edges(bpe, &edges);

  vertices_init(list);
  edges_foreach(&edges, edge, eln) {
    if(!(edge->attrs & MORIA_EDGE_HYPER))
    {
      vertices_push(list, edge->B);
    }
    else
    {
      for(x = 0; x < edge->Blen; x++)
      {
        vertices_push(list, edge->Blist[x].v);
      }
    }
  }

  edges_destroy(&edges);
}

void buildplan_entity_target_edges(const buildplan_entity * restrict bpe, edges * restrict list)
{
  moria_edge *edge;
  moria_foreach_state st;

  moria_foreach_edge_up(&st, &bpe->vertex, edge) {
    if(edge_type_get(edge) != EDGE_BPE_TGT) {
      continue;
    }
    edges_push(list, edge);
  }
}

void buildplan_entity_targets(const buildplan_entity * restrict bpe, vertices * restrict list)
{
  edges edges, *eln;
  moria_edge *edge;
  int x;

  edges_init(&edges);
  buildplan_entity_target_edges(bpe, &edges);

  vertices_init(list);
  edges_foreach(&edges, edge, eln) {
    if(!(edge->attrs & MORIA_EDGE_HYPER))
    {
      vertices_push(list, edge->A);
    }
    else
    {
      for(x = 0; x < edge->Alen; x++)
      {
        vertices_push(list, edge->Alist[x].v);
      }
    }
  }

  edges_destroy(&edges);
}

void buildplan_entity_say_sources(const buildplan_entity * restrict bpe, narrator * restrict N)
{
  fsent *n;
  int x;
  vertices list, *eln;
  moria_vertex *v;

  buildplan_entity_sources(bpe, &list);
  x = 0;
  vertices_foreach(&list, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    if(x++) {
      narrator_says(N, ", ");
    }
    fsent_path_say(n, N);
  }

  vertices_destroy(&list);
}

void buildplan_entity_say_targets(const buildplan_entity * restrict bpe, narrator * restrict N)
{
  fsent *n;
  int x;
  vertices list, *eln;
  moria_vertex *v;

  buildplan_entity_targets(bpe, &list);
  x = 0;
  vertices_foreach(&list, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    if(x++) {
      narrator_says(N, ", ");
    }
    fsent_path_say(n, N);
  }

  vertices_destroy(&list);
}

size_t buildplan_entity_targets_cat(void * restrict dst, size_t sz, const buildplan_entity * restrict bpe)
{
  fsent *n;
  int x;
  vertices list, *eln;
  size_t z;
  moria_vertex *v;

  buildplan_entity_targets(bpe, &list);
  x = 0;
  z = 0;
  vertices_foreach(&list, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    if(x++) {
      z += zcats(dst + z, sz - z, ", ");
    }
    z += fsent_path_cat(dst + z, sz - z, n);
  }

  vertices_destroy(&list);

  return z;
}

void bpe_release(buildplan_entity * restrict bpe)
{
  hashtable_delete(&bpe_cache, bpe);

  /* should be fully excised from the graph previously */
  RUNTIME_ASSERT(bpe->vertex.up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&bpe->vertex.up));
  RUNTIME_ASSERT(rbtree_empty(&bpe->vertex.down));

  buildplan_entity_dispose(bpe);
}


/* check whether this fsent is in the targets set of more than one buildable bpe.
 * if so, raises a system error.
 * a buildable bpe is one which has an associated formula.
 *
 * returns the unique bpe if there is one
 */
static buildplan_entity *bpe_target_check(fsent * restrict fse)
{
  events_context ctx;
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  moria_edge *edge;
  moria_foreach_state st;
  buildplan_entity *A;
  buildplan_entity *B;
  buildplan_entity *bpe;
  int x;
  moria_vertex *v;

  v = &fse->vertex;

  A = 0;
  B = 0;
  moria_foreach_edge_down(&st, v, edge) {
    if(edge_type_get(edge) != EDGE_BPE_TGT) {
      continue;
    }

    x = 0;
    bpe = 0;
    while(!A || !B)
    {
      if((edge->attrs & MORIA_EDGE_HYPER)) {
        if(x == edge->Blen) {
          break;
        } else {
          bpe = containerof(edge->Blist[x++].v, typeof(*bpe), vertex);
        }
      } else if(bpe) {
        break;
      } else {
        bpe = containerof(edge->B, typeof(*bpe), vertex);
      }

      if(bpe->fml == 0) {
        continue;
      } else if(A == 0) {
        A = bpe;
      } else {
        B = bpe;
      }
    }
  }

  if(A == 0 || B == 0) {
    return A; // no conflict
  }

  msg = system_error_event(&ctx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);

  builder_writer = bacon_writer_open(&writer, &fixed.base);
  { MAPPING
    STRING("file is buildable through two dependency paths")
    { LIST
      {
        vertex_describe(v, builder_writer);
      }
      {
        vertex_describe(&A->vertex, builder_writer);
      }
      {
        vertex_describe(&B->vertex, builder_writer);
      }
    }
  }
  bacon_writer_close(builder_writer);

  msg->len = fixed.l;
  msg->code = -1;
  events_publish(&ctx);
  return 0;
}

void bpe_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  buildplan_entity *bpe;
  fsent *fse;

  while((fse = llist_shift(&bpe_reconcile_list, typeof(*fse), bpe_reconcile_lln))) {
    bpe = bpe_target_check(fse);
    if(system_has_error) {
      llist_push(&bpe_reconcile_list, &fse->bpe_reconcile_lln);
      return;
    }

    /* update the cached bpe */
    if(fse->bpe != bpe) {
      fse->bpe = bpe;
      vertex_invalidate(&fse->vertex, invalidation);
    }
  }
}

void bpe_stats_build(buildplan_entity * restrict bpe, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&bpe->bpe_stats.record, &descriptor_stats_bpe, builder_writer);
}

void bpe_describe(const buildplan_entity * restrict bpe, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&bpe->vertex, builder_writer);
    { SET
      vertex_mappings(&bpe->vertex, builder_writer);
      moria_foreach_edge_down(&st, &bpe->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_RMA_BPE:
            { MAPPING STRING("rma")
              vertex_path_describe(e->B, builder_writer);
            }
          default:
            break;
        }
      }
      { MAPPING
        STRING("stats")
        { SET
          bpe_stats_build((buildplan_entity *)bpe, false, builder_writer);
        }
      }

      vertex_relations_describe(&bpe->vertex, builder_writer);
    }
  }
}

void bpe_identity_describe(const buildplan_entity * restrict bpe, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(bpe->vertex.attrs, graph_vertex_attrs, builder_writer);
  { MAPPING
    STRING("id");
    STRING(bpe->id);
  }
}

size_t bpe_path_cat(void * restrict dst, size_t sz, const buildplan_entity * restrict bpe)
{
  return zcatf(dst, sz, "//bm/bpes/%.*s", bpe->id_len, bpe->id);
}

void bpe_link_record(buildplan_entity * restrict bpe, build_record * restrict build_rec, formula_record * restrict rec)
{
  graph_invalidation_context invalidation = { };
  graph_invalidation_begin(&invalidation);

  meta_connect(&rec->vertex, &bpe->vertex, EDGE_FML_REC_BPE, &invalidation);
  meta_connect(&rec->vertex, &build_rec->vertex, EDGE_FML_REC_BUILD_REC, &invalidation);

  graph_invalidation_end(&invalidation);
}

void bpe_unlink(buildplan_entity * restrict bpe, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  moria_foreach_edge(&st, &bpe->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_SRC_BPE:
      case EDGE_BPE_TGT:
      case EDGE_RMA_BPE:
      case EDGE_FML_BPE:
      case EDGE_FML_REC_BPE:
      case EDGE_SPECIAL:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

bool bpe_reap(struct buildplan_entity * restrict bpe, struct graph_disintegrate_context * restrict ctx)
{
  if(edges_check_down(&bpe->vertex, EDGE_SRC_BPE)) {
    return false;
  } else if(edges_check_up(&bpe->vertex, EDGE_BPE_TGT)) {
    return false;
  }

  /* if no longer referenced by any module, then this varset is eligible to be reaped and any
   * of its edges will be cleaned up */
  return true;
}

void bpes_show(void)
{
  struct buildplan_entity *bpe;
  int x = 0;

  printf("bpes, live : %zu free : %zu\n", llist_count(&bpe_list), llist_count(&bpe_freelist));
  llist_foreach(&bpe_list, bpe, vertex.owner) {
    printf("%3u %p", x++, bpe);
    printf("\n");
  }
}

void bpe_id(const buildplan_entity * restrict bpe, const char ** restrict id, uint16_t * restrict idl)
{
  *id = bpe->id;
  *idl = bpe->id_len;
}

void bpe_compute_id(const buildplan_entity * restrict bpe, identity_ctx * restrict id)
{
  vertices Blist;
  vertices Alist;

  buildplan_entity_sources(bpe, &Blist);
  buildplan_entity_targets(bpe, &Alist);

  bpe_mk_id_lists(id, bpe->rma, bpe->fml, &Blist, &Alist);

  vertices_destroy(&Blist);
  vertices_destroy(&Alist);
}
