/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "common/assure.h"
#include "common/attrs.h"
#include "moria/connect.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"

#include "rule.h"
#include "rma/events.h"
#include "dependency.h"
#include "formula.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "generate.h"
#include "search.internal.h"
#include "module.h"
#include "rma.h"
#include "rule_system.h"
#include "dir_rma_edge.h"
#include "stats.h"
#include "probes.h"
#include "channel.h"
#include "events/events.h"
#include "system.h"
#include "selection.h"
#include "ruleset.h"
#include "reconcile.h"
#include "bpe.h"
#include "meta_edge.h"
#include "edges.h"
#include "vertices.h"
#include "shadow.h"
#include "fsedge.h"
#include "fsname.h"
#include "namespace.h"
#include "identity.h"
#include "disintegrate.h"
#include "request_handler.h"

#include "macros.h"
#include "vertex/probes.h"

#include "pattern/section.h"
#include "zcat.h"
#include "marshal.h"

llist rule_list = LLIST_INITIALIZER(rule_list);                 // active rule
static llist rule_freelist = LLIST_INITIALIZER(rule_freelist);  // freelist

attrs16 * rule_direction_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, y) + 1
    num : 0 RULE_DIRECTION_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT16_C(y), range : RULE_DIRECTION_OPT },
RULE_DIRECTION_TABLE
  }
}};

attrs16 * rule_cardinality_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, y) + 1
    num : 0 RULE_CARDINALITY_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT16_C(y), range : RULE_CARDINALITY_OPT },
RULE_CARDINALITY_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs16_init(rule_direction_attrs);
  attrs16_init(rule_cardinality_attrs);
}

static buildplan_entity *bpe_create(rule_run_context * restrict ctx, struct buildplan_entity_search_context * restrict sctx)
{
  buildplan_entity *bpe;
  meta_edge *me;

  bpe = buildplan_entity_create_hyper(sctx->Blist, sctx->Blen, sctx->Alist, sctx->Alen, ctx->fml, ctx->rma, &ctx->invalidation);

  /* rma -> bpe meta-edge */
  me = meta_connect(&ctx->rma->vertex, &bpe->vertex, EDGE_RMA_BPE, &ctx->invalidation);
  me->epoch = rule_system_epoch;

  /* connect */
  rule_connect_probe(ctx->rma->rule, bpe);
  if(ctx->fml) {
    me = meta_connect(&ctx->fml->vertex, &bpe->vertex, EDGE_FML_BPE, &ctx->invalidation);
    me->epoch = rule_system_epoch;
  }

  /* an fsent with an incoming EDGE_BPE_TGT can become invalidated */
  rule_run_probe(ctx, bpe, true);

  buildplan_entity_cache_put(sctx, bpe);
  buildplan_entity_validate_enqueue(bpe);

  return bpe;
}

static void search_rhs_hit(void *ctxp, const pattern_search_hit *hitp)
{
  rule_run_context *ctx = ctxp;
  pattern_search_hit *hit;

  xmalloc(&hit, sizeof(*hit));
  memcpy(hit, hitp, sizeof(*hit));
  hashtable_put(&ctx->search_rhs_hits, hit);

  rma_event_search(ctx, hit);
}

static void search_hit(void *ctxp, const pattern_search_hit *hitp)
{
  rule_run_context *ctx = ctxp;
  pattern_search_hit *hit;

  xmalloc(&hit, sizeof(*hit));
  memcpy(hit, hitp, sizeof(*hit));
  hashtable_put(&ctx->search_hits, hit);

  rma_event_search(ctx, hit);
}

static void run_search_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  hashtable_recycle(&ctx->search_hits);

  pattern_search(
      rule->search
    , ctx->rma->mod
    , search_hit
    , ctx
    , dir_rma_connect_visit
    , ctx->rma
  );

  ctx->rma->rma_stats.search_hits += ctx->search_hits.size;
}

static void generate_hit(void *ctxp, const struct moria_vertex *hit)
{
  rule_run_context *ctx = ctxp;

  vertices_push(&ctx->generate_hits, hit);

  rma_event_generate(ctx, hit);
}

static void run_generate_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  vertices_destroy(&ctx->generate_hits);
  pattern_generate(
      rule->generate
    , ctx->rma->mod
    , 0
    , 0
    , ctx->rma->mod->build_dir
    , &ctx->invalidation
    , 0
    , PATTERN_GENERATE_NOFOLLOW
    , generate_hit
    , ctx
  );

  ctx->rma->rma_stats.generate_hits += llist_count(&ctx->generate_hits.lln);
}

static void run_generate(rule_run_context * restrict ctx, pattern_search_hit * restrict match)
{
  pattern_generate(
      ctx->rma->rule->generate
    , ctx->rma->mod
    , 0
    , 0
    , ctx->rma->mod->build_dir
    , &ctx->invalidation
    , match
    , PATTERN_GENERATE_NOFOLLOW
    , generate_hit
    , ctx
  );

  ctx->rma->rma_stats.generate_hits += llist_count(&ctx->generate_hits.lln);
}

static int pattern_search_compare(const void *a, const void *b)
{
  const struct pattern_search_hit *A = *(typeof(A)*)a;
  const struct pattern_search_hit *B = *(typeof(B)*)b;

  int r;
  int x;

  if((r = INTCMP(A->group_max, B->group_max))) {
    return r;
  }

  for(x = 1; x <= A->group_max; x++) {
    if((r = memncmp(A->groups[x].start, A->groups[x].len, B->groups[x].start, B->groups[x].len))) {
      return r;
    }
  }

  return 0;
}

