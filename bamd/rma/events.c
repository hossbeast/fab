/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "xlinux/xtime.h"
#include "xlinux/xstdlib.h"

#include "rma/events.h"
#include "events/events.h"
#include "rule.h"
#include "fsent.h"
#include "rma.h"
#include "times.h"
#include "fsname.h"
#include "search.internal.h"

static llist rma_op_freelist = LLIST_INITIALIZER(rma_op_freelist);

rma_operation *rma_operation_push(rule_run_context * restrict ctx)
{
  rma_operation *op;

  if(!(op = llist_shift(&rma_op_freelist, typeof(*op), lln))) {
    xmalloc(&op, sizeof(*op));
  } else {
    memset(op, 0, sizeof(*op));
  }

  llist_unshift(&ctx->operations, &op->lln);
  op->level = ctx->level++;

  return op;
}

void rma_operation_pop(rule_run_context * restrict ctx)
{
  rma_operation *op;

  op = llist_first(&ctx->operations, typeof(*op), lln);
  llist_delete(&op->lln);
  ctx->level--;

  llist_push(&rma_op_freelist, &op->lln);
}

//
//
//

void rma_event_start(rule_run_context * restrict ctx)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  rma_operation *op;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_RMA_START))) {
    return;
  }

  op = llist_first(&ctx->operations, typeof(*op), lln);
  xclock_gettime(CLOCK_MONOTONIC_COARSE, &op->start_time);
  narrator_fixed_init(&fixed, msg->text, msg->size - 2);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("rma-start")
    { SET
      { MAPPING STRING("level") UINT(op->level) }
      { MAPPING STRING("id") BYTES(ctx->rma->id, ctx->rma->id_len) }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;

  /* support in-place parsing */
  msg->text[msg->len + 0] = 0;
  msg->text[msg->len + 1] = 0;

  events_publish(&ectx);
}

void rma_event_end(rule_run_context * restrict ctx)
{
  char buf[128];
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  rma_operation *op;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_RMA_END))) {
    return;
  }

  op = llist_first(&ctx->operations, typeof(*op), lln);
  xclock_gettime(CLOCK_MONOTONIC_COARSE, &op->end_time);
  narrator_fixed_init(&fixed, msg->text, msg->size - 2);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("rma-end")
    { SET
      { MAPPING STRING("level") UINT(op->level) }
//      { MAPPING STRING("rma-id") BYTES(ctx->rma->id, ctx->rma->id_len) }
      { MAPPING STRING("runtime")
        timespec_ns_cat(buf, sizeof(buf), timespec_delta(&op->start_time, &op->end_time));
        STRING(buf)
      }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;

  /* support in-place parsing */
  msg->text[msg->len + 0] = 0;
  msg->text[msg->len + 1] = 0;

  events_publish(&ectx);
}

void rma_event_search(rule_run_context * restrict ctx, const pattern_search_hit *hit)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  rma_operation *op;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_RMA_SEARCH))) {
    return;
  }

  op = llist_first(&ctx->operations, typeof(*op), lln);
  narrator_fixed_init(&fixed, msg->text, msg->size - 2);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("rma-search")
    { SET
      { MAPPING STRING("level") UINT(op->level) }
      { MAPPING STRING("id")
        vertex_id_describe(&hit->node->vertex, builder_writer);
      }
      { MAPPING STRING("path")
        vertex_path_describe(&hit->node->vertex, builder_writer);
      }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;

  /* support in-place parsing */
  msg->text[msg->len + 0] = 0;
  msg->text[msg->len + 1] = 0;

  events_publish(&ectx);
}

void rma_event_generate(rule_run_context * restrict ctx, const struct moria_vertex *hit)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  rma_operation *op;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_RMA_GENERATE))) {
    return;
  }

  op = llist_first(&ctx->operations, typeof(*op), lln);
  narrator_fixed_init(&fixed, msg->text, msg->size - 2);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("rma-generate")
    { SET
      { MAPPING STRING("level") UINT(op->level) }
      { MAPPING STRING("id")
        vertex_id_describe(hit, builder_writer);
      }
      { MAPPING STRING("path")
        vertex_path_describe(hit, builder_writer);
      }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;

  /* support in-place parsing */
  msg->text[msg->len + 0] = 0;
  msg->text[msg->len + 1] = 0;

  events_publish(&ectx);
}

void rma_event_resolve(rule_run_context * restrict ctx)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  rma_operation *op;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_RMA_RESOLVE))) {
    return;
  }

  op = llist_first(&ctx->operations, typeof(*op), lln);
  narrator_fixed_init(&fixed, msg->text, msg->size - 2);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("rma-resolve")
    { SET
      { MAPPING STRING("level") UINT(op->level) }
      if(ctx->lctx.hit) {
        { MAPPING STRING("hit")
          { SET
            { MAPPING STRING("id")
              vertex_id_describe(&ctx->lctx.hit->vertex, builder_writer);
            }
            { MAPPING STRING("path")
              vertex_path_describe(&ctx->lctx.hit->vertex, builder_writer);
            }
          }
        }
      } else if(ctx->lctx.miss) {
        { MAPPING STRING("miss")
          { SET
            { MAPPING STRING("id")
              vertex_id_describe(&ctx->lctx.miss->vertex, builder_writer);
            }
            { MAPPING STRING("path")
              vertex_path_describe(&ctx->lctx.miss->vertex, builder_writer);
            }
          }
        }
      }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;

  /* support in-place parsing */
  msg->text[msg->len + 0] = 0;
  msg->text[msg->len + 1] = 0;

  events_publish(&ectx);
}

void rma_event_bind(
    struct rule_run_context * restrict ctx
  , const struct fsent * restrict attachment
  , const struct fsent * restrict target
  , const struct fsent * restrict binding
)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;
  rma_operation *op;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_RMA_BIND))) {
    return;
  }

  op = llist_first(&ctx->operations, typeof(*op), lln);
  narrator_fixed_init(&fixed, msg->text, msg->size - 2);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("rma-bind")
    { SET
      { MAPPING STRING("level") UINT(op->level) }
      { MAPPING STRING("attachment")
        { SET
          { MAPPING STRING("id")
            vertex_id_describe(&attachment->vertex, builder_writer);
          }
          { MAPPING STRING("path")
            vertex_path_describe(&attachment->vertex, builder_writer);
          }
        }
      }
      if(target) {
        { MAPPING STRING("target")
          { SET
            { MAPPING STRING("id")
              vertex_id_describe(&target->vertex, builder_writer);
            }
            { MAPPING STRING("path")
              vertex_path_describe(&target->vertex, builder_writer);
            }
          }
        }
      }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;

  /* support in-place parsing */
  msg->text[msg->len + 0] = 0;
  msg->text[msg->len + 1] = 0;

  events_publish(&ectx);
}
