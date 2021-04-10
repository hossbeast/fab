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

#include "xapi.h"
#include "types.h"

#include "common/assure.h"
#include "common/attrs.h"
#include "moria/edge.h"
#include "narrator.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "rule.h"
#include "dependency.h"
#include "formula.h"
#include "fsent.h"
#include "generate.h"
#include "logging.h"
#include "match.internal.h"
#include "module.h"
#include "rule_module.h"
#include "rule_system.h"
#include "stats.h"

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

static xapi rde_alloc(rule_dirnode_edge ** rdep)
{
  enter;

  rule_dirnode_edge *rde;

  if((rde = llist_shift(&rde_freelist, typeof(*rde), edge.owner)) == 0)
  {
    fatal(xmalloc, &rde, sizeof(*rde));
  }

  moria_edge_init(&rde->edge, &g_graph);
  llist_append(&rde_list, rde, edge.owner);

  *rdep = rde;

  finally : coda;
}

static xapi connector_say(const rule * restrict r, narrator * restrict N)
{
  enter;

  if(r->card == RULE_MANY_TO_ONE || r->card == RULE_MANY_TO_MANY)
    xsays("[*] ");

  if(r->dir == RULE_LTR)
    xsays("->");
  else if(r->dir == RULE_RTL)
    xsays("<-");

  if(r->card == RULE_ONE_TO_MANY || r->card == RULE_MANY_TO_MANY)
    xsays(" [*]");

  finally : coda;
}

struct rule_dirnode_connect_context {
  rule *r;
  module *mod;
  rule_module_edge *rme;
};

/* connect rule -> directory fsent */
static xapi rule_dirnode_connect(void * restrict _ctx, fsent * restrict node)
{
  enter;

  int r;
  moria_connect_context connect_ctx;
  struct rule_dirnode_connect_context *ctx = _ctx;
  rule_dirnode_edge *rde = 0;

  r = moria_preconnect(&connect_ctx, &g_graph, &ctx->r->vertex, &node->vertex, EDGE_RULE_DIR, 0);
  if (r == MORIA_HASEDGE) {
    goto XAPI_FINALLY;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  fatal(rde_alloc, &rde);
  fatal(graph_connect, &connect_ctx, &ctx->r->vertex, &node->vertex, &rde->edge, EDGE_RULE_DIR);

  rde->rme = ctx->rme;

  finally : coda;
}

static void rule_dispose(rule * restrict r)
{
  pattern_free(r->match);
  pattern_free(r->generate);
  pattern_free(r->formula);

  llist_delete_node(&r->vertex.owner);
  llist_append(&rule_freelist, r, vertex.owner);
}

static xapi log_rule_edge(rule * restrict rule, dependency * restrict ne)
{
  enter;

  narrator *N = 0;

  fatal(log_start, L_RULE, &N);
  fatal(rule_say, rule, N);
  fatal(narrator_xsays, N, " ");
  fatal(graph_edge_say, &ne->edge, N);
  fatal(log_finish);

  finally : coda;
}

static xapi __attribute__((nonnull)) fml_node_get(
    rule * restrict rule
  , rule_run_context * restrict ctx
  , fsent ** restrict fml_node
)
{
  enter;

  rule_module_edge * rma;
  int x;
  moria_connect_context connect_ctx;
  int __attribute__((unused)) r;
  moria_edge *e;
  formula *fml = 0;

  rma = ctx->rme;

  fatal(pattern_generate, rule->formula, ctx->mod, rma->mod_owner->dir_node, ctx->mod->shadow_uses, 0, &ctx->invalidation, 0, ctx->generate_nodes);

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
    fprintf(stderr, "[MODULE:NOREF] unresolved reference ");
    fprintf(stderr, "pattern ");
    fatal(pattern_say, rule->formula, g_narrator_stderr);
    fprintf(stderr, " @ %s [%d,%d - %d,%d]\n"
      , rma->mod_owner->self_node_relpath
      , rule->formula->loc.f_lin
      , rule->formula->loc.f_col
      , rule->formula->loc.l_lin
      , rule->formula->loc.l_col
    );
    *ctx->reconciled = false;
    goto XAPI_FINALIZE;
  }

  fatal(fsent_formula_bootstrap, *fml_node, ctx->reconciled);
  fml = (*fml_node)->self_fml;

  /* connect the rule vertex and the formula vertex */
  r = moria_preconnect(&connect_ctx, &g_graph, &rule->vertex, &fml->vertex, EDGE_RULE_FML, 0);
  if(r == MORIA_NOEDGE)
  {
    fatal(graph_edge_alloc, &e);
    fatal(graph_connect, &connect_ctx, &rule->vertex, &fml->vertex, e, EDGE_RULE_FML);
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_HASEDGE);
  }

  finally : coda;
}