/* sort the matches into a set of groups, where each match in a group has the same variant and capture groups */
static void matches_sorted(rule_run_context * restrict ctx)
{
  int y;
  size_t Msz;
  pattern_search_hit *match;

  assure(&ctx->Mlist, sizeof(*ctx->Mlist), ctx->search_hits.size, &ctx->Mlist_alloc);

  Msz = 0;
  for(y = 0; y < ctx->search_hits.table_size; y++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, y))) {
      continue;
    }

    ctx->Mlist[Msz++] = match;
  }

  /* sorted by sets of matching capture groups */
  qsort(ctx->Mlist, Msz, sizeof(*ctx->Mlist), pattern_search_compare);
}

/* by being matched as the target of a rule, an fsent is promoted from UNLINKED to UNCREATED
 * UNCREATED nodes can be matched by search rules, UNLINKED nodes cannot */
static void vivify_targets(moria_vertex ** restrict A, uint16_t Alen, graph_invalidation_context * restrict invalidation)
{
  int x;
  fsent *fse;

  for(x = 0; x < Alen; x++)
  {
    fse = containerof(A[x], typeof(*fse), vertex);
    if(vertex_state_get(&fse->vertex) != VERTEX_UNLINKED) {
      continue;
    }

    vertex_state_set(&fse->vertex, VERTEX_UNCREATED);
    vertex_invalidate(&fsent_parent(fse)->vertex, invalidation);
  }
}

static buildplan_entity *bpe_search(rule_run_context * restrict ctx, struct buildplan_entity_search_context * restrict sctx)
{
  buildplan_entity *bpe;

  if(!(bpe = buildplan_entity_cache_search(sctx))) {
    return 0;
  }

  bpe->rma_bpe_me->epoch = rule_system_epoch;
  if(bpe->rma->fml_rma_me) {
    bpe->rma->fml_rma_me->epoch = rule_system_epoch;
  }

  rule_run_probe(ctx, bpe, false);

  return bpe;
}

/*
 * connector : search --
 */
static void __attribute__((nonnull)) search_zero_to_one(
    rule * restrict rule
  , rule_run_context * restrict ctx
)
{
  pattern_search_hit *match;
  buildplan_entity *bpe;
  moria_vertex *v;
  struct buildplan_entity_search_context sctx;
  int x;

  /* run the match pattern */
  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    v = &match->node->vertex;
    sctx.Blist = 0;
    sctx.Blen = 0;
    sctx.Alist = &v;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe = bpe_create(ctx, &sctx);
  }
  rma_operation_pop(ctx);
}

/*
 * connector : -- generate
 */
static void __attribute__((nonnull)) generate_zero_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  buildplan_entity *bpe;
  moria_vertex *v;
  fsent *n;
  struct buildplan_entity_search_context sctx;
  vertices *eln;

  rma_operation_push(ctx);
  run_generate_tracking(rule, ctx);
  vertices_foreach(&ctx->generate_hits, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    sctx.Blist = 0;
    sctx.Blen = 0;
    sctx.Alist = &v;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe = bpe_create(ctx, &sctx);
  }
  rma_operation_pop(ctx);
}

/*
 * connector : search-pattern [*] --
 */
static void __attribute__((nonnull)) search_zero_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_hit * match = 0;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  size_t z;
  int x;

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);

  z = 0;
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->search_hits.size, &ctx->Alist_alloc);

  for(x = 0; x < ctx->search_hits.table_size; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Alist[z++] = &match->node->vertex;
  }
  if(z == 0) {
    goto end;
  }

  sctx.Blist = 0;
  sctx.Blen = 0;
  sctx.Alist = ctx->Alist;
  sctx.Alen = z;

  vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
  sctx.rma = ctx->rma;
  sctx.fml = ctx->fml;
  if((bpe = bpe_search(ctx, &sctx))) {
    goto end;
  }
  bpe = bpe_create(ctx, &sctx);

end:
  rma_operation_pop(ctx);
}

/*
 * connector : -- [*] generate
 */
static void __attribute__((nonnull)) generate_zero_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  moria_vertex *v;
  size_t z;
  vertices *eln;

  rma_operation_push(ctx);
  run_generate_tracking(rule, ctx);

  z = 0;
  assure(&ctx->Alist, sizeof(*ctx->Alist), llist_count(&ctx->generate_hits.lln), &ctx->Alist_alloc);

  // unzip
  vertices_foreach(&ctx->generate_hits, v, eln) {
    ctx->Alist[z++] = v;
  }
  if(z == 0) {
    goto end;
  }

  sctx.Blist = 0;
  sctx.Blen = 0;
  sctx.Alist = ctx->Alist;
  sctx.Alen = z;

  vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
  sctx.rma = ctx->rma;
  sctx.fml = ctx->fml;
  if((bpe = bpe_search(ctx, &sctx))) {
    goto end;
  }
  bpe = bpe_create(ctx, &sctx);

end:
  rma_operation_pop(ctx);
}

/*
 * connector : search -> generate
 */
