/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "valyria/hashtable.h"
#include "narrator.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"
#include "moria/vertex.h"

#include "buildplan.h"
#include "dependency.h"
#include "formula.h"
#include "bpe.h"
#include "fsent.h"
#include "vertex.h"
#include "channel.h"
#include "request_handler.h"
#include "zcat.h"
#include "common/attrs.h"
#include "events/events.h"
#include "system.h"
#include "rma.h"
#include "shadow.h"
#include "subsys.h"

static uint16_t buildplan_counter;

selection buildplan_selection;
enum buildplan_state buildplan_state;

union stats_buildplan buildplan_stats;
subsys sys_plan;

typedef struct traverse_context {
  moria_vertex *stack[32];
  int32_t stack_depth;
} traverse_context;

static void error_build(bacon_writer * restrict builder_writer, const fsent * restrict fse, const traverse_context * restrict ctx)
{
  int x;

  { MAPPING
    STRING("cannot update")
    { SET
      { MAPPING
        STRING("trail")
        { LIST
          if(ctx && ctx->stack_depth >= 0) {
            for(x = 0; x <= ctx->stack_depth; x++) {
              vertex_identity_describe(ctx->stack[x], builder_writer);
            }
          } else {
            vertex_identity_describe(&fse->vertex, builder_writer);
          }
        }
      }
    }
  }
}

static void buildplan_error(const fsent * restrict fse, const traverse_context * restrict ctx)
{
  events_context ectx;
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  error_build(builder_writer, fse, ctx);

  bacon_writer_close(builder_writer);
  msg->len = fixed.l;
  msg->code = -1;
  events_publish(&ectx);
}

/*
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
static void buildplan_add(moria_vertex *v, int distance, traverse_context *ctx)
{
  fsent *fse;

  if(!vertex_invalid_get(v)) {
    return;
  }
  fse = containerof(v, fsent, vertex);

  if(!fse->bpe && vertex_shadow_get(v)) {
    /* special case ; shadow nodes can act as placeholders, even with no attached formula */
    selection_add_vertex(&buildplan_selection, &fse->vertex, distance);
    return;
  }

  /* this is an error case - node is not up to date, and theres no way to update it */
  if(!fse->bpe)
  {
    buildplan_state = UNSATISFIED;

    if(fse->bp_plan_id == buildplan_counter) {
      /* already issued the error for this node */
      return;
    }
    fse->bp_plan_id = buildplan_counter;

    buildplan_error(fse, ctx);
    return;
  }

  selection_add_bpe(&buildplan_selection, fse->bpe, distance);
}

/*
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
static void buildplan_visitor_direct(
    moria_vertex *v
  , void *arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  traverse_context *ctx;

  ctx = arg;

  /* tracking the path by which this node was visited */
  ctx->stack_depth = distance / 2;
  if(ctx->stack_depth < ARRAY_LEN(ctx->stack)) {
    ctx->stack[ctx->stack_depth] = v;
  } else {
    ctx->stack_depth = -1;
  }

  buildplan_add(v, distance, ctx);
}

void buildplan_setup()
{
  selection_init(&buildplan_selection);
  buildplan_counter = 1;

  subsys_init(&sys_plan, VERTEX_SYS_PLAN);
  shadow_mkref(g_shadow_bm, sys_plan.vertex.label, sys_plan.vertex.label_len, &sys_plan.vertex);
}

void buildplan_cleanup()
{
  selection_destroy(&buildplan_selection);
}

void buildplan_reset()
{
  selection_reset(&buildplan_selection, SELECTION_ITERATION_TYPE_RANK);
  buildplan_counter++;
  buildplan_state = NOOP;
}

void buildplan_finalize()
{
  selection_finalize(&buildplan_selection);
}

void buildplan_show()
{
  selected *sn;
  const buildplan_entity *bpe;

  printf("%3zu executions in %2hu stages\n", buildplan_selection.selected_entities.size, buildplan_selection.numranks);

  llist_foreach(&buildplan_selection.list, sn, lln) {
    printf("%-3d ", sn->rank);
    switch(sn->type) {
      case SELECTED_BPE:
        bpe = sn->bpe;
        buildplan_entity_say_targets(bpe, g_narrator_stdout);
        break;
      case SELECTED_VERTEX:
        fsent_absolute_path_say(containerof(sn->v, fsent, vertex), g_narrator_stdout);
        break;
      default:
        RTA;
    }
    printf("\n");
  }
}

void buildplan_add_transitive(moria_vertex * restrict v, moria_vertex_traversal_state * restrict state)
{
  traverse_context ctx;

  moria_traverse_vertices(
      &g_graph
    , v
    , buildplan_visitor_direct
    , state
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_DEPENDS
        , vertex_visit_mask : VERTEX_KIND_OPT
        , vertex_visit : VERTEX_KIND_FSENT
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_EXHAUSTIVE | MORIA_TRAVERSE_NOFOLLOW
    , &ctx
  );
}

void buildplan_select_transitive(llist * restrict selection)
{
  selected *sn;
  moria_vertex_traversal_state *state;
  fsent *fse;

  moria_vertex_traversal_begin(&g_graph, &state);
  llist_foreach(selection, sn, lln) {
    fse = containerof(sn->v, typeof(*fse), vertex);
    if(!fse->bpe && !vertex_shadow_get(&fse->vertex)) {
      buildplan_error(fse, 0);
      break;
    }
    buildplan_add_transitive(sn->v, state);
  }
  moria_vertex_traversal_end(&g_graph, state);
}

void buildplan_select_direct(llist * restrict selection)
{
  selected *sn;
  fsent *fse;

  llist_foreach(selection, sn, lln) {
    fse = containerof(sn->v, typeof(*fse), vertex);
    if(!fse->bpe && !vertex_shadow_get(&fse->vertex)) {
      buildplan_error(fse, 0);
      break;
    }
    buildplan_add(sn->v, 0, 0);
  }
}

void plan_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&buildplan_stats.record, &descriptor_stats_buildplan, builder_writer);
}

void plan_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      vertex_mappings(v, builder_writer);
      { MAPPING
        STRING("stats")
        { SET
          plan_sys_stats_build(v, false, builder_writer);
        }
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}

void plan_sys_tell(const moria_vertex * restrict v, struct request_handler * restrict handler)
{
  bamipc_message *msg;
  selected *sn;
  const buildplan_entity *bpe;
  char *dst;
  size_t sz;
  size_t z;

  llist_foreach(&buildplan_selection.list, sn, lln) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);
    dst = msg->text;
    sz = msg->size;
    z = 0;

    z += zcatf(dst + z, sz - z, "%-3d ", sn->rank);
    switch(sn->type) {
      case SELECTED_BPE:
        bpe = sn->bpe;
        z += zcatf(dst + z, sz - z, "%s -> ", bpe->fml->self_node->name.name);
        z += buildplan_entity_targets_cat(dst + z, sz - z, bpe);
        break;
      case SELECTED_VERTEX:
        z += fsent_absolute_path_cat(dst + z, sz - z, containerof(sn->v, fsent, vertex));
        break;
        RTA;
    }
    msg->len = z;
    request_handler_post(handler, msg);
  }
}
