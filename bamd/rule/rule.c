/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "common/assure.h"
#include "common/attrs.h"
#include "common/hash.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "bam/stats.h"

#include "rule.h"
#include "dependency.h"
#include "formula.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "generate.h"
#include "search.internal.h"
#include "module.h"
#include "rule_module_association.h"
#include "rule_system.h"
#include "dir_rma.h"
#include "stats.h"
#include "probes.h"
#include "channel.h"
#include "events.h"
#include "system_state.h"
#include "lookup.h"
#include "selection.h"
#include "rules.h"
#include "reconcile.h"
#include "buildplan_entity.h"
#include "meta_edge.h"
#include "entlist.h"
#include "shadow.h"
#include "fsedge.h"
#include "index_vertex.h"

#include "macros.h"

#include "pattern/section.h"
#include "render.h"
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

static void connector_say(const rule * restrict r, narrator * restrict N)
{
  if(r->card == RULE_MANY_TO_ONE || r->card == RULE_MANY_TO_MANY)
    xsays("[*] ");

  if(r->dir == RULE_LTR)
    xsays("->");
  else if(r->dir == RULE_RTL)
    xsays("<-");

  if(r->card == RULE_ONE_TO_MANY || r->card == RULE_MANY_TO_MANY)
    xsays(" [*]");
}

static formula *formula_resolve(rule * restrict rule, rule_run_context * restrict ctx)
{
  fsent *fml_n;
  formula *fml;
  channel *chan;
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  meta_edge *me;

  entlist_destroy(&ctx->generate_hits);
  pattern_generate(
      rule->formula
    , ctx->rma->mod
    , ctx->rma->dirnode
    , &ctx->invalidation
    , 0
    , &ctx->generate_hits
    , 0
  );
  ctx->rma->stats.formula_hits = llist_count(&ctx->generate_hits.lln);

  RUNTIME_ASSERT(llist_count(&ctx->generate_hits.lln) <= 1);
  fml_n = entlist_first(&ctx->generate_hits)->p;

  /* formula attachment did not resolve, or resolved to an uncreated fsent, possibly one that
   * was vivified by running the generate formula */
  if(!fml_n || !vertex_exists_get(&fml_n->vertex)) {
    msg = system_error_event(&chan);
    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("unresolved formula ref ")
      { SET
        vertex_describe(&ctx->rma->vertex, builder_writer);
      }
    }
    bacon_writer_close(&writer);

    msg->size = fixed.l;
    msg->code = EINVAL;
    events_publish(chan, msg);
    return 0;
  }

  fsent_formula_bootstrap(fml_n, &ctx->invalidation);
  fml = fml_n->self_fml;

  /* fml -> rma meta-edge */
  meta_connect(&fml->vertex, &ctx->rma->vertex, EDGE_FML_RMA, &ctx->invalidation, &me);
  me->epoch = rule_system_epoch;

  return fml;
}

static void bpe_create(
    rule_run_context * restrict ctx
  , struct buildplan_entity_search_context * restrict sctx
  , buildplan_entity ** restrict bpep
)
{
  buildplan_entity *bpe;
  meta_edge *me;
  fsent *A;
  entlist list, *eln;

  buildplan_entity_create_hyper(sctx->Blist, sctx->Blen, sctx->Alist, sctx->Alen, ctx->fml, ctx->rma, &bpe);

  /* rma -> bpe meta-edge */
  meta_connect(&ctx->rma->vertex, &bpe->vertex, EDGE_RMA_BPE, &ctx->invalidation, &me);
  me->epoch = rule_system_epoch;

  /* connect */
  rule_connect_probe(ctx->rma->rule, bpe);

  /* an fsent with an incoming EDGE_BPE_TGT can become invalidated */
  buildplan_entity_targets(bpe, &list);
  entlist_foreach(&list, A, eln) {
    if(!A->is_target) {
      A->is_target = true;
      vertex_invalidate(&A->vertex, &ctx->invalidation);
    }

    if(!bpe->fml) {
      continue;
    }
  }

  entlist_destroy(&list);

  rule_run_probe(ctx, bpe, true);

  buildplan_entity_cache_put(sctx, bpe);
  buildplan_entity_validate_enqueue(bpe);

  *bpep = bpe;
}