static void __attribute__((nonnull)) search_one_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_hit * match = 0;
  buildplan_entity *bpe;
  int x;
  moria_vertex *generated;
  struct buildplan_entity_search_context sctx;
  vertices *eln;

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  if(ctx->search_hits.size == 0 && !system_has_error) {
    rule_run_probe(ctx, 0, true);
  }

  for(x = 0; x < ctx->search_hits.table_size && !system_has_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    vertices_foreach(&ctx->generate_hits, generated, eln) {
      ctx->Blist[0] = &match->node->vertex; // source, down
      ctx->Alist[0] = generated;            // target, up

      sctx = (typeof(sctx)) {
          Blist : ctx->Blist
        , Blen : 1
        , Alist : ctx->Alist
        , Alen : 1
      };

      vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
      sctx.rma = ctx->rma;
      sctx.fml = ctx->fml;
      if((bpe = bpe_search(ctx, &sctx))) {
        continue;
      }
      bpe = bpe_create(ctx, &sctx);
    }
  }

  rma_operation_pop(ctx);
}

/*
 * connector : search <- generate
 */
static void __attribute__((nonnull)) generate_one_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_hit * match = 0;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  moria_vertex *generated;
  vertices *eln;
  int x;

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size && !system_has_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    vertices_foreach(&ctx->generate_hits, generated, eln) {
      ctx->Blist[0] = generated;            // source, down
      ctx->Alist[0] = &match->node->vertex; // target, up

      sctx.Blist = ctx->Blist;
      sctx.Blen = 1;
      sctx.Alist = ctx->Alist;
      sctx.Alen = 1;

      vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
      sctx.rma = ctx->rma;
      sctx.fml = ctx->fml;
      if((bpe = bpe_search(ctx, &sctx))) {
        continue;
      }
      bpe = bpe_create(ctx, &sctx);
    }
  }
  rma_operation_pop(ctx);
}

/*
 * connector : search -> [*] generate
 */
static void __attribute__((nonnull)) search_one_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_hit * match = 0;
  buildplan_entity *bpe;
  int x;
  size_t Asz;
  moria_vertex *v;
  struct buildplan_entity_search_context sctx;
  vertices *eln;

  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size && !system_has_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Blist[0] = &match->node->vertex;

    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    assure(&ctx->Alist, sizeof(*ctx->Alist), llist_count(&ctx->generate_hits.lln), &ctx->Alist_alloc);
    Asz = 0;
    vertices_foreach(&ctx->generate_hits, v, eln) {
      ctx->Alist[Asz++] = v;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = 1;
    sctx.Alist = ctx->Alist;
    sctx.Alen = Asz;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe = bpe_create(ctx, &sctx);
  }
  rma_operation_pop(ctx);
}

/*
 * connector : search [*] <- generate
 */
static void __attribute__((nonnull)) generate_one_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_hit * match = 0;
  buildplan_entity *bpe;
  size_t Bsz;
  moria_vertex *v;
  struct buildplan_entity_search_context sctx;
  int x;
  vertices *eln;

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size && !system_has_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Alist[0] = &match->node->vertex;

    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    assure(&ctx->Blist, sizeof(*ctx->Blist), llist_count(&ctx->generate_hits.lln), &ctx->Blist_alloc);
    Bsz = 0;
    vertices_foreach(&ctx->generate_hits, v, eln) {
      ctx->Blist[Bsz++] = v;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      return;
    }
    bpe = bpe_create(ctx, &sctx);
  }
  rma_operation_pop(ctx);
}

/*
 * search [*] -> generate
 */
static void __attribute__((nonnull)) search_many_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  size_t Bsz;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  int x, y;
  moria_vertex *v;
  vertices *eln;

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->search_hits.size, &ctx->Blist_alloc);

  x = 0;
  for(y = 1; y <= ctx->search_hits.size && !system_has_error; y++)
  {
    if(y < ctx->search_hits.size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Bsz = 0; Bsz < (y - x); Bsz++) {
      ctx->Blist[Bsz] = &ctx->Mlist[x + Bsz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, ctx->Mlist[x]);

    vertices_foreach(&ctx->generate_hits, v, eln) {
      ctx->Alist[0] = v;

      sctx.Blist = ctx->Blist;
      sctx.Blen = Bsz;
      sctx.Alist = ctx->Alist;
      sctx.Alen = 1;

      vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
      /* check for an existing bpe */
      sctx.rma = ctx->rma;
      sctx.fml = ctx->fml;
      if((bpe = bpe_search(ctx, &sctx))) {
        goto next;
      }
      bpe = bpe_create(ctx, &sctx);
    }

  next:
    x = y;
  }
  rma_operation_pop(ctx);
}

/*
 * connector : search <- [*] generate
 */
static void __attribute__((nonnull)) generate_many_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_hit * match = 0;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  size_t Bsz;
  moria_vertex *v;
  vertices *eln;
  int x;

  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);

  for(x = 0; x < ctx->search_hits.table_size && !system_has_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Alist[0] = &match->node->vertex;
    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    Bsz = 0;
    assure(&ctx->Blist, sizeof(*ctx->Blist), llist_count(&ctx->generate_hits.lln), &ctx->Blist_alloc);
    vertices_foreach(&ctx->generate_hits, v, eln) {
      ctx->Blist[Bsz++] = v;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe = bpe_create(ctx, &sctx);
  }
  rma_operation_pop(ctx);
}

/*
 * connector : search [*] -> [*] generate
 */
static void __attribute__((nonnull)) search_many_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  size_t Asz;
  size_t Bsz;
  buildplan_entity *bpe;
  int x;
  int y;
  moria_vertex *v;
  struct buildplan_entity_search_context sctx;
  vertices *eln;

  /* run the match pattern */
  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->search_hits.size, &ctx->Blist_alloc);

  x = 0;
  for(y = 1; y <= ctx->search_hits.size && !system_has_error; y++)
  {
    if(y < ctx->search_hits.size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Bsz = 0; Bsz < (y - x); Bsz++) {
      ctx->Blist[Bsz] = &ctx->Mlist[x + Bsz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, ctx->Mlist[x]);

    assure(&ctx->Alist, sizeof(*ctx->Alist), llist_count(&ctx->generate_hits.lln), &ctx->Alist_alloc);
    Asz = 0;
    vertices_foreach(&ctx->generate_hits, v, eln) {
      ctx->Alist[Asz++] = v;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = Asz;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      goto next;
    }
    bpe = bpe_create(ctx, &sctx);

  next:
    x = y;
  }
  rma_operation_pop(ctx);
}