static xapi run_match_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  enter;

  struct rule_dirnode_connect_context rdctx;

  rdctx = (typeof(rdctx)){
      .r = rule
    , .mod = ctx->mod
    , .rme = ctx->rme
  };

  fatal(pattern_match, rule->match, ctx->mod, ctx->modules, ctx->variants, ctx->match_nodes, rule_dirnode_connect, &rdctx);

  if(log_would(L_WARN))
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

  finally : coda;
}

static xapi run_generate_tracking(rule * restrict rule, rule_run_context * restrict ctx)
{
  enter;

  fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, 0, ctx->generate_nodes);

  if(log_would(L_WARN))
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

  finally : coda;
}

static int pattern_match_compare(const void *a, const void *b)
{
  const struct pattern_match_node *A = *(typeof(A)*)a;
  const struct pattern_match_node *B = *(typeof(B)*)b;

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
static xapi matches_sorted(rule_run_context * restrict ctx)
{
  enter;

  int y;
  size_t Msz;
  pattern_match_node *match;

  fatal(assure, &ctx->Mlist, sizeof(*ctx->Mlist), ctx->match_nodes->size, &ctx->Mlist_alloc);

  Msz = 0;
  for(y = 0; y < ctx->match_nodes->table_size; y++)
  {
    if(!(match = set_table_get(ctx->match_nodes, y)))
      continue;

    ctx->Mlist[Msz++] = match;
  }

  /* sorted by sets of matching capture groups */
  qsort(ctx->Mlist, Msz, sizeof(*ctx->Mlist), pattern_match_compare);

  finally : coda;
}

/*
 * Attach a newly-created dependency edge to a rule-module edge
 *
 * dep - dependency edge
 * fml - formula node if any
 */
static xapi __attribute__((nonnull(1, 3))) dependency_setup(
    dependency * restrict dep
  , fsent * restrict fml
  , rule_run_context * restrict ctx
)
{
  enter;

  moria_edge *e;
  fsent *n;
  int x;
  rule_module_edge * restrict rme = ctx->rme;

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

    fprintf(stderr, "[MODULE:MANYREF] ");
    fatal(fsent_project_relative_path_say, n, g_narrator_stderr);
    fprintf(stderr, " already created by rule (setup)\n ");
    fatal(rule_say, dep->rme->rule, g_narrator_stderr);
    fprintf(stderr, " @ %s:%d\n", dep->rme->mod_owner->self_node_relpath, dep->rme->rule->formula->loc.f_lin + 1);
    fprintf(stderr, "  dep %p rme %p rule %p\n ", dep, dep->rme, dep->rme->rule);
    fatal(rule_say, rme->rule, g_narrator_stderr);
    fprintf(stderr, " @ %s:%d\n", rme->mod_owner->self_node_relpath, rme->rule->formula->loc.f_lin + 1);
    fprintf(stderr, "  dep %p rme %p rule %p\n ", dep, rme, rme->rule);
    *ctx->reconciled = false;
    goto XAPI_FINALIZE;
  }

  /* mark this edge as having been touched during the rules reconciliation */
  dep->reconciliation_id = rule_system_reconciliation_id;

  /* dependency already associated to this rme */
  if(dep->rme && dep->rme == rme) {
    goto XAPI_FINALIZE;
  }

  RUNTIME_ASSERT(!llist_attached(dep, dependencies_lln));
  llist_append(&rme->dependencies, dep, dependencies_lln);

  dep->rme = rme;
  dep->fml = fml;

  if(!dep->fml) {
    fatal(log_rule_edge, rme->rule, dep);
    goto XAPI_FINALIZE;
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
      fatal(fsent_invalidate, n, &ctx->invalidation);
    }

    if(!(e->attrs & MORIA_EDGE_HYPER)) {
      break;
    }
  }

  fatal(log_rule_edge, rme->rule, dep);

  finally : coda;
}

/*
 * connector : match --
 */
