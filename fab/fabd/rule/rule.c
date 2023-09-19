/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <errno.h>

#include "types.h"

#include "common/assure.h"
#include "common/attrs.h"
#include "moria/edge.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "rule.h"
#include "dependency.h"
#include "formula.h"
#include "fsent.h"
#include "generate.h"
#include "search.internal.h"
#include "module.h"
#include "rule_module.h"
#include "rule_system.h"
#include "stats.h"
#include "channel.h"
#include "events.h"
#include "system_state.h"

#include "macros.h"

llist rule_list = LLIST_INITIALIZER(rule_list);                 // active rule
static llist rule_freelist = LLIST_INITIALIZER(rule_freelist);  // freelist

llist rde_list = LLIST_INITIALIZER(rde_list);                   // active
static llist rde_freelist = LLIST_INITIALIZER(rde_freelist);    // freelist

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

static void rde_alloc(rule_dirnode_edge ** rdep)
{
  rule_dirnode_edge *rde;

  if((rde = llist_shift(&rde_freelist, typeof(*rde), edge.owner)) == 0)
  {
    xmalloc(&rde, sizeof(*rde));
  }

  moria_edge_init(&rde->edge, &g_graph);
  llist_append(&rde_list, rde, edge.owner);

  *rdep = rde;
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

struct rule_dirnode_connect_context {
  rule *r;
  module *mod;
  rule_module_edge *rme;
};

/* connect rule -> directory fsent */
static void rule_dirnode_connect(void * restrict _ctx, fsent * restrict node)
{
  int r;
  moria_connect_context connect_ctx;
  struct rule_dirnode_connect_context *ctx = _ctx;
  rule_dirnode_edge *rde = 0;

  r = moria_preconnect(&connect_ctx, &g_graph, &ctx->r->vertex, &node->vertex, EDGE_RULE_DIR, 0);
  if (r == MORIA_HASEDGE) {
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  rde_alloc(&rde);
  graph_connect(&connect_ctx, &ctx->r->vertex, &node->vertex, &rde->edge, EDGE_RULE_DIR);

  rde->rme = ctx->rme;
}

static void rule_dispose(rule * restrict r)
{
  pattern_free(r->match);
  pattern_free(r->generate);
  pattern_free(r->formula);

  llist_delete_node(&r->vertex.owner);
  llist_append(&rule_freelist, r, vertex.owner);
}

static void __attribute__((nonnull)) fml_node_get(
    rule * restrict rule
  , rule_run_context * restrict ctx
  , fsent ** restrict fml_node
)
{
  rule_module_edge * rma;
  int x;
  moria_connect_context connect_ctx;
  int __attribute__((unused)) r;
  moria_edge *e;
  formula *fml = 0;
  narrator_fixed fixed;
  narrator *N;
  fabipc_message *msg;
  channel *chan;

  rma = ctx->rme;

  set_recycle(ctx->generate_nodes);
  pattern_generate(
      rule->formula
    , ctx->mod
    , rma->mod_owner->dir_node
    , ctx->mod->shadow_uses
    , 0
    , &ctx->invalidation
    , 0
    , ctx->generate_nodes
  );

  /* reference patterns can specify at most one fsent */
  RUNTIME_ASSERT(ctx->generate_nodes->size <= 1);

  if(ctx->generate_nodes->size)
  {
    for(x = 0; x < ctx->generate_nodes->table_size; x++)
    {
      if((*fml_node = set_table_get(ctx->generate_nodes, x))) {
        break;
      }
    }
  }

  /* formula attachment did not resolve, or resolved to a non-formula fsent */
  if(ctx->generate_nodes->size != 1 || !fsent_exists_get(*fml_node)) {
    system_error = true;
    if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
      msg->code = EINVAL;

      N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
      narrator_xsays(N, "unresolved formula ref ");
      pattern_say(rule->formula, N);
      narrator_xsayf(N, " @ %s [%d,%d - %d,%d]"
        , rma->mod_owner->self_node_relpath
        , rule->formula->loc.f_lin + 1
        , rule->formula->loc.f_col + 1
        , rule->formula->loc.l_lin + 1
        , rule->formula->loc.l_col + 1
      );
      msg->size = fixed.l;
      events_publish(chan, msg);
    }

    return;
  }

  fsent_formula_bootstrap(*fml_node);
  fml = (*fml_node)->self_fml;

  /* connect the rule vertex and the formula vertex */
  r = moria_preconnect(&connect_ctx, &g_graph, &rule->vertex, &fml->vertex, EDGE_RULE_FML, 0);
  if(r == MORIA_NOEDGE)
  {
    graph_edge_alloc(&e);
    graph_connect(&connect_ctx, &rule->vertex, &fml->vertex, e, EDGE_RULE_FML);
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_HASEDGE);
  }
}