/*
 * connector : search [*] <- [*] generate
 */
static void __attribute__((nonnull)) generate_many_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  size_t Asz;
  size_t Bsz;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  int x;
  int y;
  vertices *eln;
  moria_vertex *v;

  /* run the match pattern */
  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->search_hits.size, &ctx->Alist_alloc);

  x = 0;
  for(y = 1; y <= ctx->search_hits.size && !system_has_error; y++)
  {
    if(y < ctx->search_hits.size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    vertices_destroy(&ctx->generate_hits);
    run_generate(ctx, ctx->Mlist[x]);

    assure(&ctx->Blist, sizeof(*ctx->Blist), llist_count(&ctx->generate_hits.lln), &ctx->Blist_alloc);
    Bsz = 0;
    vertices_foreach(&ctx->generate_hits, v, eln) {
      ctx->Blist[Bsz++] = v;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = Asz;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    sctx.fml = ctx->fml;
    if((bpe = bpe_search(ctx, &sctx))) {
      goto next;
    }
    bpe = bpe_create(ctx, &sctx);

  next:
    x = y;
  }
  rma_operation_pop(ctx);
}

//
// public
//

void rule_dispose(struct rule * restrict rule)
{
  /* not on any block list */
  //RUNTIME_ASSERT(!llist_attached(&rule->lln));

  pattern_free((void*)rule->search);
  pattern_free((void*)rule->generate);
  pattern_free((void*)rule->formula);
  pattern_free((void*)rule->target);
  pattern_free((void*)rule->attachment);
  pattern_free((void*)rule->search_rhs);

  llist_delete(&rule->vertex.owner);
  llist_append(&rule_freelist, &rule->vertex.owner);
  ASAN_TAINT(rule);
}

void rule_release(rule * restrict r)
{
  moria_vertex __attribute__((unused)) *v;

  /* must have been fully excised from the graph previously */
  v = &r->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  rule_dispose(r);
}

void rule_cleanup(void)
{
  rule *r;

  while((r = llist_shift(&rule_freelist, typeof(*r), vertex.owner))) {
    ASAN_CLEAN(r);
    free(r);
  }
}

int rule_validate(const rule * restrict rule, char * restrict error, size_t error_sz)
{
  /* only those invalid cases which absolutely cannot be detected by the parser
   * should be considered here */
  if(rule->card == RULE_MANY_TO_ONE) {
    if(rule->search && rule->search->pattern.capture_groups_count > 0) {
      return zcatf(
          error, error_sz, "search pattern in many-to-one rules cannot have capturing groups @ %s [%d,%d - %d,%d]"
        , rule->loc.fname
        , rule->loc.f_lin + 1
        , rule->loc.f_col + 1
        , rule->loc.l_lin + 1
        , rule->loc.l_col + 1
      );
    }
  }

  return 0;
}

rule *rule_mk(
    /*  1 */ moria_graph * restrict g
  , /*  2 */ const ruleset * restrict container
  , /*  3 */ search_pattern * restrict search
  , /*  4 */ generate_pattern * restrict generate
  , /*  5 */ lookup_pattern * restrict formula
  , /*  6 */ lookup_pattern * restrict target
  , /*  7 */ attachment_pattern * restrict attachment
  , /*  8 */ search_pattern * restrict search2
  , /*  9 */ const rule_bacon * restrict bacon
  , /* 10 */ uint32_t attrs
  , /* 11 */ const yyu_location * restrict loc
)
{
  rule *r;
  identity_ctx id;

  if((r = llist_shift(&rule_freelist, typeof(*r), vertex.owner))) {
    ASAN_CLEAN(r);
    memset(r, 0, sizeof(*r));
  } else {
    xmalloc(&r, sizeof(*r));
  }

  moria_vertex_init(&r->vertex, g, VERTEX_RULE | VERTEX_OK);
  llist_append(&rule_list, &r->vertex.owner);

  r->search = search;
  r->generate = generate;
  r->formula = formula;
  r->target = target;
  r->attachment = attachment;
  r->search_rhs = search2;
  r->dir = attrs & RULE_DIRECTION_OPT;
  r->card = attrs & RULE_CARDINALITY_OPT;
  if(loc) {
    memcpy(&r->loc, loc, sizeof(r->loc));
  } else {
    memset(&r->loc, 0, sizeof(r->loc));
  }

  identity_init(&id);
  r->container = container;
  rule_compute_id(r, &id);
  r->id_len = identity_mk(&id, r->id, sizeof(r->id));

  r->name_len = zcatw(r->name, sizeof(r->name), bacon->name, bacon->name_len);
  if(bacon->namespace_len) {
    r->namespace_len = zcatw(r->namespace, sizeof(r->namespace), bacon->namespace, bacon->namespace_len);
  } else {
    r->namespace_len = zcats(r->namespace, sizeof(r->namespace), "modules");
  }

  /* uniqueness by id is enforced */
  shadow_mkref_new(g_shadow_rules, r->id, r->id_len, &r->vertex);

  /* uniqueness of names is not enforced */
  if(r->name_len) {
    shadow_mkref(g_shadow_rules_byname, r->name, r->name_len, &r->vertex);
  }

  vertex_alloc_probe(&r->vertex);

  return r;
}

static fsent *resolve_attachment(rule * restrict bind, rule_run_context * restrict ctx)
{
  fsent *attachment;

  vertices_destroy(&ctx->generate_hits);

  pattern_attachment_generate(
      bind->attachment
    , ctx->rma->mod
    , 0
    , 0
    , ctx->rma->mod->build_dir
    , &ctx->invalidation
    , 0 /* match */
    , PATTERN_GENERATE_NOFOLLOW
    , generate_hit
    , ctx
  );

  RUNTIME_ASSERT(llist_count(&ctx->generate_hits.lln) <= 1);

  attachment = 0;
  if(llist_count(&ctx->generate_hits.lln) == 1) {
    attachment = containerof(vertices_first(&ctx->generate_hits)->p, typeof(*attachment), vertex);
  }

  return attachment;
}

static void bind_one_to_one(struct rule * restrict bind, fsent *attachment, rule_run_context * restrict ctx)
{
  fsent *target;
  meta_edge *me;

  RUNTIME_ASSERT(ctx->lctx.rctx.r == 1);
  target = containerof(ctx->lctx.rctx.vertices[0], typeof(*target), vertex);

  /* promote the attachment node to a link */
  if(vertex_filetype_get(&attachment->vertex) == VERTEX_FILETYPE_REG) {
    vertex_filetype_set(&attachment->vertex, VERTEX_FILETYPE_SYMLINK);
    attachment->vertex.attrs |= MORIA_VERTEX_LINK;
  }
  RUNTIME_ASSERT(vertex_type_get(&attachment->vertex) == VERTEX_SHADOW_LINK);

  attachment->vertex.ref = &target->vertex;

  me = meta_connect(&ctx->rma->vertex, &attachment->vertex, EDGE_RMA_BIND, &ctx->invalidation);
  me->epoch = rule_system_epoch;

  rma_event_bind(ctx, attachment, target, 0);
  bind_run_probe(ctx->rma, attachment, target, 0);
}

static void bind_one_to_many(struct rule * restrict bind, fsent *attachment, rule_run_context * restrict ctx)
{
  moria_vertex *v;
  fsent *target;
  fsent *binding;
  int x;
  pattern_search_hit *match;
  meta_edge *me;

  /* promote the attachment node to a directory */
  if(vertex_filetype_get(&attachment->vertex) == VERTEX_FILETYPE_REG) {
    vertex_filetype_set(&attachment->vertex, VERTEX_FILETYPE_DIR);
  }
  RUNTIME_ASSERT(vertex_type_get(&attachment->vertex) == VERTEX_SHADOW_DIR);

  if(bind->target) {
    v = ctx->lctx.rctx.vertices[0];
    target = containerof(v, typeof(*target), vertex);

    /* make a link in the attachment directory ; this needs to propagate from
     * the directory via EDGE_DEPENDS */
    shadow_graft_bind(ctx->rma->mod, target, attachment, &binding, &ctx->invalidation);

    me = meta_connect(&ctx->rma->vertex, &binding->vertex, EDGE_RMA_BIND, &ctx->invalidation);
    me->epoch = rule_system_epoch;

    rma_event_bind(ctx, attachment, target, binding);
    bind_run_probe(ctx->rma, attachment, target, binding);
  } else if(bind->search) {
    for(x = 0; x < ctx->search_hits.table_size; x++)
    {
      if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
        continue;
      }
      target = match->node;

      // link @ //module/uses/$name
      shadow_graft_bind(ctx->rma->mod, target, attachment, &binding, &ctx->invalidation);

      me = meta_connect(&ctx->rma->vertex, &binding->vertex, EDGE_RMA_BIND, &ctx->invalidation);
      me->epoch = rule_system_epoch;

      rma_event_bind(ctx, attachment, target, binding);
      bind_run_probe(ctx->rma, attachment, target, binding);
    }
    if(ctx->search_hits.size == 0) {
      rma_event_bind(ctx, attachment, 0, 0);
      bind_run_probe(ctx->rma, attachment, 0, 0);
    }
  }
}

