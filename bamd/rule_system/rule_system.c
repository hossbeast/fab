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
#include "xlinux/xtime.h"

#include "rule_system.h"
#include "dependency.h"
#include "fsent.h"
#include "module.h"
#include "rule.h"
#include "rma.h"
#include "bpe.h"
#include "channel.h"
#include "edge.h"
#include "vertex.h"
#include "system.h"
#include "rma/events.h"
#include "events/events.h"
#include "zcat.h"
#include "meta_edge.h"
#include "edges.h"
#include "vertices.h"
#include "rma.h"
#include "disintegrate.h"

#include "times.h"

/* refresh state */
uint32_t rule_system_epoch;
static llist work_queue;

//
// public
//

void rule_system_setup()
{
  llist_init(&work_queue);
  rule_system_epoch = 1;
}

void rule_system_reconcile(rule_run_context * restrict ctx)
{
  rule_system_work *work;
  rule_module_association *rma;
  moria_foreach_state st;
  moria_edge *edge;
  meta_edge *meta;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;

  vertices_init(&unlinking);
  vertices_init(&releasing);
  edges_init(&edges);

  /* run to quiescence */
  while(1) {
    work = llist_shift(&work_queue, typeof(*work), changed.lln);
    if(!work) {
      break;
    }

    rule_system_epoch++;
    rma = containerof(work, typeof(*rma), work);

    rma_reset(rma);
    ctx->rma = rma;

    /* rma-start */
    rma_operation_push(ctx);
    rma_event_start(ctx);

    rule_resolve(rma->rule, ctx);
    if(system_has_error) {
      goto clean;
    }
    ctx->fml = ctx->rma->fml;

#define RMA_INVALIDATE_SELF 0

#if RMA_INVALIDATE_SELF
    /* allow the rma to become invalidated during its execution */
    vertex_mark_fresh(&rma->vertex);
#endif

    /* rule-run has no failure cases */
    rule_run(rma->rule, ctx);
    RUNTIME_ASSERT(!system_has_error);

    /* resolve bpe exec refs */
    rma_resolve(rma);

    /* remove edges previously created by running this rma but which were not refreshed during this round */
    /* this can invalidate other rmas */
clean:
    moria_foreach_edge(&st, &rma->vertex, edge) {
      switch(edge_type_get(edge)) {
        case EDGE_RMA_BPE:
        case EDGE_FSNAME_RMA:
        case EDGE_RMA_BIND:
          break;
        default:
          continue;
      }
      meta = containerof(edge, typeof(*meta), edge);
      if(meta->epoch == rule_system_epoch) {
        continue;
      }

      switch(edge_type_get(edge)) {
        case EDGE_FSNAME_RMA:
        case EDGE_RMA_BIND:
          edges_push(&edges, edge);
        case EDGE_RMA_BPE:
          vertices_push(&unlinking, edge->A);
          break;
        default:
          continue;
      }
    }

    graph_invalidation_begin(&ctx->invalidation);
    graph_disintegrate(&edges, &unlinking, &releasing, &ctx->invalidation);
    graph_invalidation_end(&ctx->invalidation);

    /* rma-end */
    rma_event_end(ctx);
    rma_operation_pop(ctx);

    if(system_has_error) {
#if RMA_INVALIDATE_SELF
      vertex_invalidate(&rma->vertex, &ctx->invalidation);
#else
      llist_push(&work_queue, &rma->work.changed.lln);
#endif
      break;
    }

    vertex_mark_fresh(&rma->vertex);
  }
}

void rule_system_work_enqueue(rule_module_association * restrict rma)
{
  if(llist_attached(&rma->work.changed.lln)) {
    return;
  }

  llist_push(&work_queue, &rma->work.changed.lln);
}