static void run_match_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  struct rule_dirnode_connect_context rdctx;

  rdctx = (typeof(rdctx)){
      .r = rule
    , .mod = ctx->mod
    , .rme = ctx->rme
  };

  pattern_search(rule->match, ctx->mod, ctx->modules, ctx->variants, ctx->match_nodes, rule_dirnode_connect, &rdctx);

  /* this is only actually needed when some probes are enabled */
  if(1)
  {
    if(ctx->match_nodes->size)
    {
      rule_system_rma_hit(ctx, ctx->rme);
    }
    else
    {
      rule_system_rma_nohit(ctx, ctx->rme);
    }
  }
}

static void run_generate_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  set_recycle(ctx->generate_nodes);
  pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, 0, ctx->generate_nodes);

  /* this is only actually needed when some probes are enabled */
  if(1)
  {
    if(ctx->generate_nodes->size)
    {
      rule_system_rma_hit(ctx, ctx->rme);
    }
    else
    {
      rule_system_rma_nohit(ctx, ctx->rme);
    }
  }
}

static int pattern_search_compare(const void *a, const void *b)
{
  const struct pattern_search_node *A = *(typeof(A)*)a;
  const struct pattern_search_node *B = *(typeof(B)*)b;

  int r;
  int x;

  if((r = INTCMP(A->variant, B->variant))) {
    return r;
  }

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

  assure(&ctx->Mlist, sizeof(*ctx->Mlist), ctx->match_nodes->size, &ctx->Mlist_alloc);

  Msz = 0;
  for(y = 0; y < ctx->match_nodes->table_size; y++)
  {
    if(!(match = set_table_get(ctx->match_nodes, y)))
      continue;

    ctx->Mlist[Msz++] = match;
  }

  /* sorted by sets of matching capture groups */
  qsort(ctx->Mlist, Msz, sizeof(*ctx->Mlist), pattern_search_compare);
}

/*
 * Attach a newly-created dependency edge to a rule-module edge
 *
 * dep - dependency edge
 * fml - formula node if any
 */
static void __attribute__((nonnull(1, 3))) dependency_setup(
    dependency * restrict dep
  , fsent * restrict fml
  , rule_run_context * restrict ctx
)
{
  moria_edge *e;
  fsent *n;
  int x;
  rule_module_edge * restrict rme = ctx->rme;
  narrator_fixed fixed;
  narrator *N;
  fabipc_message *msg;
  channel *chan;

  /* this particular dependency edge is instantiated by two different rmes */
  if(dep->rme && dep->rme != rme)
  {
    /* representative target fsent */
    e = &dep->edge;
    if(e->attrs & MORIA_EDGE_HYPER) {
      n = containerof(e->Alist[0].v, typeof(*n), vertex);
    } else {
      n = containerof(e->A, typeof(*n), vertex);
    }

    system_error = true;
    if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
      msg->code = EINVAL;

      N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
      narrator_xsays(N, "fsent ");
      fsent_project_relative_path_say(n, N);
      narrator_xsays(N, " already created by rule (setup)\n ");
      rule_say(dep->rme->rule, g_narrator_stderr);
      narrator_xsayf(N, " @ %s:%d\n"
        , dep->rme->mod_owner->self_node_relpath
        , dep->rme->rule->formula->loc.f_lin + 1
      );
      narrator_xsayf(N, "  dep %p rme %p rule %p\n ", dep, dep->rme, dep->rme->rule);
      rule_say(rme->rule, N);
      narrator_xsayf(N, " @ %s:%d\n"
        , rme->mod_owner->self_node_relpath
        , rme->rule->formula->loc.f_lin + 1
      );
      narrator_xsayf(N, "  dep %p rme %p rule %p ", dep, rme, rme->rule);
      msg->size = fixed.l;
      events_publish(chan, msg);
    }

    return;
  }

  /* mark this edge as having been touched during the rules reconciliation */
  dep->reconciliation_id = rule_system_reconciliation_id;

  /* dependency already associated to this rme */
  if(dep->rme && dep->rme == rme) {
    return;
  }

  RUNTIME_ASSERT(!llist_attached(dep, dependencies_lln));
  llist_append(&rme->dependencies, dep, dependencies_lln);

  dep->rme = rme;
  dep->fml = fml;

  /* rule has no specified formula */
  if(!dep->fml) {
    return;
  }

  /* the formula was not parsed */
  if(!dep->fml->self_fml) {
    return;
  }

  RUNTIME_ASSERT(dep->fml->self_fml);
  dep->fml->self_fml->refs++;

  e = &dep->edge;
  x = 0;
  while(!(e->attrs & MORIA_EDGE_HYPER) || x < e->Alen)
  {
    if((e->attrs & MORIA_EDGE_HYPER))
    {
      n = containerof(e->Alist[x].v, typeof(*n), vertex);
      x++;
    }
    else
    {
      n = containerof(e->A, typeof(*n), vertex);
    }

    if(n->dep && n->dep != dep) {
      /* this can happen when a rule is run twice in a reconciliation and matches a different number
       * of nodes on the different executions - the first edge will be cleaned up */
    }

    if(n->dep != dep) {
      n->dep = dep;
      fsent_invalidate(n, &ctx->invalidation);
    }

    if(!(e->attrs & MORIA_EDGE_HYPER)) {
      break;
    }
  }
}