static void run_search_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  hashtable_recycle(&ctx->search_hits);

  if(ctx->rma->mod->dir_node == ctx->rma->mod->anchor_node || rule->anchor_search) {
    pattern_search(
        rule->search
      , ctx->rma->mod
      , ctx->rma->mod->anchor_node
      , &ctx->search_hits
      , dir_rma_connect_visit
      , ctx->rma
    );
  }
  if(ctx->rma->mod->dir_node != ctx->rma->mod->anchor_node && rule->module_search) {
    pattern_search(
        rule->search
      , ctx->rma->mod
      , ctx->rma->mod->dir_node
      , &ctx->search_hits
      , dir_rma_connect_visit
      , ctx->rma
    );
  }

  ctx->rma->stats.search_hits += ctx->search_hits.size;
  if(ctx->search_hits.size) {
    rule_system_rma_hit(ctx, ctx->rma);
  } else {
    rule_system_rma_nohit(ctx, ctx->rma);
  }
}

static void run_generate_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  entlist_destroy(&ctx->generate_hits);
  pattern_generate(
      rule->generate
    , ctx->rma->mod
    , ctx->rma->mod->dir_node
    , &ctx->invalidation
    , 0
    , &ctx->generate_hits
    , 0
  );

  ctx->rma->stats.generate_hits += llist_count(&ctx->generate_hits.lln);
  if(llist_count(&ctx->generate_hits.lln)) {
    rule_system_rma_hit(ctx, ctx->rma);
  } else {
    rule_system_rma_nohit(ctx, ctx->rma);
  }
}


static void run_generate(rule_run_context * restrict ctx, pattern_search_node * restrict match)
{
  pattern_generate(
      ctx->rma->rule->generate
    , ctx->rma->mod
    , ctx->rma->mod->dir_node
    , &ctx->invalidation
    , match
    , &ctx->generate_hits
    , 0
  );

  ctx->rma->stats.generate_hits += llist_count(&ctx->generate_hits.lln);
}

static int pattern_search_compare(const void *a, const void *b)
{
  const struct pattern_search_node *A = *(typeof(A)*)a;
  const struct pattern_search_node *B = *(typeof(B)*)b;

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
  pattern_search_node *match;

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
  pattern_search_node *match;
  buildplan_entity *bpe;
  moria_vertex *v;
  struct buildplan_entity_search_context sctx;
  int x;

  /* run the match pattern */
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
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe_create(ctx, &sctx, &bpe);
  }
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
  entlist *eln;

  run_generate_tracking(rule, ctx);
  entlist_foreach(&ctx->generate_hits, n, eln) {
    v = &n->vertex;
    sctx.Blist = 0;
    sctx.Blen = 0;
    sctx.Alist = &v;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe_create(ctx, &sctx, &bpe);
  }
}

/*
 * connector : search-pattern [*] --
 */
static void __attribute__((nonnull)) search_zero_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_node * match = 0;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  size_t z;
  int x;

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
    return;
  }

  sctx.Blist = 0;
  sctx.Blen = 0;
  sctx.Alist = ctx->Alist;
  sctx.Alen = z;

  vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
  sctx.rma = ctx->rma;
  if((bpe = bpe_search(ctx, &sctx))) {
    return;
  }
  bpe_create(ctx, &sctx, &bpe);
}

/*
 * connector : -- [*] generate
 */
