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

#include <string.h>

#include "xapi.h"
#include "types.h"
#include "narrator.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "xlinux/xstdlib.h"
#include "valyria/set.h"
#include "moria/vertex.h"
#include "moria/graph.h"
#include "moria/edge.h"
#include "moria/operations.h"
#include "narrator.h"

#include "rule.internal.h"
#include "node.h"
#include "node_operations.h"
#include "lookup.h"
#include "path.h"
#include "match.internal.h"
#include "generate.h"
#include "module.h"
#include "logging.h"
#include "variant.h"
#include "formula.h"
#include "MODULE.errtab.h"
#include "stats.h"

#include "common/attrs.h"

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
  rule_module_association *rma;
};

/* connect rule -> directory node */
static xapi rule_dirnode_connect(void * restrict _ctx, node * restrict node)
{
  enter;

  struct rule_dirnode_connect_context *ctx = _ctx;
  edge_type relation = EDGE_TYPE_RULE_DIR;
  bool created = false;
  rule_dirnode_edge *rde;
  edge *e = 0;

  fatal(graph_connect
    , g_graph
    , vertex_containerof(ctx->r)
    , vertex_containerof(node)
    , relation
    , &e
    , &created
  );

  if(!created)
    goto XAPI_FINALLY;

  rde = edge_value(e);
  rde->rma = ctx->rma;

  finally : coda;
}

//
// public
//

xapi rule_mk(
    rule ** restrict rulep
  , graph * restrict g
  , pattern * match
  , pattern * generate
  , node * formula
  , uint32_t attrs
)
{
  enter;

  rule * r;
  vertex *v;

  STATS_INC(rules);
  fatal(vertex_create, &v, g, VERTEX_RULE);
  r = vertex_value(v);

  r->match = match;
  r->generate = generate;
  r->fml_node = formula;
  r->dir = attrs & RULE_DIRECTION_OPT;
  r->card = attrs & RULE_CARDINALITY_OPT;
  r->relation = attrs & EDGE_TYPE_OPT;

  *rulep = r;

  finally : coda;
}

xapi rule_xdestroy(rule * restrict r)
{
  enter;

  pattern_free(r->match);
  pattern_free(r->generate);

  finally : coda;
}

static xapi rma_edge_associate(node_edge * restrict ne, rule_module_association * restrict rma)
{
  enter;

  narrator * N;

  if(llist_attached(ne, lln))
  {
    if(ne->rma != rma)
    {
      logs(L_ERROR, "edge info");

      fatal(log_start, L_ERROR, &N);
      fatal(narrator_xsayf, N, " edge : %p ", ne);
      fatal(graph_edge_say, edge_containerof(ne), N);
      fatal(log_finish);

      fatal(log_start, L_ERROR, &N);
      fatal(narrator_xsayf, N, " rule : %p ", ne->rma ? ne->rma->rule : 0);
      if(ne->rma) {
        fatal(rule_say, ne->rma->rule, N);
      }
      fatal(log_finish);

      fatal(log_start, L_ERROR, &N);
      fatal(narrator_xsayf, N, " rule : %p ", rma->rule);
      fatal(rule_say, rma->rule, N);
      fatal(log_finish);

      fail(MODULE_EDGEINUSE);
    }
  }
  else
  {
    llist_append(&rma->edges, ne, lln);
    ne->rma = rma;
  }

  /* mark this edge as having been touched during the refresh */
  ne->refresh_id = graph_refresh_id;

  finally : coda;
}

static int nohits_rbn_cmp(const rbnode * restrict a, const rbnode * restrict b)
{
  return INTCMP(a, b);
}

static xapi __attribute__((nonnull(1))) bpe_setup(buildplan_entity * restrict bpe, node * restrict fmlnode)
{
  enter;

  RUNTIME_ASSERT(!bpe->fml || bpe->fml == fmlnode);

  bpe->fml = fmlnode;

  finally : coda;
}