/*
 * connector : match --
 */
static void __attribute__((nonnull)) match_zero_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node *match;
  dependency *ne;
  int x;
  fsent *n;
  moria_vertex *v;

  /* run the match pattern */
  run_match_tracking(rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    n = match->node;
    v = &n->vertex;

    dependency_hyperconnect(&v, 1, 0, 0, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
   // if(ctx->err->l) {
   //   break;
   // }
  }
}

/*
 * connector : -- generate
 */
static void __attribute__((nonnull)) generate_zero_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  dependency *ne;
  int x;
  moria_vertex *v;
  fsent *n;

  run_generate_tracking(rule, ctx);
  for(x = 0; x < ctx->generate_nodes->table_size; x++)
  {
    if(!(n = set_table_get(ctx->generate_nodes, x)))
      continue;

    v = &n->vertex;

    dependency_hyperconnect(&v, 1, 0, 0, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
    //if(ctx->err->l) {
    //  break;
    //}
  }
}

/*
 * connector : match-pattern [*] --
 */
static void __attribute__((nonnull)) match_zero_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node * match = 0;
  dependency *ne;
  int x;
  size_t z;

  run_match_tracking(rule, ctx);

  z = 0;
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);

  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[z++] = &match->node->vertex;
  }

  if(z)
  {
    dependency_hyperconnect(ctx->Alist, z, 0, 0, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
  }
}

/*
 * connector : -- [*] generate
 */
static void __attribute__((nonnull)) generate_zero_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  dependency *ne;
  int x;
  fsent *n;
  size_t z;

  run_generate_tracking(rule, ctx);

  z = 0;
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->generate_nodes->size, &ctx->Alist_alloc);

  for(x = 0; x < ctx->generate_nodes->table_size; x++)
  {
    if(!(n = set_table_get(ctx->generate_nodes, x)))
      continue;

    ctx->Alist[z++] = &n->vertex;
  }

  dependency_hyperconnect(ctx->Alist, z, 0, 0, rule->relation, &ctx->invalidation, &ne);
  dependency_setup(ne, fml_node, ctx);
}

/*
 * connector : match -> generate
 */
static void __attribute__((nonnull(1, 3))) match_one_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node * match = 0;
  dependency *ne;
  int x, y;
  fsent * generated;

  run_match_tracking(rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    set_recycle(ctx->generate_nodes);
    pattern_generate(
        rule->generate
      , ctx->mod
      , ctx->mod->dir_node
      , ctx->mod->shadow_imports
      , ctx->variants
      , &ctx->invalidation
      , match
      , ctx->generate_nodes
    );

    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(generated = set_table_get(ctx->generate_nodes, y)))
        continue;

      dependency_connect(generated, match->node, rule->relation, &ctx->invalidation, &ne);
      dependency_setup(ne, fml_node, ctx);
      //if(ctx->err->l) {
      //  goto XAPI_FINALIZE;
      //}
    }
  }
}

/*
 * connector : match <- generate
 */
static void __attribute__((nonnull(1, 3))) generate_one_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node * match = 0;
  dependency *ne;
  int x, y;
  fsent * generated;

  run_match_tracking(rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(generated = set_table_get(ctx->generate_nodes, y)))
        continue;

      dependency_connect(match->node, generated, rule->relation, &ctx->invalidation, &ne);
      dependency_setup(ne, fml_node, ctx);
      //if(ctx->err->l) {
      //  goto XAPI_FINALIZE;
      //}
    }
  }
}