static void __attribute__((nonnull)) generate_zero_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  fsent *n;
  size_t z;
  entlist *eln;

  run_generate_tracking(rule, ctx);

  z = 0;
  assure(&ctx->Alist, sizeof(*ctx->Alist), llist_count(&ctx->generate_hits.lln), &ctx->Alist_alloc);

  entlist_foreach(&ctx->generate_hits, n, eln) {
    ctx->Alist[z++] = &n->vertex;
  }
  if(z == 0) {
    return;
  }

  sctx.Blist = 0;
  sctx.Blen = 0;
  sctx.Alist = ctx->Alist;
  sctx.Alen = z;

  vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
  sctx.rma = ctx->rma;
  if((bpe = bpe_search(ctx, &sctx))) {
    return;
  }
  bpe_create(ctx, &sctx, &bpe);
}

/*
 * connector : search -> generate
 */
static void __attribute__((nonnull)) search_one_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_node * match = 0;
  buildplan_entity *bpe;
  int x;
  fsent * generated;
  struct buildplan_entity_search_context sctx;
  entlist *eln;

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  run_search_tracking(rule, ctx);
  if(ctx->search_hits.size == 0 && !system_error) {
    rule_run_probe(ctx, 0, true);
  }

  for(x = 0; x < ctx->search_hits.table_size && !system_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    entlist_foreach(&ctx->generate_hits, generated, eln) {
      ctx->Blist[0] = &match->node->vertex; // source, down
      ctx->Alist[0] = &generated->vertex;   // target, up

      sctx = (typeof(sctx)) {
          Blist : ctx->Blist
        , Blen : 1
        , Alist : ctx->Alist
        , Alen : 1
      };

      vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
      sctx.rma = ctx->rma;
      if((bpe = bpe_search(ctx, &sctx))) {
        continue;
      }
      bpe_create(ctx, &sctx, &bpe);
    }
  }
}

/*
 * connector : search <- generate
 */
static void __attribute__((nonnull)) generate_one_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_node * match = 0;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  fsent * generated;
  entlist *eln;
  int x;

  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size && !system_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    entlist_foreach(&ctx->generate_hits, generated, eln) {
      ctx->Blist[0] = &generated->vertex;   // source, down
      ctx->Alist[0] = &match->node->vertex; // target, up

      sctx.Blist = ctx->Blist;
      sctx.Blen = 1;
      sctx.Alist = ctx->Alist;
      sctx.Alen = 1;

      vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
      sctx.rma = ctx->rma;
      if((bpe = bpe_search(ctx, &sctx))) {
        continue;
      }
      bpe_create(ctx, &sctx, &bpe);
    }
  }
}

/*
 * connector : search -> [*] generate
 */
static void __attribute__((nonnull)) search_one_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_node * match = 0;
  buildplan_entity *bpe;
  int x;
  size_t Asz;
  fsent *n = 0;
  struct buildplan_entity_search_context sctx;
  entlist *eln;

  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size && !system_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Blist[0] = &match->node->vertex;

    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    assure(&ctx->Alist, sizeof(*ctx->Alist), llist_count(&ctx->generate_hits.lln), &ctx->Alist_alloc);
    Asz = 0;
    entlist_foreach(&ctx->generate_hits, n, eln) {
      ctx->Alist[Asz++] = &n->vertex;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = 1;
    sctx.Alist = ctx->Alist;
    sctx.Alen = Asz;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe_create(ctx, &sctx, &bpe);
  }
}

/*
 * connector : search [*] <- generate
 */