static xapi __attribute__((nonnull)) match_zero_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node *match;
  dependency *ne;
  int x;
  fsent *n;
  moria_vertex *v;

  /* run the match pattern */
  fatal(run_match_tracking, rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    n = match->node;
    v = &n->vertex;

    fatal(dependency_hyperconnect, &v, 1, 0, 0, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }
  }

  finally : coda;
}

/*
 * connector : -- generate
 */
static xapi __attribute__((nonnull)) generate_zero_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  dependency *ne;
  int x;
  moria_vertex *v;
  fsent *n;

  fatal(run_generate_tracking, rule, ctx);
  for(x = 0; x < ctx->generate_nodes->table_size; x++)
  {
    if(!(n = set_table_get(ctx->generate_nodes, x)))
      continue;

    v = &n->vertex;

    fatal(dependency_hyperconnect, &v, 1, 0, 0, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }
  }

  finally : coda;
}

/*
 * connector : match-pattern [*] --
 */
static xapi __attribute__((nonnull)) match_zero_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node * match = 0;
  dependency *ne;
  int x;
  size_t z;

  fatal(run_match_tracking, rule, ctx);

  z = 0;
  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);

  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[z++] = &match->node->vertex;
  }

  if(z)
  {
    fatal(dependency_hyperconnect, ctx->Alist, z, 0, 0, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
  }

  finally : coda;
}

/*
 * connector : -- [*] generate
 */
static xapi __attribute__((nonnull)) generate_zero_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  dependency *ne;
  int x;
  fsent *n;
  size_t z;

  fatal(run_generate_tracking, rule, ctx);

  z = 0;
  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), ctx->generate_nodes->size, &ctx->Alist_alloc);

  for(x = 0; x < ctx->generate_nodes->table_size; x++)
  {
    if(!(n = set_table_get(ctx->generate_nodes, x)))
      continue;

    ctx->Alist[z++] = &n->vertex;
  }

  fatal(dependency_hyperconnect, ctx->Alist, z, 0, 0, rule->relation, &ctx->invalidation, &ne);
  fatal(dependency_setup, ne, fml_node, ctx);

  finally : coda;
}

/*
 * connector : match -> generate
 */
static xapi __attribute__((nonnull(1, 3))) match_one_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node * match = 0;
  dependency *ne;
  int x, y;
  fsent * generated;

  fatal(run_match_tracking, rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    fatal(pattern_generate
      , rule->generate
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

      fatal(dependency_connect, generated, match->node, rule->relation, &ctx->invalidation, &ne);
      fatal(dependency_setup, ne, fml_node, ctx);
      if(!*ctx->reconciled) {
        goto XAPI_FINALIZE;
      }
    }
  }

  finally : coda;
}

/*
 * connector : match <- generate
 */
static xapi __attribute__((nonnull(1, 3))) generate_one_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node * match = 0;
  dependency *ne;
  int x, y;
  fsent * generated;

  fatal(run_match_tracking, rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(generated = set_table_get(ctx->generate_nodes, y)))
        continue;

      fatal(dependency_connect, match->node, generated, rule->relation, &ctx->invalidation, &ne);
      fatal(dependency_setup, ne, fml_node, ctx);
      if(!*ctx->reconciled) {
        goto XAPI_FINALIZE;
      }
    }
  }

  finally : coda;
}

/*
 * connector : match -> [*] generate
 */
static xapi __attribute__((nonnull)) match_one_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node * match = 0;
  dependency *ne;
  int x, y;
  size_t Bsz;
  fsent *n = 0;

  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);

  fatal(run_match_tracking, rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[0] = &match->node->vertex;

    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    fatal(assure, &ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, y)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    fatal(dependency_hyperconnect, ctx->Blist, Bsz, ctx->Alist, 1, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }
  }

  finally : coda;
}

/*
 * connector : match [*] <- generate
 */
static xapi __attribute__((nonnull)) generate_one_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node * match = 0;
  dependency *ne;
  int x, y;
  size_t Bsz;
  fsent *n = 0;

  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), 1, &ctx->Alist_alloc);

  fatal(run_match_tracking, rule, ctx);
  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[0] = &match->node->vertex;

    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    fatal(assure, &ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, y)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    fatal(dependency_hyperconnect, ctx->Alist, 1, ctx->Blist, Bsz, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }
  }

  finally : coda;
}