/*
 * connector : match -> [*] generate
 */
static void __attribute__((nonnull)) match_one_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node * match = 0;
  dependency *ne;
  int x, y;
  size_t Bsz;
  fsent *n = 0;

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);

  run_match_tracking(rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[0] = &match->node->vertex;

    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, y)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    dependency_hyperconnect(ctx->Blist, Bsz, ctx->Alist, 1, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
    //if(ctx->err->l) {
    //  break;
    //}
  }
}

/*
 * connector : match [*] <- generate
 */
static void __attribute__((nonnull)) generate_one_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node * match = 0;
  dependency *ne;
  int x, y;
  size_t Bsz;
  fsent *n = 0;

  assure(&ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);

  run_match_tracking(rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[0] = &match->node->vertex;

    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, y)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    dependency_hyperconnect(ctx->Alist, 1, ctx->Blist, Bsz, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
    //if(ctx->err->l) {
    //  break;
    //}
  }
}

/*
 * connector : match [*] -> generate
 */
static void __attribute__((nonnull(1, 3))) match_many_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  size_t Asz;
  dependency *ne;
  int x, y, z;
  fsent *n = 0;

  run_match_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);
  assure(&ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  x = 0;
  for(y = 1; y <= ctx->match_nodes->size; y++)
  {
    if(y < ctx->match_nodes->size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, ctx->Mlist[x], ctx->generate_nodes);

    for(z = 0; z < ctx->generate_nodes->table_size; z++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, z)))
        continue;

      ctx->Blist[0] = &n->vertex;
      dependency_hyperconnect(ctx->Blist, 1, ctx->Alist, Asz, rule->relation, &ctx->invalidation, &ne);
      dependency_setup(ne, fml_node, ctx);
      //if(ctx->err->l) {
      //  goto XAPI_FINALIZE;
      //}
    }

    x = y;
  }
}

/*
 * connector : match <- [*] generate
 */
static void __attribute__((nonnull)) generate_many_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  pattern_search_node * match = 0;
  dependency *ne;
  int x, y;
  size_t Bsz;
  fsent *n = 0;

  run_match_tracking(rule, ctx);

  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[0] = &match->node->vertex;
    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    Bsz = 0;
    assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, y)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    dependency_hyperconnect(ctx->Alist, 1, ctx->Blist, Bsz, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
    //if(ctx->err->l) {
    //  break;
    //}
  }
}

/*
 * connector : match [*] -> [*] generate
 */
static void __attribute__((nonnull)) match_many_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  size_t Asz;
  size_t Bsz;
  dependency *ne;
  int x;
  int y;
  int z;
  fsent *n = 0;

  /* run the match pattern */
  run_match_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);

  x = 0;
  for(y = 1; y <= ctx->match_nodes->size; y++)
  {
    if(y < ctx->match_nodes->size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, ctx->Mlist[x], ctx->generate_nodes);

    assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(z = 0; z < ctx->generate_nodes->table_size; z++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, z)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    dependency_hyperconnect(ctx->Blist, Bsz, ctx->Alist, Asz, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
    //if(ctx->err->l) {
    //  break;
    //}

    x = y;
  }
}

/*
 * connector : match [*] <- [*] generate
 */
