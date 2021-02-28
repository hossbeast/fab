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

#define BOATS 1

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "moria/edge.h"
#include "narrator.h"

#include "rule_system.h"
#include "rule_module.h"
#include "fsent.h"
#include "rule.h"
#include "module.h"
#include "node_operations.h"
#include "logging.h"
#include "stats.h"
#include "dependency.h"

/* refresh state */
uint32_t rule_system_reconciliation_id;
//static uint32_t rule_system_reconcile_round_id;
static llist rma_list[2];
static int rma_list_index;

/* visit rules attached to a directory, part of run-invalid-rules
 *
 * v - rule node
 */
static xapi rule_run_rma(rule_module_edge * restrict rma, rule_run_context * restrict ctx)
{
  enter;

//  if((ctx->variants = rma->variants) == 0) {
//    ctx->variants = rma->mod->unscoped_block->variants;
//  }
  ctx->variants = rma->variants;
  ctx->mod = rma->mod;
  ctx->mod_owner = rma->mod_owner;
  ctx->rme = rma;

  fatal(rule_run, rma->rule, ctx);

  finally : coda;
}

static int nohits_rbn_cmp(const rbnode * restrict a, const rbnode * restrict b)
{
  return INTCMP(a, b);
}

//
// public
//

xapi rule_system_setup()
{
  enter;

  rule_system_reconciliation_id = 1;
//  rule_system_reconcile_round_id = 1;
  llist_init_node(&rma_list[0]);
  llist_init_node(&rma_list[1]);

  finally : coda;
}

xapi rule_system_reconcile(rule_run_context * restrict ctx, bool * restrict reconciled)
{
  enter;

  rule_module_edge *rma;
  llist *head;
  dependency *ne;
  llist *T;
  narrator * N;
  rbtree nohits;      // rmas with no matches during the reconciliation
  graph_invalidation_context invalidation = { };

  /* list of rmas run during the refresh operation */
//  llist rma_ranlist;

  rbtree_init(&nohits);
  ctx->nohits = &nohits;
  ctx->reconciled = reconciled;

//printf("START\n");

  /* run rules to quiescence */
  head = &rma_list[rma_list_index];
  while(!llist_empty(head) && *reconciled)
  {
    rule_system_reconciliation_id++;
//    llist_init_node(&rma_ranlist);
//printf("RECONCILE ID %d index %d - rules %zu\n", rule_system_reconciliation_id, rma_list_index, llist_count(head));
//
//if(rule_system_reconciliation_id > 5) {
//extern void exit(int);
//  exit(0);
//}

    graph_invalidation_end(&invalidation);
    fatal(graph_invalidation_begin, &invalidation);

    /* flip the rma list so that invalidations queue up in the other slot */
    rma_list_index = !rma_list_index;

    /* run all queued up rules - this can drive rule invalidations */
    llist_foreach(head, rma, changed[!rma_list_index].lln) { // , T) {
      if(*reconciled) {
        fatal(rule_run_rma, rma, ctx);

        if(!*reconciled) {
          fprintf(stderr, "rules were not run (unresolved formula references)\n");
        }
      }
      if(!*reconciled) {
        /* run this rma again next time */
        rule_system_rma_enqueue(rma);
        continue;
      }

      /* blow away edges previously instantiated by this rma but which were not refreshed */
      /* this can also drive rule invalidations */
      llist_foreach_safe(&rma->dependencies, ne, dependencies_lln, T) {
        if(ne->reconciliation_id != rule_system_reconciliation_id) {
          RUNTIME_ASSERT(ne->edge.attrs & EDGE_DEPENDENCY);
          fatal(graph_disintegrate, &ne->edge, &invalidation);
          llist_delete(ne, dependencies_lln);
        }
      }
    }

    llist_init_node(head);

//printf("REAPING for ID %d checking %zu rmas\n", rule_system_reconciliation_id, llist_count(&rma_ranlist));
//extern llist dependency_list;
//extern llist dependency_freelist;
//printf(" active dependencies %zu free dependencies %zu\n", llist_count(&dependency_list), llist_count(&dependency_freelist));

    head = &rma_list[rma_list_index];
  }

//printf("STOP - reconciled %d\n", *reconciled);

  /* warn about rules which have no effect */
  if(*reconciled && log_would(L_WARN))
  {
    rbtree_foreach(&nohits, rma, nohits_rbn) {
      fatal(log_start, L_WARN, &N);
      xsays("0 matches : ");
      fatal(rule_say, rma->rule, N);
      xsays(" @ module ");
      fatal(fsent_absolute_path_say, rma->mod->dir_node, N);
      fatal(log_finish);
    }
  }

finally:
  graph_invalidation_end(&invalidation);
coda;
}

void rule_system_rma_hit(rule_run_context * restrict ctx, rule_module_edge * restrict rma)
{
  if(!rbnode_attached(&rma->nohits_rbn)) {
//printf("rme %p hit (skip)\n", rma);
    return;
  }

//printf("rme %p hit (del)\n", rma);
  rbtree_delete(ctx->nohits, rma, nohits_rbn);
}

void rule_system_rma_nohit(rule_run_context * restrict ctx, rule_module_edge * restrict rma)
{
  if(rbnode_attached(&rma->nohits_rbn)) {
//printf("rme %p nohit (skip)\n", rma);
    return;
  }

//printf("rme %p nohit (put)\n", rma);
  rbtree_put(ctx->nohits, rma, nohits_rbn, nohits_rbn_cmp);
}

void rule_system_rma_enqueue(rule_module_edge * restrict rma)
{
  if(rma->changed[rma_list_index].reconciliation_id == rule_system_reconciliation_id) {
    return;
  }

//printf("rma enqueue %p index %d\n", rma, rma_list_index);

  llist_append(&rma_list[rma_list_index], rma, changed[rma_list_index].lln);
  rma->changed[rma_list_index].reconciliation_id = rule_system_reconciliation_id;
}