static void search_search_run(struct rule * restrict rule, rule_run_context * restrict ctx)
{
  size_t Asz;
  size_t Bsz;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  int x;
  pattern_search_hit *match;

  /* LHS */
  rma_operation_push(ctx);
  run_search_tracking(rule, ctx);

  assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->search_hits.size, &ctx->Blist_alloc);
  Bsz = 0;
  for(x = 0; x < ctx->search_hits.table_size; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Blist[Bsz++] = &match->node->vertex;
  }

  /* RHS */
  hashtable_recycle(&ctx->search_rhs_hits);
  pattern_search(
      rule->search_rhs
    , ctx->rma->mod
    , search_rhs_hit
    , ctx
    , dir_rma_connect_visit
    , ctx->rma
  );
  ctx->rma->rma_stats.search_rhs_hits += ctx->search_rhs_hits.size;

  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->search_rhs_hits.size, &ctx->Alist_alloc);
  Asz = 0;
  for(x = 0; x < ctx->search_rhs_hits.table_size; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_rhs_hits, x))) {
      continue;
    }

    ctx->Alist[Asz++] = &match->node->vertex;
  }

  sctx.Blist = ctx->Blist;
  sctx.Blen = Bsz;
  sctx.Alist = ctx->Alist;
  sctx.Alen = Asz;

  sctx.rma = ctx->rma;
  sctx.fml = ctx->fml;
  if((bpe = bpe_search(ctx, &sctx))) {
    goto end;
  }
  bpe = bpe_create(ctx, &sctx);

