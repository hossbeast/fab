/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "moria/edge.h"
#include "moria/vertex.h"
#include "moria/foreach.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "valyria/hashtable.h"
#include "common/hash.h"

#include "buildplan_entity.h"
#include "dependency.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "formula.h"
#include "rule_module_association.h"
#include "entlist.h"
#include "rule.h"
#include "channel.h"
#include "system.h"
#include "events.h"
#include "shadow.h"

#include "zcat.h"

llist bpe_list = LLIST_INITIALIZER(bpe_list);                 // active
static llist bpe_freelist = LLIST_INITIALIZER(bpe_freelist);  // free
static hashtable bpe_cache;
static llist bpe_reconcile_list = LLIST_INITIALIZER(bpe_reconcile_list);    // struct fsent

void buildplan_entity_alloc(buildplan_entity ** restrict rv)
{
  buildplan_entity *bpe;

  if((bpe = llist_shift(&bpe_freelist, typeof(*bpe), vertex.owner)) == 0) {
    xmalloc(&bpe, sizeof(*bpe));
  } else {
    memset(bpe, 0, sizeof(*bpe));
  }

  moria_vertex_init(&bpe->vertex, &g_graph, VERTEX_BPE | VERTEX_OK);

  llist_push(&bpe_list, &bpe->vertex.owner);
  *rv = bpe;
}

static void buildplan_entity_dispose(buildplan_entity * restrict bpe)
{
  llist_delete(&bpe->vertex.owner);
  llist_push(&bpe_freelist, &bpe->vertex.owner);
}

static uint64_t bpe_hash(uint64_t h, const void * val)
{
  const buildplan_entity *bpe = val;
  entlist list, *eln;
  const fsent *fse;

  h = hash64(h, &bpe->rma, sizeof(bpe->rma));
  h = hash64(h, &bpe->fml, sizeof(bpe->fml));

  buildplan_entity_sources(bpe, &list);
  entlist_foreach(&list, fse, eln) {
    h = hash64(h, &fse, sizeof(fse));
  }
  entlist_destroy(&list);

  h += 0xface;
  buildplan_entity_targets(bpe, &list);
  entlist_foreach(&list, fse, eln) {
    h = hash64(h, &fse, sizeof(fse));
  }
  entlist_destroy(&list);

  return h;
}

static uint32_t bpe_hash_32(uint32_t h, const void * val)
{
  return bpe_hash(h, val);
}

static uint64_t search_key_hash(uint64_t h, const void * val)
{
  const struct buildplan_entity_search_context *ctx = val;
  int x;

  h = hash64(h, &ctx->rma, sizeof(ctx->rma));
  h = hash64(h, &ctx->fml, sizeof(ctx->fml));

  for(x = 0; x < ctx->Alen; x++) {
    h = hash64(h, &ctx->Alist[x], sizeof(ctx->Alist[x]));
  }

  h += 0xface;
  for(x = 0; x < ctx->Blen; x++) {
    h = hash64(h, &ctx->Blist[x], sizeof(ctx->Blist[x]));
  }

  return h;
}

static uint32_t search_key_hash_32(uint32_t h, const void * val)
{
  return search_key_hash(h, val);
}

static int search_key_cmp(const void *A, const void *K)
{
  const buildplan_entity *bpe = A;
  const struct buildplan_entity_search_context *ctx = K;
  entlist list, *eln;
  const void *bp;
  int r;
  int x;

  /* rma */
  if((r = INTCMP(bpe->rma, ctx->rma))) {
    return r;
  }

  /* fml */
  if((r = INTCMP(bpe->fml, ctx->fml))) {
    return r;
  }

  /* sources */
  buildplan_entity_sources(A, &list);
  eln = entlist_first(&list);
  x = 0;
  while(eln && x < ctx->Blen)
  {
    bp = ctx->Blist[x];
    if((r = INTCMP(eln->p, bp))) {
      break;
    }

    eln = entlist_next(&list, eln);
    x++;
  }

  entlist_destroy(&list);

  if(r) {
  } else if(eln) {
    r = 1;
  } else if(x < ctx->Blen) {
    r = -1;
  }

  if(r) {
    return r;
  }

  /* targets */
  buildplan_entity_targets(A, &list);
  eln = entlist_first(&list);
  x = 0;
  while(eln && x < ctx->Alen)
  {
    bp = ctx->Alist[x];
    if((r = INTCMP(eln->p, bp))) {
      break;
    }

    eln = entlist_next(&list, eln);
    x++;
  }

  entlist_destroy(&list);

  if(r) {
  } else if(eln) {
    r = 1;
  } else if(x < ctx->Alen) {
    r = -1;
  }

  return r;
}

