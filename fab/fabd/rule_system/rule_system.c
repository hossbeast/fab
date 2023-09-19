/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include <stdio.h>

#include "types.h"

#include "narrator.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"

#include "rule_system.h"
#include "dependency.h"
#include "fsent.h"
#include "module.h"
#include "rule.h"
#include "rule_module.h"
#include "channel.h"
#include "system_state.h"

/* refresh state */
uint32_t rule_system_reconciliation_id;
static llist rma_list[2];
static int rma_list_index;

static int nohits_rbn_cmp(const rbnode * restrict a, const rbnode * restrict b)
{
  return INTCMP(a, b);
}

//
// public
//

void rule_system_setup()
{
  rule_system_reconciliation_id = 1;
  llist_init_node(&rma_list[0]);
  llist_init_node(&rma_list[1]);
}

void rule_system_reconcile(rule_run_context * restrict ctx)
{
  rule_module_edge *rma;
  llist *head;
  dependency *ne;
  llist *T;
  rbtree nohits;      // rmas with no matches during the reconciliation
  graph_invalidation_context invalidation = { };

  rbtree_init(&nohits);
  ctx->nohits = &nohits;

  /* run rules to quiescence */
  head = &rma_list[rma_list_index];
  while(!llist_empty(head) && !system_error)
  {
    rule_system_reconciliation_id++;
    graph_invalidation_end(&invalidation);
    graph_invalidation_begin(&invalidation);

    /* flip the rma list so that invalidations queue up in the other slot */
    rma_list_index = !rma_list_index;

    /* run all queued up rules - this can drive rule invalidations */
    llist_foreach(head, rma, changed[!rma_list_index].lln) {
      if(!system_error) {
        ctx->variants = rma->variants;
        ctx->mod = rma->mod;
        ctx->mod_owner = rma->mod_owner;
        ctx->rme = rma;
        rule_run(rma->rule, ctx);
      }
      if(system_error) {
        rule_system_rma_enqueue(rma);
        continue;
      }

      /* blow away edges previously instantiated by this rma but which were not refreshed */
      /* this can also drive rule invalidations */
      llist_foreach_safe(&rma->dependencies, ne, dependencies_lln, T) {
        if(ne->reconciliation_id != rule_system_reconciliation_id) {
          RUNTIME_ASSERT(ne->edge.attrs & EDGE_DEPENDENCY);
          graph_disintegrate(&ne->edge, &invalidation);
          llist_delete(ne, dependencies_lln);
        }
      }
    }

    llist_init_node(head);

    head = &rma_list[rma_list_index];
  }

  /* warn about rules which had no effect */
  if(!system_error && 1)
  {
    rbtree_foreach(&nohits, rma, nohits_rbn) {
      printf("0 matches : ");
      rule_say(rma->rule, g_narrator_stdout);
      printf(" @ module ");
      fsent_absolute_path_say(rma->mod->dir_node, g_narrator_stdout);
      printf("\n");
    }
  }

  graph_invalidation_end(&invalidation);
}

void rule_system_rma_hit(rule_run_context * restrict ctx, rule_module_edge * restrict rma)
{
  if(!rbnode_attached(&rma->nohits_rbn)) {
    return;
  }

  rbtree_delete(ctx->nohits, rma, nohits_rbn);
}

void rule_system_rma_nohit(rule_run_context * restrict ctx, rule_module_edge * restrict rma)
{
  if(rbnode_attached(&rma->nohits_rbn)) {
    return;
  }

  rbtree_put(ctx->nohits, rma, nohits_rbn, nohits_rbn_cmp);
}

void rule_system_rma_enqueue(rule_module_edge * restrict rma)
{
  if(rma->changed[rma_list_index].reconciliation_id == rule_system_reconciliation_id) {
    return;
  }

  llist_append(&rma_list[rma_list_index], rma, changed[rma_list_index].lln);
  rma->changed[rma_list_index].reconciliation_id = rule_system_reconciliation_id;
}