/*
 * connector : match [*] -> generate
 */
static xapi __attribute__((nonnull(1, 3))) match_many_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  size_t Asz;
  dependency *ne;
  int x, y, z;
  fsent *n = 0;

  fatal(run_match_tracking, rule, ctx);
  fatal(matches_sorted, ctx);
  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);
  fatal(assure, &ctx->Blist, sizeof(*ctx->Blist), 1, &ctx->Blist_alloc);

  x = 0;
  for(y = 1; y <= ctx->match_nodes->size; y++)
  {
    if(y < ctx->match_nodes->size && pattern_match_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, ctx->Mlist[x], ctx->generate_nodes);

    for(z = 0; z < ctx->generate_nodes->table_size; z++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, z)))
        continue;

      ctx->Blist[0] = &n->vertex;
      fatal(dependency_hyperconnect, ctx->Blist, 1, ctx->Alist, Asz, rule->relation, &ctx->invalidation, &ne);
      fatal(dependency_setup, ne, fml_node, ctx);
      if(!*ctx->reconciled) {
        goto XAPI_FINALIZE;
      }
    }

    x = y;
  }

  finally : coda;
}

/*
 * connector : match <- [*] generate
 */
static xapi __attribute__((nonnull)) generate_many_to_one(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  pattern_match_node * match = 0;
  dependency *ne;
  int x, y;
  size_t Bsz;
  fsent *n = 0;

  fatal(run_match_tracking, rule, ctx);

  for(x = 0; x < ctx->match_nodes->table_size; x++)
  {
    if(!(match = set_table_get(ctx->match_nodes, x)))
      continue;

    ctx->Alist[0] = &match->node->vertex;
    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

    Bsz = 0;
    fatal(assure, &ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    for(y = 0; y < ctx->generate_nodes->table_size; y++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, y)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    fatal(dependency_hyperconnect, ctx->Alist, 1, ctx->Blist, Bsz, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }
  }

  finally : coda;
}

/*
 * connector : match [*] -> [*] generate
 */
static xapi __attribute__((nonnull)) match_many_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  size_t Asz;
  size_t Bsz;
  dependency *ne;
  int x;
  int y;
  int z;
  fsent *n = 0;

  /* run the match pattern */
  fatal(run_match_tracking, rule, ctx);
  fatal(matches_sorted, ctx);
  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);

  x = 0;
  for(y = 1; y <= ctx->match_nodes->size; y++)
  {
    if(y < ctx->match_nodes->size && pattern_match_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, ctx->Mlist[x], ctx->generate_nodes);

    fatal(assure, &ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(z = 0; z < ctx->generate_nodes->table_size; z++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, z)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    fatal(dependency_hyperconnect, ctx->Blist, Bsz, ctx->Alist, Asz, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }

    x = y;
  }

  finally : coda;
}

/*
 * connector : match [*] <- [*] generate
 */