static int bpe_cmp(const void * _A, const void *_B)
{
  const buildplan_entity *A = _A;
  const buildplan_entity *B = _B;
  entlist alist, *aln, blist, *bln;
  int r;

  /* rma */
  if((r = INTCMP(A->rma, B->rma))) {
    return r;
  }

  /* fml */
  if((r = INTCMP(A->fml, B->fml))) {
    return r;
  }

  /* sources */
  buildplan_entity_sources(A, &alist);
  aln = entlist_first(&alist);
  buildplan_entity_sources(B, &blist);
  bln = entlist_first(&blist);
  while(aln && bln)
  {
    if((r = INTCMP(aln->p, bln->p))) {
      break;
    }

    aln = entlist_next(&alist, aln);
    bln = entlist_next(&blist, bln);
  }

  entlist_destroy(&alist);
  entlist_destroy(&blist);

  if(r) {
  } else if(aln) {
    r = 1;
  } else if(bln) {
    r = -1;
  }

  if(r) {
    return r;
  }

  /* targets */
  buildplan_entity_targets(A, &alist);
  aln = entlist_first(&alist);
  buildplan_entity_targets(B, &blist);
  bln = entlist_first(&blist);
  while(aln && bln)
  {
    if((r = INTCMP(aln->p, bln->p))) {
      break;
    }

    aln = entlist_next(&alist, aln);
    bln = entlist_next(&blist, bln);
  }

  if(r) {
  } else if(aln) {
    r = 1;
  } else if(bln) {
    r = -1;
  }

  entlist_destroy(&alist);
  entlist_destroy(&blist);

  return r;
}

//
// public
//

void buildplan_entity_setup()
{
  hashtable_initx(&bpe_cache, 0, bpe_hash_32, bpe_cmp, 0);
}

void buildplan_entity_cleanup()
{
  llist *T;
  buildplan_entity *bpe;

  llist_foreach_safe(&bpe_list, bpe, vertex.owner, T) {
    buildplan_entity_dispose(bpe);
  }

  llist_foreach_safe(&bpe_freelist, bpe, vertex.owner, T) {
    free(bpe);
  }

  hashtable_destroy(&bpe_cache);
}