static void __attribute__((nonnull)) generate_one_to_many(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_node * match = 0;
  buildplan_entity *bpe;
  size_t Bsz;
  fsent *n = 0;
  struct buildplan_entity_search_context sctx;
  int x;
  entlist *eln;

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);

  run_search_tracking(rule, ctx);
  for(x = 0; x < ctx->search_hits.table_size && !system_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Alist[0] = &match->node->vertex;

    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    assure(&ctx->Blist, sizeof(*ctx->Blist), llist_count(&ctx->generate_hits.lln), &ctx->Blist_alloc);
    Bsz = 0;
    entlist_foreach(&ctx->generate_hits, n, eln) {
      ctx->Blist[Bsz++] = &n->vertex;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    if((bpe = bpe_search(ctx, &sctx))) {
      return;
    }
    bpe_create(ctx, &sctx, &bpe);
  }
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
  fsent *n = 0;
  entlist *eln;

  run_search_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->search_hits.size, &ctx->Blist_alloc);

  x = 0;
  for(y = 1; y <= ctx->search_hits.size && !system_error; y++)
  {
    if(y < ctx->search_hits.size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Bsz = 0; Bsz < (y - x); Bsz++) {
      ctx->Blist[Bsz] = &ctx->Mlist[x + Bsz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, ctx->Mlist[x]);

    entlist_foreach(&ctx->generate_hits, n, eln) {
      ctx->Alist[0] = &n->vertex;

      sctx.Blist = ctx->Blist;
      sctx.Blen = Bsz;
      sctx.Alist = ctx->Alist;
      sctx.Alen = 1;

      vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
      /* check for an existing bpe */
      sctx.rma = ctx->rma;
      if((bpe = bpe_search(ctx, &sctx))) {
        goto next;
      }
      bpe_create(ctx, &sctx, &bpe);
    }

  next:
    x = y;
  }
}

/*
 * connector : search <- [*] generate
 */