static xapi __attribute__((nonnull)) bpe_attach(node * restrict n, buildplan_entity * restrict bpe, graph_invalidation_context * restrict invalidation)
{
  enter;

  RUNTIME_ASSERT(!n->bpe || n->bpe == bpe);
  n->bpe = bpe;

  finally : coda;
}

xapi rule_run(rule * restrict rule, rule_run_context * restrict ctx)
{
  enter;

  int x, y, z;
  pattern_match_node * match;
  node * generated;
  node * above = 0;
  node * below = 0;
  narrator *N;
  node_edge * ne;
  node *n;
  vertex ** Alist = 0;
  vertex ** vlist = 0;
  vertex * Blist[1];
  rule_module_association * rma;
  struct rule_dirnode_connect_context rdctx;

  rma = ctx->rma;

  if(rule->match)
  {
    rdctx = (typeof(rdctx)){
        .r = rule
      , .mod = ctx->mod
      , .rma = rma
    };

    fatal(pattern_match, rule->match, ctx->mod, ctx->modules, ctx->variants, ctx->match_nodes, 0, rule_dirnode_connect, &rdctx);

    if(log_would(L_WARN))
    {
      if(ctx->match_nodes->size)
      {
        if(rbnode_attached(&rma->nohits_rbn))
        {
          rbtree_delete(&ctx->nohits, rma, nohits_rbn);
        }
      }
      else if(!rbnode_attached(&rma->nohits_rbn))
      {
        rbtree_put(&ctx->nohits, rma, nohits_rbn, nohits_rbn_cmp);
      }
    }
  }
  else if(rule->generate)
  {
    // generate-only
    fatal(pattern_generate, rule->generate, ctx->mod, ctx->variants, &ctx->invalidation, 0, ctx->generate_nodes);

    if(log_would(L_WARN))
    {
      if(ctx->generate_nodes->size)
      {
        if(rbnode_attached(&rma->nohits_rbn))
        {
          rbtree_delete(&ctx->nohits, rma, nohits_rbn);
        }
      }
      else if(!rbnode_attached(&rma->nohits_rbn))
      {
        rbtree_put(&ctx->nohits, rma, nohits_rbn, nohits_rbn_cmp);
      }
    }
  }

  if(log_would(L_RULE))
  {
    fatal(log_start, L_RULE, &N);
    fatal(rule_say, rule, N);
    xsays(" @ module ");
    fatal(node_absolute_path_say, ctx->mod->dir_node, N);
    fatal(log_finish);

    fatal(log_start, L_RULE, &N);
    xsayf(" variants %zu { ", ctx->variants->size);
    for(x = 0; x < ctx->variants->table_size; x++)
    {
      variant *var;
      if(!(var = set_table_get(ctx->variants, x)))
        continue;

      xsayf("%s ", var->norm);
    }
    xsays("}");
    if(rule->match)
      logf(L_RULE, " matches %zu", ctx->match_nodes->size);
    else
      logs(L_RULE, " matches -");
    if(rule->generate)
      logf(L_RULE, " generates %zu", ctx->generate_nodes->size);
    else
      logs(L_RULE, " generates -");
  }

  if(!rule->match && rule->generate)
  {
    if(rule->card == RULE_ZERO_TO_ONE)
    {
      for(x = 0; x < ctx->generate_nodes->table_size; x++)
      {
        if(!(generated = set_table_get(ctx->generate_nodes, x)))
          continue;

        fatal(bpe_setup, &generated->self_bpe, rule->fml_node);
        fatal(bpe_attach, generated, &generated->self_bpe, &ctx->invalidation);

        if(log_would(L_RULE))
        {
          fatal(log_start, L_RULE, &N);
          fatal(node_path_say, generated, N);
          if(rule->fml_node)
          {
            xsays(" : ");
            fatal(node_path_say, rule->fml_node, N);
          }
          fatal(log_finish);
        }
      }
    }
    else if(rule->card == RULE_ZERO_TO_MANY)
    {
      RUNTIME_ASSERT(rule->dir == 0);
      RUNTIME_ASSERT(rule->relation);

      fatal(xrealloc, &vlist, sizeof(*vlist), ctx->generate_nodes->size, 0);
      z = 0;
      for(y = 0; y < ctx->generate_nodes->table_size; y++)
      {
        if(!(generated = set_table_get(ctx->generate_nodes, y)))
          continue;

        vlist[z++] = vertex_containerof(generated);
      }

      if(log_would(L_RULE))
      {
        logs(L_RULE, "  {");
        for(y = 0; y < z; y++)
        {
          n = vertex_value(vlist[y]);
          fatal(log_start, L_RULE, &N);
          xsays("   ");
          fatal(node_path_say, n, N);
          if(rule->fml_node)
          {
            xsays(" : ");
            fatal(node_path_say, rule->fml_node, N);
          }
          xsayf(" var %s", n->var ? n->var->norm : "(none)");
          fatal(log_finish);
        }
        logs(L_RULE, "  }");
      }

      fatal(node_hyperconnect, 0, 0, vlist, z, rule->relation, &ctx->invalidation, &ne);
      fatal(rma_edge_associate, ne, rma);
      ne->dir = EDGE_SRC_TGT;
      fatal(bpe_setup, &ne->bpe, rule->fml_node);

      for(y = 0; y < z; y++) {
        n = vertex_value(vlist[y]);
        fatal(bpe_attach, n, &ne->bpe, &ctx->invalidation);
      }
    }
  }

  if(rule->match && !rule->generate)
  {
    for(x = 0; x < ctx->match_nodes->table_size; x++)
    {
      if(!(match = set_table_get(ctx->match_nodes, x)))
        continue;

      fatal(bpe_setup, &match->node->self_bpe, rule->fml_node);
      fatal(bpe_attach, match->node, &match->node->self_bpe, &ctx->invalidation);

      if(log_would(L_RULE))
      {
        fatal(log_start, L_RULE, &N);
        fatal(node_path_say, match->node, N);
        xsays(" : ");
        fatal(node_path_say, rule->fml_node, N);
        fatal(log_finish);
      }
    }
  }

  if(rule->match && rule->generate)
  {
    for(x = 0; x < ctx->match_nodes->table_size; x++)
    {
      if(!(match = set_table_get(ctx->match_nodes, x)))
        continue;

      fatal(set_recycle, ctx->generate_nodes);
      fatal(pattern_generate, rule->generate, ctx->mod, ctx->variants, &ctx->invalidation, match, ctx->generate_nodes);

      if(log_would(L_RULE))
      {
        fatal(log_start, L_RULE, &N);
        xsays("  ");
        fatal(node_path_say, match->node, N);
        xsays(" ");
        fatal(connector_say, rule, N);
        xsays(" {");
        fatal(log_finish);
      }

      if(rule->card == RULE_ONE_TO_ONE)
      {
        for(y = 0; y < ctx->generate_nodes->table_size; y++)
        {
          if(!(generated = set_table_get(ctx->generate_nodes, y)))
            continue;

          if(rule->dir == RULE_LTR)
          {
            above = generated;
            below = match->node;
          }
          else if(rule->dir == RULE_RTL)
          {
            above = match->node;
            below = generated;
          }
          else
          {
            RUNTIME_ABORT();
          }

          if(log_would(L_RULE))
          {
            fatal(log_start, L_RULE, &N);
            xsays("   ");
            fatal(node_path_say, generated, N);
            if(rule->fml_node)
            {
              xsays(" : ");
              fatal(node_path_say, rule->fml_node, N);
            }
            xsayf(" var %s", generated->var ? generated->var->norm : "(none)");
            fatal(log_finish);
          }

          RUNTIME_ASSERT(rule->relation);
          fatal(node_connect, above, below, rule->relation, &ctx->invalidation, &ne, 0);
          fatal(rma_edge_associate, ne, rma);

          if(rule->fml_node)
          {
            if(!generated->bpe)
            {
              fatal(bpe_setup, &ne->bpe, rule->fml_node);
              fatal(bpe_attach, generated, &ne->bpe, &ctx->invalidation);
            }

            if(rule->dir == RULE_LTR)
              ne->dir = EDGE_TGT_SRC;
            else if(rule->dir == RULE_RTL)
              ne->dir = EDGE_SRC_TGT;
          }
        }

        logs(L_RULE, "  }");
      }
      else if(rule->card == RULE_ONE_TO_MANY)
      {
        RUNTIME_ASSERT(rule->dir == RULE_LTR);
        RUNTIME_ASSERT(rule->fml_node);
        RUNTIME_ASSERT(rule->relation);

        fatal(xrealloc, &Alist, sizeof(*Alist), ctx->generate_nodes->size, 0);
        z = 0;
        for(y = 0; y < ctx->generate_nodes->table_size; y++)
        {
          if(!(generated = set_table_get(ctx->generate_nodes, y)))
            continue;

          Alist[z++] = vertex_containerof(generated);
        }

        Blist[0] = vertex_containerof(match->node);

        if(log_would(L_RULE))
        {
          for(y = 0; y < z; y++)
          {
            n = vertex_value(Alist[y]);
            fatal(log_start, L_RULE, &N);
            xsays("   ");
            fatal(node_path_say, n, N);
            xsayf(" var %s", n->var ? n->var->norm : "(none)");
            fatal(log_finish);
          }

          fatal(log_start, L_RULE, &N);
          xsays("  }");
          if(rule->fml_node)
          {
            xsays(" : ");
            fatal(node_path_say, rule->fml_node, N);
          }
          fatal(log_finish);
        }

        fatal(node_hyperconnect, Alist, z, Blist, 1, rule->relation, &ctx->invalidation, &ne);
        fatal(rma_edge_associate, ne, rma);

        if(rule->fml_node)
        {
          ne->dir = EDGE_TGT_SRC;
          fatal(bpe_setup, &ne->bpe, rule->fml_node);

          for(y = 0; y < z; y++) {
            n = vertex_value(Alist[y]);
            fatal(bpe_attach, n, &ne->bpe, &ctx->invalidation);
          }
        }
      }
      else
      {
        RUNTIME_ABORT();
      }
    }
  }

finally:
  wfree(Alist);
  wfree(vlist);
coda;
}