buildplan_entity *buildplan_entity_cache_search(struct buildplan_entity_search_context * restrict ctx)
{
  if(ctx->Alist) {
    qsort(ctx->Alist, ctx->Alen, sizeof(*ctx->Alist), ptrcmp);
  }
  if(ctx->Blist) {
    qsort(ctx->Blist, ctx->Blen, sizeof(*ctx->Blist), ptrcmp);
  }

  ctx->htx.entry = ctx;
  ctx->htx.hash_fn = search_key_hash_32;
  ctx->htx.cmp_fn = search_key_cmp;
  if(hashtable_search(&bpe_cache, &ctx->htx)) {
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
  entlist list, *eln;
  fsent *fse;

  if(!bpe->fml) {
    return;
  }

  entlist_init(&list);
  buildplan_entity_targets(bpe, &list);

  entlist_foreach(&list, fse, eln) {
    llist_delete(&fse->bpe_reconcile_lln);
    llist_push(&bpe_reconcile_list, &fse->bpe_reconcile_lln);
  }

  entlist_destroy(&list);
}

buildplan_entity *buildplan_entity_create_hyper(
    moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , formula * restrict fml
  , rule_module_association * restrict rma
)
{
  buildplan_entity *bpe;

  buildplan_entity_alloc(&bpe);
  dependency_hyperconnect(Blist, Blen, (moria_vertex *[]) { &bpe->vertex }, 1, EDGE_SRC_BPE);
  dependency_hyperconnect((moria_vertex *[]) { &bpe->vertex }, 1, Alist, Alen, EDGE_BPE_TGT);

  /* cached */
  bpe->rma = rma;
  bpe->fml = fml;

  bpe->hash = bpe_hash(0, bpe);
  bpe->id_len = zcatf(bpe->id, sizeof(bpe->id), "%016"PRIx64, bpe->hash);

  shadow_link(g_shadow_bpes, bpe->id, bpe->id_len, &bpe->vertex, 0);

  return bpe;
}

void buildplan_entity_source_edges(const buildplan_entity * restrict bpe, entlist * restrict list)
{
  moria_edge *edge;
  moria_foreach_state st;

  moria_foreach_edge_down(&st, &bpe->vertex, edge) {
    if(edge_type_get(edge) != EDGE_SRC_BPE) {
      continue;
    }
    entlist_push(list, edge);
  }
}

void buildplan_entity_sources(const buildplan_entity * restrict bpe, entlist * restrict list)
{
  entlist edges, *eln;
  moria_edge *edge;
  int x;

  entlist_init(&edges);
  buildplan_entity_source_edges(bpe, &edges);

  entlist_init(list);
  entlist_foreach(&edges, edge, eln) {
    if(!(edge->attrs & MORIA_EDGE_HYPER))
    {
      entlist_push(list, containerof(edge->B, fsent, vertex));
    }
    else
    {
      for(x = 0; x < edge->Blen; x++)
      {
        entlist_push(list, containerof(edge->Blist[x].v, fsent, vertex));
      }
    }
  }

  entlist_destroy(&edges);
}

void buildplan_entity_target_edges(const buildplan_entity * restrict bpe, entlist * restrict list)
{
  moria_edge *edge;
  moria_foreach_state st;

  moria_foreach_edge_up(&st, &bpe->vertex, edge) {
    if(edge_type_get(edge) != EDGE_BPE_TGT) {
      continue;
    }
    entlist_push(list, edge);
  }
}

void buildplan_entity_targets(const buildplan_entity * restrict bpe, entlist * restrict list)
{
  entlist edges, *eln;
  moria_edge *edge;
  int x;

  entlist_init(&edges);
  buildplan_entity_target_edges(bpe, &edges);

  entlist_init(list);
  entlist_foreach(&edges, edge, eln) {
    if(!(edge->attrs & MORIA_EDGE_HYPER))
    {
      entlist_push(list, containerof(edge->A, fsent, vertex));
    }
    else
    {
      for(x = 0; x < edge->Alen; x++)
      {
        entlist_push(list, containerof(edge->Alist[x].v, fsent, vertex));
      }
    }
  }

  entlist_destroy(&edges);
}

void buildplan_entity_say_sources(const buildplan_entity * restrict bpe, narrator * restrict N)
{
  fsent *n;
  int x;
  entlist list;
  entlist *eln;

  buildplan_entity_sources(bpe, &list);
  x = 0;
  entlist_foreach(&list, n, eln) {
    if(x++) {
      narrator_says(N, ", ");
    }
    fsent_path_say(n, N);
  }

  entlist_destroy(&list);
}

void buildplan_entity_say_targets(const buildplan_entity * restrict bpe, narrator * restrict N)
{
  fsent *n;
  int x;
  entlist list;
  entlist *eln;

  buildplan_entity_targets(bpe, &list);
  x = 0;
  entlist_foreach(&list, n, eln) {
    if(x++) {
      narrator_says(N, ", ");
    }
    fsent_path_say(n, N);
  }

  entlist_destroy(&list);
}

size_t buildplan_entity_targets_cat(void * restrict dst, size_t sz, const buildplan_entity * restrict bpe)
{
  fsent *n;
  int x;
  entlist list;
  entlist *eln;
  size_t z;

  buildplan_entity_targets(bpe, &list);
  x = 0;
  z = 0;
  entlist_foreach(&list, n, eln) {
    if(x++) {
      z += zcats(dst + z, sz - z, ", ");
    }
    z += fsent_path_cat(dst + z, sz - z, n);
  }

  entlist_destroy(&list);

  return z;
}

static void bpe_excise(buildplan_entity * restrict bpe, graph_invalidation_context * restrict invalidation)
{
  moria_foreach_state st;
  moria_edge *edge;
  entlist edges, vertices;

  /* collect edges for orphaned formulas */
  entlist_init(&edges);
  moria_foreach_edge(&st, &bpe->vertex, edge) {
    entlist_push(&edges, edge);
  }

  /* shadow refs */
  if((edge = moria_edge_between(&bpe->vertex, &g_shadow_bpes->vertex))) {
    entlist_push(&edges, edge);
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);
}

void bpe_release(buildplan_entity * restrict bpe, graph_invalidation_context * restrict invalidation)
{
  hashtable_delete(&bpe_cache, bpe);
  bpe_excise(bpe, invalidation);

  /* should be fully excised from the graph previously */
  RUNTIME_ASSERT(bpe->vertex.up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&bpe->vertex.up));
  RUNTIME_ASSERT(rbtree_empty(&bpe->vertex.down));

  buildplan_entity_dispose(bpe);
}