static void __attribute__((nonnull)) generate_many_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  size_t Asz;
  size_t Bsz;
  dependency *ne;
  int x;
  int y;
  int z;
  fsent *n;

  /* run the match pattern */
  run_match_tracking(rule, ctx);
  matches_sorted(ctx);
  assure(&ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);

  x = 0;
  for(y = 1; y <= ctx->match_nodes->size; y++)
  {
    if(y < ctx->match_nodes->size && pattern_search_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    set_recycle(ctx->generate_nodes);
    pattern_generate(rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, ctx->Mlist[x], ctx->generate_nodes);

    assure(&ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(z = 0; z < ctx->generate_nodes->table_size; z++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, z)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    dependency_hyperconnect(ctx->Alist, Asz, ctx->Blist, Bsz, rule->relation, &ctx->invalidation, &ne);
    dependency_setup(ne, fml_node, ctx);
    //if(ctx->err->l) {
    //  break;
    //}

    x = y;
  }
}

static void rule_dirnode_edge_release(rule_dirnode_edge * restrict rde)
{
  llist_delete_node(&rde->edge.owner);
  llist_append(&rde_freelist, rde, edge.owner);
}

//
// public
//

void rule_release(rule * restrict r)
{
  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
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
  rule_dirnode_edge *rde;

  llist_foreach_safe(&rule_list, r, vertex.owner, T) {
    rule_dispose(r);
  }

  llist_foreach_safe(&rule_freelist, r, vertex.owner, T) {
    wfree(r);
  }

  llist_foreach_safe(&rde_list, rde, edge.owner, T) {
    rule_dirnode_edge_release(rde);
  }

  llist_foreach_safe(&rde_freelist, rde, edge.owner, T) {
    wfree(rde);
  }
}

void rule_mk(
    rule ** restrict rulep
  , moria_graph * restrict g
  , pattern * match
  , pattern * generate
  , pattern * formula
  , edge_type relation
  , uint32_t attrs
)
{
  rule * r;

  if((r = llist_shift(&rule_freelist, typeof(*r), vertex.owner)) == 0)
  {
    xmalloc(&r, sizeof(*r));
  }

  moria_vertex_init(&r->vertex, g, VERTEX_RULE);
  llist_append(&rule_list, r, vertex.owner);

  r->match = match;
  r->generate = generate;
  r->formula = formula;
  r->dir = attrs & RULE_DIRECTION_OPT;
  r->card = attrs & RULE_CARDINALITY_OPT;
  r->relation = relation;

  *rulep = r;
}

void rule_run(rule * restrict rule, rule_run_context * restrict ctx)
{
  fsent *fml_node = 0;

  RUNTIME_ASSERT(rule->relation);
  STATS_INC(g_stats.rule_run);

  if(rule->formula)
  {
    fml_node_get(rule, ctx, &fml_node);
    if(system_error) {
      return;
    }
  }

  switch(rule->card | rule->dir)
  {
    case RULE_ZERO_TO_ONE | RULE_LTR:
      generate_zero_to_one(rule, fml_node, ctx);
      break;
    case RULE_ZERO_TO_MANY | RULE_LTR:
      generate_zero_to_many(rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_ONE | RULE_LTR:
      match_one_to_one(rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_MANY | RULE_LTR:
      match_one_to_many(rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_ONE | RULE_LTR:
      match_many_to_one(rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_MANY | RULE_LTR:
      match_many_to_many(rule, fml_node, ctx);
      break;
    case RULE_ZERO_TO_ONE | RULE_RTL:
      match_zero_to_one(rule, fml_node, ctx);
      break;
    case RULE_ZERO_TO_MANY | RULE_RTL:
      match_zero_to_many(rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_ONE | RULE_RTL:
      generate_one_to_one(rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_MANY | RULE_RTL:
      generate_one_to_many(rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_ONE | RULE_RTL:
      generate_many_to_one(rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_MANY | RULE_RTL:
      generate_many_to_many(rule, fml_node, ctx);
      break;
    default:
      RUNTIME_ABORT();
  }
}

void rule_say(const rule * restrict r, narrator * restrict N)
{
  xsays("rule ");

  if((r->relation & EDGE_TYPE_OPT) != EDGE_TYPE_DEPENDS)
  {
    xsayf(" { relation : %s } ", attrs32_name_byvalue(graph_edge_type_attrs, r->relation));
  }

  if(r->match)
  {
    pattern_say(r->match, N);
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
    narrator_xsays(N, " : ");
    pattern_say(r->formula, N);
  }
}

void rule_run_context_xinit(rule_run_context * restrict rule_ctx)
{
  memset(rule_ctx, 0, sizeof(*rule_ctx));

  pattern_search_matches_create(&rule_ctx->match_nodes);
  set_create(&rule_ctx->generate_nodes);

  rule_ctx->modules = &module_list;
}

void rule_run_context_xdestroy(rule_run_context * restrict rule_ctx)
{
  graph_invalidation_end(&rule_ctx->invalidation);
  set_xfree(rule_ctx->match_nodes);
  set_xfree(rule_ctx->generate_nodes);
  wfree(rule_ctx->Alist);
  wfree(rule_ctx->Blist);
  wfree(rule_ctx->Mlist);
}

#if 0
static void log_rule_edge(rule * restrict rule, dependency * restrict ne)
{
  narrator *N = 0;

  log_start(L_RULE, &N);
  rule_say(rule, N);
  narrator_xsays(N, " ");
  graph_edge_say(&ne->edge, N);
  log_finish();
}
#endif