end:
  rma_operation_pop(ctx);
}

static void namespace_lookup_error(struct rule * restrict bind)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;
  const yyu_location *loc;

  loc = &bind->loc;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("unresolved namespace ref");
    { SET
      { MAPPING
        STRING("location")
        LOADF("%s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
      }
      { MAPPING STRING("namespace")
        BYTES(bind->namespace, bind->namespace_len)
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void namespace_resolve_error(struct lookup_context * restrict ctx, int r)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    if(r == 0) {
      STRING("unresolved bind target")
    } else {
      STRING("ambiguous bind target")
    }
    lookup_namespace_error_build(builder_writer, ctx);
  }

  bacon_writer_close(&writer);
  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void bind_run(struct rule * restrict bind, rule_run_context * restrict ctx)
{
  meta_edge *me;
  fsent *attachment;
  namespace *fsi;
  int r;

  rma_operation_push(ctx);

  /* attachment */
  if((attachment = resolve_attachment(bind, ctx)) == 0) {
    ctx->rma->rma_stats.attachment_hits = 0;
    goto end;
  }
  RUNTIME_ASSERT(vertex_state_get(&attachment->vertex) != VERTEX_UNLINKED);
  RUNTIME_ASSERT(vertex_shadow_get(&attachment->vertex));
  ctx->rma->rma_stats.attachment_hits = 1;

  if(bind->target) {
    /* lookup targets pattern namespace */
    if((fsi = namespace_lookup(bind->namespace, bind->namespace_len)) == NULL) {
      namespace_lookup_error(bind);
      goto end;
    }

    /* resolve targets pattern */
    memset(&ctx->lctx, 0, sizeof(ctx->lctx));
    ctx->lctx.ns = fsi;
    ctx->lctx.ns_name = fsi->name;
    ctx->lctx.ns_name_len = fsi->name_len;
    ctx->lctx.loc = &bind->target->pattern.loc;
    ctx->lctx.rctx.vars = ctx->rma->mod->vars;
    if((r = namespace_resolve(&ctx->lctx, bind->target)) != 1) {
      namespace_resolve_error(&ctx->lctx, r);
    }
    ctx->rma->rma_stats.target_hits = r;
    rma_event_resolve(ctx);

    /* take an edge the fsname visited */
    if(ctx->lctx.hit) {
      me = meta_connect(&ctx->lctx.hit->vertex, &ctx->rma->vertex, EDGE_FSNAME_RMA, &ctx->invalidation);
      me->epoch = rule_system_epoch;
    } else if(ctx->lctx.miss) {
      me = meta_connect(&ctx->lctx.miss->vertex, &ctx->rma->vertex, EDGE_FSNAME_RMA, &ctx->invalidation);
      me->epoch = rule_system_epoch;
    }

    if(r != 1) {
      bind_run_probe(ctx->rma, attachment, 0, 0);
      goto end;
    }
  } else if(bind->search) {
    run_search_tracking(bind, ctx);
  }

  switch(bind->card) {
    case RULE_BIND_MANY:
      bind_one_to_many(bind, attachment, ctx);
      break;
    case RULE_BIND_ONE:
      bind_one_to_one(bind, attachment, ctx);
      break;
    default:
      RTA;
  }

end:
  rma_operation_pop(ctx);
}

void rule_resolve(rule * restrict rule, rule_run_context * restrict ctx)
{
  graph_invalidation_begin(&ctx->invalidation);

  switch(rule->card | rule->dir)
  {
    case RULE_BIND_ONE:
    case RULE_BIND_MANY:
      bind_run(rule, ctx);
      break;
    default:
      break;
  }

  ctx->rma->fml = 0;
  if(ctx->rma->rule->formula) {
    if((ctx->rma->fml = formula_resolve(ctx->rma->rule->formula, ctx))) {
      formula_reparse(ctx->rma->fml);
    }
  }

  if(system_has_error) {
    system_stats_events.rule_resolve_err++;
    ctx->rma->rma_stats.rule_resolve_err++;
    ctx->rma->rule->rule_stats.rule_resolve_err++;
  } else {
    system_stats_events.rule_resolve_ok++;
    ctx->rma->rma_stats.rule_resolve_ok++;
    ctx->rma->rule->rule_stats.rule_resolve_ok++;
  }

  graph_invalidation_end(&ctx->invalidation);
}

void rule_run(rule * restrict rule, rule_run_context * restrict ctx)
{
  system_stats_events.rule_run++;
  ctx->rma->rma_stats.rule_run++;
  ctx->rma->rule->rule_stats.rule_run++;

  graph_invalidation_begin(&ctx->invalidation);

  rma_operation_push(ctx);
  switch(rule->card | rule->dir)
  {
    case RULE_ZERO_TO_ONE | RULE_LTR:     /*            -- generate     */
      generate_zero_to_one(rule, ctx);
      break;
    case RULE_ZERO_TO_ONE | RULE_RTL:     /*     search --              */
      search_zero_to_one(rule, ctx);
      break;
    case RULE_ZERO_TO_MANY | RULE_LTR:    /*            -- [*] generate */
      generate_zero_to_many(rule, ctx);
      break;
    case RULE_ZERO_TO_MANY | RULE_RTL:    /* search [*] --              */
      search_zero_to_many(rule, ctx);
      break;
    case RULE_ONE_TO_ONE | RULE_LTR:      /*     search -> generate     */
      search_one_to_one(rule, ctx);
      break;
    case RULE_ONE_TO_ONE | RULE_RTL:      /*     search <- generate     */
      generate_one_to_one(rule, ctx);
      break;
    case RULE_ONE_TO_MANY | RULE_LTR:     /*     search -> [*] generate */
      search_one_to_many(rule, ctx);
      break;
    case RULE_ONE_TO_MANY | RULE_RTL:     /* search [*] <- generate     */
      generate_one_to_many(rule, ctx);
      break;
    case RULE_MANY_TO_ONE | RULE_LTR:     /* search [*] -> generate     */
      search_many_to_one(rule, ctx);
      break;
    case RULE_MANY_TO_ONE | RULE_RTL:     /*     search <- [*] generate */
      generate_many_to_one(rule, ctx);
      break;
    case RULE_MANY_TO_MANY | RULE_LTR:    /* search [*] -> [*] generate */
      search_many_to_many(rule, ctx);
      break;
    case RULE_MANY_TO_MANY | RULE_RTL:    /* search [*] <- [*] generate */
      generate_many_to_many(rule, ctx);
      break;
    case RULE_SEARCH_SEARCH | RULE_LTR:
      search_search_run(rule, ctx);
      break;
    case RULE_BIND_ONE:
    case RULE_BIND_MANY:
      break;
    default:
      RTA;
  }
  rma_operation_pop(ctx);

  graph_invalidation_end(&ctx->invalidation);
}

void rule_say(const rule * restrict r, narrator * restrict N)
{
  /* bind rules */
  if(r->card == RULE_BIND_ONE || r->card == RULE_BIND_MANY) {
    if(r->name_len) {
      xsayf("bind { name : %.*s } ", r->name_len, r->name);
    } else {
      xsays("bind ");
    }
    if(r->target) {
      pattern_say(&r->target->pattern, N);
    } else if(r->search) {
      pattern_say(&r->search->pattern, N);
    }
    switch(r->card) {
      case RULE_BIND_ONE:
        xsays(" => ");
        break;
      case RULE_BIND_MANY:
        xsays(" => [*] ");
        break;
      default:
        RTA;
    }
    pattern_say(&r->attachment->pattern, N);
    return;
  }

  if(r->name_len) {
    xsayf("rule { name : %.*s } ", r->name_len, r->name);
  }

  /* search/search rules */
  if(r->card == RULE_SEARCH_SEARCH) {
    xsays("search ");
  } else if(!r->name_len) {
    xsays("rule ");
  }

  if(r->search) {
    pattern_say(&r->search->pattern, N);
  }

  switch(CASE2(r->card, r->dir)) {
    case CASE2(RULE_ONE_TO_ONE, RULE_LTR):
      xsays(" -> ");
      break;
    case CASE2(RULE_ONE_TO_ONE, RULE_RTL):
      xsays(" <- ");
      break;
    case CASE2(RULE_MANY_TO_ONE, RULE_RTL):
      xsays(" <- [*] ");
      break;
    case CASE2(RULE_ONE_TO_MANY, RULE_LTR):
      xsays(" -> [*] ");
      break;
    case CASE2(RULE_ONE_TO_MANY, RULE_RTL):
      xsays(" [*] <- ");
      break;
    case CASE2(RULE_MANY_TO_ONE, RULE_LTR):
      xsays(" [*] -> ");
      break;
    case CASE2(RULE_ZERO_TO_ONE, RULE_LTR):
      xsays("-- ");
      break;
    case CASE2(RULE_ZERO_TO_ONE, RULE_RTL):
      xsays(" --");
      break;
    case CASE2(RULE_ZERO_TO_MANY, RULE_LTR):
      xsays("-- [*] ");
      break;
    case CASE2(RULE_ZERO_TO_MANY, RULE_RTL):
      xsays(" [*] --");
      break;
    case CASE2(RULE_MANY_TO_MANY, RULE_LTR):
    case CASE2(RULE_SEARCH_SEARCH, RULE_LTR):
      xsays(" [*] -> [*] ");
      break;
    case CASE2(RULE_MANY_TO_MANY, RULE_RTL):
    case CASE2(RULE_SEARCH_SEARCH, RULE_RTL):
      xsays("[*] <- [*]");
      break;
    default:
      RTA;
  }

  if(r->card == RULE_SEARCH_SEARCH) {
    xsays("search ");
    pattern_say(&r->search_rhs->pattern, N);
  } else if(r->generate) {
    pattern_say(&r->generate->pattern, N);
  }

  if(r->formula) {
    narrator_says(N, " : ");
    pattern_say(&r->formula->pattern, N);
  }
}

void rule_show(const rule * restrict r)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  rule_describe(r, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void rule_run_context_init(rule_run_context * restrict ctx)
{
  memset(ctx, 0, sizeof(*ctx));

  pattern_search_matches_init(&ctx->search_rhs_hits);
  pattern_search_matches_init(&ctx->search_hits);
  vertices_init(&ctx->generate_hits);

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  llist_init(&ctx->operations);
}

void rule_run_context_destroy(rule_run_context * restrict ctx)
{
  graph_invalidation_end(&ctx->invalidation);
  hashtable_destroy(&ctx->search_hits);
  hashtable_destroy(&ctx->search_rhs_hits);
  vertices_destroy(&ctx->generate_hits);
  free(ctx->Alist);
  free(ctx->Blist);
  free(ctx->Mlist);

  RUNTIME_ASSERT(llist_empty(&ctx->operations));
}

void rule_stats_build(struct rule * restrict rule, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&rule->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&rule->rule_stats.record, &descriptor_stats_rule, builder_writer);
}

void rule_describe(const struct rule * restrict rule, struct bacon_writer * restrict builder_writer)
{
  edges list, *eln;
  moria_edge *e;

  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&rule->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);

    { SET
      vertex_mappings(&rule->vertex, builder_writer);
      if(rule->name_len) {
        MAPPING STRING("name")
        BYTES(rule->name, rule->name_len)
      }
      { MAPPING STRING("location")
        LOADF("%s [%d,%d - %d,%d]", rule->loc.fname, rule->loc.f_lin + 1, rule->loc.f_col + 1, rule->loc.l_lin + 1, rule->loc.l_col + 1);
      }
      { MAPPING
        STRING("dir")
        STRING(attrs16_name_byvalue(rule_direction_attrs, rule->dir))
      }
      { MAPPING
        STRING("cardinality")
        STRING(attrs16_name_byvalue(rule_cardinality_attrs, rule->card))
      }
      { MAPPING STRING("text")
        bacon_writer_push_string(builder_writer);
        rule_say(rule, builder_writer->N);
        bacon_writer_pop_string(builder_writer);
      }
      if(edges_gather_up(&rule->vertex, EDGE_RULE_RMA, &list)) {
        MAPPING STRING("rmas")
        { SET
          edges_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        edges_destroy(&list);
      }

      vertex_relations_describe(&rule->vertex, builder_writer);
    }
  }
}

void rule_identity_describe(const struct rule * restrict rule, bacon_writer * restrict builder_writer)
{
  attrs32_describe(rule->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(rule->id, rule->id_len);
  }
  if(rule->name_len) {
    MAPPING STRING("name")
    BYTES(rule->name, rule->name_len);
  }

  /* location */
  bacon_writer_push_string(builder_writer);
  narrator_says(builder_writer->N, rule->loc.fname);
  narrator_sayf(builder_writer->N, " [%d,%d - %d,%d] ", rule->loc.f_lin + 1, rule->loc.f_col + 1, rule->loc.l_lin + 1, rule->loc.l_col + 1);

  /* contents */
  rule_say(rule, builder_writer->N);
  bacon_writer_pop_string(builder_writer);
}

size_t rule_path_cat(void * restrict dst, size_t sz, const rule * restrict rule)
{
  return zcatf(dst, sz, "//bm/rules/%.*s", rule->id_len, rule->id);
}

static void rule_tell_describe(const struct rule * restrict rule, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&rule->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);

    { SET
      vertex_mappings(&rule->vertex, builder_writer);

      { MAPPING STRING("dir")
        STRING(attrs16_name_byvalue(rule_direction_attrs, rule->dir))
      }
      { MAPPING STRING("cardinality")
        STRING(attrs16_name_byvalue(rule_cardinality_attrs, rule->card))
      }

      if(rule->search) {
        MAPPING STRING("search")
        pattern_describe(&rule->search->pattern, builder_writer);
      }
      if(rule->generate) {
        MAPPING STRING("generate")
        pattern_describe(&rule->generate->pattern, builder_writer);
      }
      if(rule->formula) {
        MAPPING STRING("formula")
        pattern_describe(&rule->formula->pattern, builder_writer);
      }
      if(rule->target) {
        MAPPING STRING("target")
        pattern_describe(&rule->target->pattern, builder_writer);
      }
      if(rule->attachment) {
        MAPPING STRING("attachment")
        pattern_describe(&rule->attachment->pattern, builder_writer);
      }
      if(rule->search_rhs) {
        MAPPING STRING("search-rhs")
        pattern_describe(&rule->search_rhs->pattern, builder_writer);
      }
    }
  }
}