static buildplan_entity *bpe_target_check(fsent * restrict fse)
{
  events_context ctx;
  event_bacon *pb;
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

  /*
   * this fsent is the target of two or more buildable bpes
   */

  pb = system_error_event(&ctx);
  narrator_fixed_init(&fixed, pb->text, sizeof(pb->text));
  bacon_writer_init(&writer);

  builder_writer = bacon_writer_open(&writer, &fixed.base);
  { MAPPING
    STRING("file is buildable through two dependency paths")
    { LIST
      { MAPPING
        STRING("file")
        vertex_describe(v, builder_writer);
      }
      { SET
        vertex_describe(&A->vertex, builder_writer);
      }
      { SET
        vertex_describe(&B->vertex, builder_writer);
      }
    }
  }
  bacon_writer_close(builder_writer);

  pb->size = fixed.l;
  pb->code = -1;
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
  entlist list, *eln;
  const fsent *fse;

  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&bpe->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      { MAPPING STRING("attrs")
        attrs32_describe(bpe->vertex.attrs, graph_vertex_attrs, builder_writer);
      }
      { MAPPING STRING("id")
        BYTES(bpe->id, bpe->id_len)
      }
      moria_foreach_edge_down(&st, &bpe->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_RMA_BPE:
            { MAPPING STRING("rma")
              vertex_path_describe(e->B, builder_writer);
            }
          default:
        }
      }
      { MAPPING STRING("sources")
        { LIST
          buildplan_entity_sources(bpe, &list);
          entlist_foreach(&list, fse, eln) {
            vertex_identity_describe(&fse->vertex, builder_writer);
          }
          entlist_destroy(&list);
        }
      }
      { MAPPING STRING("targets")
        { LIST
          buildplan_entity_targets(bpe, &list);
          entlist_foreach(&list, fse, eln) {
            vertex_identity_describe(&fse->vertex, builder_writer);
          }
          entlist_destroy(&list);
        }
      }
      { MAPPING
        STRING("stats")
        { SET
          bpe_stats_build((buildplan_entity *)bpe, false, builder_writer);
        }
      }
    }
  }
}

void bpe_identity_say(const buildplan_entity * restrict bpe, struct narrator * restrict N)
{
  attrs32_say(bpe->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %s", bpe->id);
}

size_t bpe_path_cat(void * restrict dst, size_t sz, const buildplan_entity * restrict bpe)
{
  return zcatf(dst, sz, "//bpes/%.*s", bpe->id_len, bpe->id);
}