xapi rule_say(const rule * restrict r, narrator * restrict N)
{
  enter;

  xsays("rule ");

  if(r->relation != EDGE_TYPE_STRONG)
  {
    xsays(" { relation : conduit } ");
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

  if(r->fml_node)
  {
    fatal(narrator_xsays, N, " : ");
    fatal(node_project_relative_path_say, r->fml_node, N);
  }

  finally : coda;
}

xapi rule_run_context_xinit(rule_run_context * restrict rule_ctx)
{
  enter;

  memset(rule_ctx, 0, sizeof(*rule_ctx));

  fatal(pattern_match_matches_create, &rule_ctx->match_nodes);
  fatal(set_create, &rule_ctx->generate_nodes);
  rbtree_init(&rule_ctx->nohits);

  finally : coda;
}

xapi rule_run_context_begin(rule_run_context * restrict rule_ctx)
{
  enter;

  fatal(graph_invalidation_begin, &rule_ctx->invalidation);

  /* reset tracking for which rules have hit */
  rbtree_init(&rule_ctx->nohits);

  finally : coda;
}

void rule_run_context_end(rule_run_context * restrict rule_ctx)
{
  graph_invalidation_end(&rule_ctx->invalidation);
}

xapi rule_run_context_xdestroy(rule_run_context * restrict rule_ctx)
{
  enter;

  graph_invalidation_end(&rule_ctx->invalidation);
  fatal(set_xfree, rule_ctx->match_nodes);
  fatal(set_xfree, rule_ctx->generate_nodes);

  finally : coda;
}