static xapi __attribute__((nonnull)) generate_many_to_many(
    rule * restrict rule
  , fsent * restrict fml_node
  , rule_run_context * restrict ctx
)
{
  enter;

  size_t Asz;
  size_t Bsz;
  dependency *ne;
  int x;
  int y;
  int z;
  fsent *n;

  /* run the match pattern */
  fatal(run_match_tracking, rule, ctx);
  fatal(matches_sorted, ctx);
  fatal(assure, &ctx->Alist, sizeof(*ctx->Alist), ctx->match_nodes->size, &ctx->Alist_alloc);

  x = 0;
  for(y = 1; y <= ctx->match_nodes->size; y++)
  {
    if(y < ctx->match_nodes->size && pattern_match_compare(&ctx->Mlist[x], &ctx->Mlist[y]) == 0) {
      continue;
    }

    for(Asz = 0; Asz < (y - x); Asz++) {
      ctx->Alist[Asz] = &ctx->Mlist[x + Asz]->node->vertex;
    }

    /* run the generate pattern in context of a match node in the group */
    fatal(pattern_generate, rule->generate, ctx->mod, ctx->mod->dir_node, ctx->mod->shadow_imports, ctx->variants, &ctx->invalidation, ctx->Mlist[x], ctx->generate_nodes);

    fatal(assure, &ctx->Blist, sizeof(*ctx->Blist), ctx->generate_nodes->size, &ctx->Blist_alloc);
    Bsz = 0;
    for(z = 0; z < ctx->generate_nodes->table_size; z++)
    {
      if(!(n = set_table_get(ctx->generate_nodes, z)))
        continue;

      ctx->Blist[Bsz++] = &n->vertex;
    }

    fatal(dependency_hyperconnect, ctx->Alist, Asz, ctx->Blist, Bsz, rule->relation, &ctx->invalidation, &ne);
    fatal(dependency_setup, ne, fml_node, ctx);
    if(!*ctx->reconciled) {
      break;
    }

    x = y;
  }

  finally : coda;
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

xapi rule_mk(
    rule ** restrict rulep
  , moria_graph * restrict g
  , pattern * match
  , pattern * generate
  , pattern * formula
  , edge_type relation
  , uint32_t attrs
)
{
  enter;

  rule * r;

  if((r = llist_shift(&rule_freelist, typeof(*r), vertex.owner)) == 0)
  {
    fatal(xmalloc, &r, sizeof(*r));
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

  finally : coda;
}

xapi rule_run(rule * restrict rule, rule_run_context * restrict ctx)
{
  enter;

  fsent *fml_node = 0;

  RUNTIME_ASSERT(rule->relation);
  STATS_INC(g_stats.rule_run);

  if(rule->formula)
  {
    fatal(fml_node_get, rule, ctx, &fml_node);
    if(!*ctx->reconciled) {
      goto XAPI_FINALIZE;
    }
  }

  switch(rule->card | rule->dir)
  {
    case RULE_ZERO_TO_ONE | RULE_LTR:
      fatal(generate_zero_to_one, rule, fml_node, ctx);
      break;
    case RULE_ZERO_TO_MANY | RULE_LTR:
      fatal(generate_zero_to_many, rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_ONE | RULE_LTR:
      fatal(match_one_to_one, rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_MANY | RULE_LTR:
      fatal(match_one_to_many, rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_ONE | RULE_LTR:
      fatal(match_many_to_one, rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_MANY | RULE_LTR:
      fatal(match_many_to_many, rule, fml_node, ctx);
      break;
    case RULE_ZERO_TO_ONE | RULE_RTL:
      fatal(match_zero_to_one, rule, fml_node, ctx);
      break;
    case RULE_ZERO_TO_MANY | RULE_RTL:
      fatal(match_zero_to_many, rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_ONE | RULE_RTL:
      fatal(generate_one_to_one, rule, fml_node, ctx);
      break;
    case RULE_ONE_TO_MANY | RULE_RTL:
      fatal(generate_one_to_many, rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_ONE | RULE_RTL:
      fatal(generate_many_to_one, rule, fml_node, ctx);
      break;
    case RULE_MANY_TO_MANY | RULE_RTL:
      fatal(generate_many_to_many, rule, fml_node, ctx);
      break;
    default:
      RUNTIME_ABORT();
  }

  finally : coda;
}

xapi rule_say(const rule * restrict r, narrator * restrict N)
{
  enter;

  xsays("rule ");

  if((r->relation & EDGE_TYPE_OPT) != EDGE_TYPE_DEPENDS)
  {
    xsayf(" { relation : %s } ", attrs32_name_byvalue(graph_edge_type_attrs, r->relation));
  }

  if(r->match)
  {
    fatal(pattern_say, r->match, N);
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

  fatal(connector_say, r, N);

  if(r->generate)
  {
    xsays(" ");
    fatal(pattern_say, r->generate, N);
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
    fatal(narrator_xsays, N, " : ");
    fatal(pattern_say, r->formula, N);
  }

  finally : coda;
}

xapi rule_run_context_xinit(rule_run_context * restrict rule_ctx)
{
  enter;

  memset(rule_ctx, 0, sizeof(*rule_ctx));

  fatal(pattern_match_matches_create, &rule_ctx->match_nodes);
  fatal(set_create, &rule_ctx->generate_nodes);

  rule_ctx->modules = &module_list;

  finally : coda;
}

xapi rule_run_context_xdestroy(rule_run_context * restrict rule_ctx)
{
  enter;

  graph_invalidation_end(&rule_ctx->invalidation);
  fatal(set_xfree, rule_ctx->match_nodes);
  fatal(set_xfree, rule_ctx->generate_nodes);
  wfree(rule_ctx->Alist);
  wfree(rule_ctx->Blist);
  wfree(rule_ctx->Mlist);

  finally : coda;
}