static void __attribute__((nonnull)) generate_many_to_one(rule * restrict rule, rule_run_context * restrict ctx)
{
  pattern_search_node * match = 0;
  buildplan_entity *bpe;
  struct buildplan_entity_search_context sctx;
  size_t Bsz;
  fsent *n = 0;
  entlist *eln;
  int x;

  run_search_tracking(rule, ctx);

  for(x = 0; x < ctx->search_hits.table_size && !system_error; x++)
  {
    if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
      continue;
    }

    ctx->Alist[0] = &match->node->vertex;
    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, match);

    Bsz = 0;
    assure(&ctx->Blist, sizeof(*ctx->Blist), llist_count(&ctx->generate_hits.lln), &ctx->Blist_alloc);
    entlist_foreach(&ctx->generate_hits, n, eln) {
      ctx->Blist[Bsz++] = &n->vertex;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = 1;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    if((bpe = bpe_search(ctx, &sctx))) {
      continue;
    }
    bpe_create(ctx, &sctx, &bpe);
  }
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
  fsent *n = 0;
  struct buildplan_entity_search_context sctx;
  entlist *eln;

  /* run the match pattern */
  run_search_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->search_hits.size, &ctx->Blist_alloc);

  x = 0;
  for(y = 1; y <= ctx->search_hits.size && !system_error; y++)
  {
    if(y < ctx->search_hits.size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Bsz = 0; Bsz < (y - x); Bsz++) {
      ctx->Blist[Bsz] = &ctx->Mlist[x + Bsz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, ctx->Mlist[x]);

    assure(&ctx->Alist, sizeof(*ctx->Alist), llist_count(&ctx->generate_hits.lln), &ctx->Alist_alloc);
    Asz = 0;
    entlist_foreach(&ctx->generate_hits, n, eln) {
      ctx->Alist[Asz++] = &n->vertex;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = Asz;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    if((bpe = bpe_search(ctx, &sctx))) {
      goto next;
    }
    bpe_create(ctx, &sctx, &bpe);

  next:
    x = y;
  }
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
  fsent *n;
  entlist *eln;

  /* run the match pattern */
  run_search_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->search_hits.size, &ctx->Alist_alloc);

  x = 0;
  for(y = 1; y <= ctx->search_hits.size && !system_error; y++)
  {
    if(y < ctx->search_hits.size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    entlist_destroy(&ctx->generate_hits);
    run_generate(ctx, ctx->Mlist[x]);

    assure(&ctx->Blist, sizeof(*ctx->Blist), llist_count(&ctx->generate_hits.lln), &ctx->Blist_alloc);
    Bsz = 0;
    entlist_foreach(&ctx->generate_hits, n, eln) {
      ctx->Blist[Bsz++] = &n->vertex;
    }

    sctx.Blist = ctx->Blist;
    sctx.Blen = Bsz;
    sctx.Alist = ctx->Alist;
    sctx.Alen = Asz;

    vivify_targets(sctx.Alist, sctx.Alen, &ctx->invalidation);
    sctx.rma = ctx->rma;
    if((bpe = bpe_search(ctx, &sctx))) {
      goto next;
    }
    bpe_create(ctx, &sctx, &bpe);

  next:
    x = y;
  }
}

//
// public
//

static void rule_destroy(rule * restrict r)
{
  pattern_free(r->search);
  pattern_free(r->generate);
  pattern_free(r->formula);
  pattern_free(r->target);
  pattern_free(r->attachment);
  pattern_free(r->search_rhs);
}

void rule_dispose(rule * restrict r)
{
  rule_destroy(r);

  llist_delete(&r->vertex.owner);
  llist_append(&rule_freelist, &r->vertex.owner);
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
  llist *T;

  llist_foreach_safe(&rule_list, r, vertex.owner, T) {
    rule_dispose(r);
  }

  llist_foreach_safe(&rule_freelist, r, vertex.owner, T) {
    free(r);
  }
}

static size_t loc_hash(char * restrict dst, size_t dst_size, const yyu_location * restrict loc)
{
  uint64_t h;

  h = 0;
  h = hash64(h, loc->fname, strlen(loc->fname));
  h = hash64(h, loc, sizeof(*loc));

  return zcatf(dst, dst_size, "%016lx", h);
}

int rule_validate(const rule * restrict rule, char * restrict error, size_t error_sz)
{
  /* only those invalid cases which absolutely cannot be detected by the parser
   * should be considered here */
  if(rule->card == RULE_MANY_TO_ONE) {
    if(rule->search && rule->search->capture_groups_count > 42) {
      return zcatf(
          error, error_sz, "many-to-one rules search pattern cannot have capturing groups @ %s [%d,%d - %d,%d]"
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

void rule_mk(
    rule ** restrict rulep
  , moria_graph * restrict g
  , pattern * search
  , pattern * generate
  , pattern * formula
  , pattern * target
  , pattern * attachment
  , pattern * search2
  , const rule_bacon * restrict bacon
  , uint32_t attrs
  , const yyu_location * restrict loc
)
{
  rule * r;
  bool anchor_search;
  bool module_search;
  uint64_t token;
  uint64_t hash;

  if((r = llist_shift(&rule_freelist, typeof(*r), vertex.owner)) == 0) {
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
  if((r->graph = bacon->graph) == 0) {
    r->graph = EDGE_DEPENDS;
  }
  if(loc) {
    memcpy(&r->loc, loc, sizeof(r->loc));
  } else {
    memset(&r->loc, 0, sizeof(r->loc));
  }
  anchor_search = bacon->anchor_search;
  module_search = bacon->module_search;
  if(!anchor_search && !module_search) {
    anchor_search = true;
    module_search = true;
  }
  r->anchor_search = anchor_search;
  r->module_search = module_search;

  if(bacon->id_len) {
    r->id_len = zcatw(r->id, sizeof(r->id), bacon->id, bacon->id_len);
  } else {
    r->id_len = loc_hash(r->id, sizeof(r->id), loc);
  }

  hash = hash64(0, r->id, r->id_len);
  token = atomic_fetch_inc(&vertex_token_counter);
  r->token_len = zcatf(r->token, sizeof(r->token), "%016"PRIx64, hash64(hash, MM(token)));

  *rulep = r;
}

static fsent *resolve_attachment(rule * restrict bind, rule_run_context * restrict ctx)
{
  fsent *attachment;

  entlist_destroy(&ctx->generate_hits);
  pattern_generate(
      bind->attachment
    , ctx->rma->mod
    , ctx->rma->mod->dir_node
    , &ctx->invalidation
    , 0 /* match */
    , &ctx->generate_hits
    , PATTERN_GENERATE_NOFOLLOW //VERTEX_DIR
  );

  RUNTIME_ASSERT(llist_count(&ctx->generate_hits.lln) <= 1);

  attachment = 0;
  if(llist_count(&ctx->generate_hits.lln) == 1) {
    attachment = entlist_first(&ctx->generate_hits)->p;
  }

  return attachment;
}

static void bind_one_to_one(struct rule * restrict bind, fsent *attachment, rule_run_context * restrict ctx)
{
  moria_vertex *target;
  meta_edge *me;

  if(llist_count(&ctx->target_hits.lln) == 0) {
    bind_run_probe(bind, ctx->rma->mod, 0, 0, 0);
    return;
  }
  RUNTIME_ASSERT(llist_count(&ctx->target_hits.lln) == 1);
  target = entlist_first(&ctx->target_hits)->p;

  /* promote the attachment node to a link */
  if(vertex_filetype_get(&attachment->vertex) == VERTEX_FILETYPE_REG) {
    vertex_filetype_set(&attachment->vertex, VERTEX_FILETYPE_LINK);
    attachment->vertex.attrs |= MORIA_VERTEX_LINK;
  }
  RUNTIME_ASSERT(vertex_type_get(&attachment->vertex) == VERTEX_SHADOW_LINK);

  attachment->vertex.ref = target;

  meta_connect(&ctx->rma->vertex, &attachment->vertex, EDGE_RMA_TGT, &ctx->invalidation, &me);
  me->epoch = rule_system_epoch;

  bind_run_probe(bind, ctx->rma->mod, 0, attachment, containerof(target, fsent, vertex));
}

static void bind_one_to_many(struct rule * restrict bind, fsent *attachment, rule_run_context * restrict ctx)
{
  moria_vertex *v;
  fsent *target;
  fsent *location;
  int x;
  pattern_search_node *match;
  entlist *eln;
  meta_edge *me;

  /* promote the attachment node to a directory */
  if(vertex_filetype_get(&attachment->vertex) == VERTEX_FILETYPE_REG) {
    vertex_filetype_set(&attachment->vertex, VERTEX_FILETYPE_DIR);
  }
  RUNTIME_ASSERT(vertex_type_get(&attachment->vertex) == VERTEX_SHADOW_DIR);

  if(bind->target) {
    entlist_foreach(&ctx->target_hits, v, eln) {
      target = containerof(v, typeof(*target), vertex);

      // link @ //module/uses/$name
      shadow_graft_bind(ctx->rma->mod, target, attachment, &location, &ctx->invalidation);

      meta_connect(&ctx->rma->vertex, &location->vertex, EDGE_RMA_TGT, &ctx->invalidation, &me);
      me->epoch = rule_system_epoch;

      bind_run_probe(bind, ctx->rma->mod, attachment, location, target);
    }
    if(llist_empty(&ctx->target_hits.lln)) {
      bind_run_probe(bind, ctx->rma->mod, attachment, 0, 0);
    }
  } else if(bind->search) {
    for(x = 0; x < ctx->search_hits.table_size; x++)
    {
      if(!(match = hashtable_table_entry(&ctx->search_hits, x))) {
        continue;
      }

      target = match->node;
      // link @ //module/uses/$name
      shadow_graft_bind(ctx->rma->mod, target, attachment, &location, &ctx->invalidation);

      meta_connect(&ctx->rma->vertex, &location->vertex, EDGE_RMA_TGT, &ctx->invalidation, &me);
      me->epoch = rule_system_epoch;

      bind_run_probe(bind, ctx->rma->mod, attachment, location, target);
    }
    if(ctx->search_hits.size == 0) {
      bind_run_probe(bind, ctx->rma->mod, attachment, 0, 0);
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
  pattern_search_node *match;

  /* LHS */
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
    , ctx->rma->mod->dir_node
    , &ctx->search_rhs_hits
    , dir_rma_connect_visit
    , ctx->rma
  );
  ctx->rma->stats.search_hits += ctx->search_rhs_hits.size;

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
  if((bpe = bpe_search(ctx, &sctx))) {
    return;
  }
  bpe_create(ctx, &sctx, &bpe);
}

static void bind_run(struct rule * restrict bind, rule_run_context * restrict ctx)
{
  index_vertex *index;
  entlist *eln;
  meta_edge *me;
  fsent *attachment;

  entlist_destroy(&ctx->target_hits);
  entlist_destroy(&ctx->target_index_hits);

  /* attachment */
  if((attachment = resolve_attachment(bind, ctx)) == 0) {
    ctx->rma->stats.attachment_hits = 0;
    return;
  }
  RUNTIME_ASSERT(vertex_shadowtype_get(&attachment->vertex));
  ctx->rma->stats.attachment_hits = 1;

  if(bind->target) {
    /* resolve targets pattern */
    pattern_lookup_indexed(bind->target, ctx->rma->mod->vars, &ctx->target_index_hits, &ctx->target_hits);
    ctx->rma->stats.target_hits = llist_count(&ctx->target_hits.lln);
    if(system_error) {
      goto end;
    }

    /* take an edge on each index_vertex visited during targets resolution */
    entlist_foreach(&ctx->target_index_hits, index, eln) {
      meta_connect(&index->vertex, &ctx->rma->vertex, EDGE_INDEX_RMA, &ctx->invalidation, &me);
      me->epoch = rule_system_epoch;
    }
    bind_connect_probe(bind, ctx->rma->mod, &ctx->target_index_hits);
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
  entlist_destroy(&ctx->target_index_hits);
}

void rule_run(rule * restrict rule, rule_run_context * restrict ctx)
{
  RUNTIME_ASSERT(rule->graph);
  STATS_INC(g_stats.rule_run);
  STATS_INC(ctx->rma->stats.rule_run);

  graph_invalidation_begin(&ctx->invalidation);

  /* stats are only posted on success */
  ctx->rma->stats.target_hits = 0;
  ctx->rma->stats.attachment_hits = 0;
  ctx->rma->stats.search_hits = 0;
  ctx->rma->stats.generate_hits = 0;
  ctx->rma->stats.formula_hits = 0;
  ctx->rma->stats.dependencies = 0;

  ctx->fml = 0;
  if(rule->formula) {
    if((ctx->fml = formula_resolve(rule, ctx)) == 0) {
      goto end;
    }
  }

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
    case RULE_BIND_ONE:
    case RULE_BIND_MANY:
      bind_run(rule, ctx);
      break;
    case RULE_SEARCH_SEARCH:
      search_search_run(rule, ctx);
      break;
    default:
      RTA;
  }

end:
  if(!system_error) {
    STATS_INC(g_stats.rule_run_ok);
    ctx->rma->stats.rule_run_ok++;
  }

  graph_invalidation_end(&ctx->invalidation);
}

void rule_say(const rule * restrict r, narrator * restrict N)
{
  if(r->target) {
    xsayf("bind { id : %.*s } ", r->id_len, r->id);
    pattern_say(r->target, N);
    xsays(" -> ");
    pattern_say(r->attachment, N);
    return;
  }
  if(r->card == RULE_SEARCH_SEARCH) {
    xsayf("rule { id : %.*s } ", r->id_len, r->id);
    xsays("search ");
    pattern_say(r->search, N);
    xsays("[*] -> [*] ");
    xsays("search ");
    pattern_say(r->search_rhs, N);
  }

  xsayf("rule { id : %.*s } ", r->id_len, r->id);

  if(r->search)
  {
    pattern_say(r->search, N);
    xsays(" ");
  }
  else if(r->card == RULE_ZERO_TO_MANY)
  {
    xsays("-- [*] ");
  }
  else
  {
    xsays("-- ");
  }

  connector_say(r, N);

  if(r->generate)
  {
    xsays(" ");
    pattern_say(r->generate, N);
  }
  else if(r->card == RULE_ZERO_TO_MANY)
  {
    xsays(" [*] --");
  }
  else
  {
    xsays(" --");
  }

  if(r->formula)
  {
    narrator_says(N, " : ");
    pattern_say(r->formula, N);
  }
}

void rule_describe(const rule * restrict rule, struct bacon_writer * restrict builder_writer)
{
  char buf[512];
  narrator_fixed fixed;

  { MAPPING
    STRING(rule->id)
    { SET
      { MAPPING
        STRING("location")
        LOADF("%s [%d,%d - %d,%d]", rule->loc.fname, rule->loc.f_lin + 1, rule->loc.f_col + 1, rule->loc.l_lin + 1, rule->loc.l_col + 1);
      }
      { MAPPING
        STRING("graph")
        STRING(attrs32_name_byvalue(graph_edge_attrs, rule->graph))
      }
      { MAPPING
        STRING("dir")
        STRING(attrs16_name_byvalue(rule_direction_attrs, rule->dir))
      }
      { MAPPING
        STRING("cardinality")
        STRING(attrs16_name_byvalue(rule_cardinality_attrs, rule->card))
      }
      { MAPPING
        STRING("anchor-search")
        BOOL(rule->anchor_search)
      }
      { MAPPING
        STRING("module-search")
        BOOL(rule->module_search)
      }
      if(rule->target) {
        MAPPING
        STRING("target")
        narrator_fixed_init(&fixed, buf, sizeof(buf));
        pattern_say(rule->target, &fixed.base);
        BYTES(buf, fixed.l);
      }
      if(rule->attachment) {
        MAPPING
        STRING("attachment")
        narrator_fixed_init(&fixed, buf, sizeof(buf));
        pattern_say(rule->attachment, &fixed.base);
        BYTES(buf, fixed.l);
      }
      if(rule->search) {
        MAPPING
        STRING("search")
        narrator_fixed_init(&fixed, buf, sizeof(buf));
        pattern_say(rule->search, &fixed.base);
        BYTES(buf, fixed.l);
      }
      if(rule->generate) {
        MAPPING
        STRING("generate")
        narrator_fixed_init(&fixed, buf, sizeof(buf));
        pattern_say(rule->generate, &fixed.base);
        BYTES(buf, fixed.l);
      }
      if(rule->search_rhs) {
        MAPPING
        STRING("search-rhs")
        narrator_fixed_init(&fixed, buf, sizeof(buf));
        pattern_say(rule->search_rhs, &fixed.base);
        BYTES(buf, fixed.l);
      }
      if(rule->formula) {
        MAPPING
        STRING("formula")
        narrator_fixed_init(&fixed, buf, sizeof(buf));
        pattern_say(rule->formula, &fixed.base);
        BYTES(buf, fixed.l);
      }
    }
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
  entlist_init(&ctx->generate_hits);
  entlist_init(&ctx->target_hits);
  entlist_init(&ctx->target_index_hits);

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);
}

void rule_run_context_destroy(rule_run_context * restrict ctx)
{
  graph_invalidation_end(&ctx->invalidation);
  hashtable_destroy(&ctx->search_hits);
  hashtable_destroy(&ctx->search_rhs_hits);
  entlist_destroy(&ctx->generate_hits);
  entlist_destroy(&ctx->target_hits);
  entlist_destroy(&ctx->target_index_hits);
  free(ctx->Alist);
  free(ctx->Blist);
  free(ctx->Mlist);
}