void rule_tell(const rule * restrict rule, struct request_handler * restrict handler)
{
  bamipc_message *msg;
  narrator_fixed fixed;
  narrator *N;
  bacon_writer writer, *builder_writer;

  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);
  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  rule_tell_describe(rule, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  msg->len = fixed.l;
  request_handler_post(handler, msg);
}

void rule_unlink(struct rule * restrict rule, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  moria_foreach_edge(&st, &rule->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULE_RMA:
        vertices_push(ctx->unlinking, edge->A);
      case EDGE_RULE_RULESET:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

bool rule_reap(struct rule * restrict rule, struct graph_disintegrate_context * restrict ctx)
{
  if(edges_check_up(&rule->vertex, EDGE_RULE_RULESET)) {
    return false;
  }

  /* if no longer referenced by any rma, then this rule is eligible to be reaped and any
   * of its edges will be cleaned up */
  return true;
}

void rule_id(const struct rule * restrict rule, const char ** restrict id, uint16_t * restrict idl)
{
  *id = rule->id;
  *idl = rule->id_len;
}

void rule_compute_id(const rule * restrict rule, identity_ctx * restrict id)
{
  char str[128];

  /* build.bam file containing this rule */
  vertex_compute_id(&rule->container->vertex, id);
  identity_add(id, "\n", 1);

  /* location */
  zcatf(str, sizeof(str), "%u", rule->loc.f_lin + 1);
  identity_name(id, VERTEX_RULE, str, strlen(str));
}

#if CONFIG_DEVEL
void rules_show(void)
{
  struct rule *rule;
  int x = 0;

  printf("rules, live : %zu free : %zu\n", llist_count(&rule_list), llist_count(&rule_freelist));
  llist_foreach(&rule_list, rule, vertex.owner) {
    printf("%3u %p", x++, rule);
    printf("\n");
  }
}
#endif
