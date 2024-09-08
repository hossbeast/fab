/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "types.h"

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "bacon/builder.h"
#include "moria/foreach.h"

#include "rule_system.h"
#include "dependency.h"
#include "fsent.h"
#include "module.h"
#include "rule.h"
#include "rule_module_association.h"
#include "buildplan_entity.h"
#include "channel.h"
#include "edge.h"
#include "vertex.h"
#include "system.h"
#include "events.h"
#include "zcat.h"
#include "meta_edge.h"
#include "entlist.h"

/* refresh state */
uint32_t rule_system_epoch;
static llist work_queue[2];
static int work_queue_index;

//
// public
//

void rule_system_setup()
{
  llist_init(&work_queue[0]);
  llist_init(&work_queue[1]);
  rule_system_epoch = 1;
}

void rule_system_reconcile(rule_run_context * restrict ctx)
{
  rule_system_work *work;
  rule_module_association *rma;
  llist *head;
  graph_invalidation_context invalidation = { };
  moria_foreach_state st;
  moria_edge *edge;
  meta_edge *meta;
  entlist edges, vertices;

  entlist_init(&vertices);
  entlist_init(&edges);

  /* run rules to quiescence */
  head = &work_queue[work_queue_index];
  while(!llist_empty(head) && !system_error)
  {
    rule_system_epoch++;
    graph_invalidation_end(&invalidation);
    graph_invalidation_begin(&invalidation);

    /* flip the rma edges so that invalidations queue up in the other slot */
    work_queue_index = !work_queue_index;
    llist_init(&work_queue[work_queue_index]);

    /* run all queued up rules - this can drive rule invalidations */
    while((work = llist_shift(head, typeof(*work), changed[!work_queue_index].lln))) {
      rma = containerof(work, typeof(*rma), work);
      vertex_mark_fresh(&rma->vertex);
      if(!system_error) {
        ctx->rma = rma;
        rule_run(rma->rule, ctx);
      }

      /* remove edges previously created by running this rma but which were not refreshed during this round */
      /* this can also drive rule invalidations */
      moria_foreach_edge(&st, &rma->vertex, edge) {
        switch(edge_type_get(edge)) {
          case EDGE_RMA_BPE:
          case EDGE_FSNAME_RMA:
          case EDGE_RMA_TGT:
            break;
          default:
            continue;
        }
        meta = containerof(edge, typeof(*meta), edge);
        if(meta->epoch == rule_system_epoch) {
          continue;
        }
        entlist_push(&edges, &meta->edge);
      }
      graph_disintegrate_bulk(&edges, &vertices, &invalidation);
      entlist_destroy(&edges);
      entlist_destroy(&vertices);

      if(system_error) {
        vertex_mark_stale(&rma->vertex);  // re-enqueue
        continue;
      }
    }

    head = &work_queue[work_queue_index];
  }

  graph_invalidation_end(&invalidation);
}

void rule_system_rma_hit(rule_run_context * restrict ctx, rule_module_association * restrict rma)
{
}

void rule_system_rma_nohit(rule_run_context * restrict ctx, rule_module_association * restrict rma)
{
}

void rule_system_work_enqueue(rule_system_work * restrict work)
{
  if(work->changed[work_queue_index].epoch == rule_system_epoch) {
    return;
  }

  llist_push(&work_queue[work_queue_index], &work->changed[work_queue_index].lln);
  work->changed[work_queue_index].epoch = rule_system_epoch;
}
